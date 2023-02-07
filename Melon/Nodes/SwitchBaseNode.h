#pragma once

#include "Node.h"

#include "Boxx/Math.h"

#include "KiwiMemoryExpression.h"
#include "BreakStatement.h"
#include "TypeExpression.h"
#include "KiwiVariable.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/SymbolTable.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		template <class T>
		concept BaseSwitchType = std::is_same<T, Expression>::value || std::is_same<T, Statement>::value;

		template <class T>
		concept BaseSwitchType2 = std::is_same<T, Expression>::value || std::is_same<T, Statement>::value || std::is_same<T, Statements>::value;

		/// Abstract base node for {switch}.
		template <BaseSwitchType T, BaseSwitchType2 U = T>
		class SwitchBaseNode : public T {
		public:
			/// The match expression.
			Ptr<Expression> match;

			/// The nodes for all the cases.
			Boxx::List<Ptr<U>> nodes;

			/// All case expressions.
			Boxx::List<Boxx::List<Ptr<Expression>>> cases;

			/// The default node.
			Ptr<U> def;

			SwitchBaseNode(Symbols::Symbol* const scope, const FileInfo& file);
			~SwitchBaseNode();

			/// Scan info for switch nodes.
			struct SwitchScanInfo {
				/// {true} if the {switch} is in a constructor.
				bool init = false;

				/// Scope info.
				ScopeInfo scope;

				/// Scope info for all cases.
				Boxx::List<ScopeInfo> cases;

				/// {true} if at least one case is guaranteed to run.
				bool willACaseRun = false;
			};

			/// Compile info for {switch} nodes.
			struct SwitchCompileInfo {
				/// The compiled node.
				CompiledNode cn;

				/// The memory location of the match value.
				Ptr<KiwiMemoryExpression> match;

				/// Indices for jump to end.
				Boxx::List<Boxx::UInt> endJumps;

				/// Indicies for jump to case contents.
				Boxx::List<Boxx::List<Boxx::UInt>> caseJumps;

				/// The current case index.
				Boxx::UInt caseIndex;

				/// The jump to the default case.
				Boxx::UInt defaultJump;

				/// Result argument.
				KiwiOld::Argument result;

				/// Result node.
				Ptr<KiwiMemoryExpression> resultNode;
			};


			virtual Boxx::UInt GetSize() const override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual CompiledNode Compile(OldCompileInfo& info) override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			Symbols::TypeSymbol* SwitchType() const;
			Ptr<T> OptimizeSwitch(OptimizeInfo& info);

			virtual Symbols::NameList FindSideEffectScope(const bool assign);

			SwitchScanInfo ScanSetup(ScanInfo& info) const;

			void ScanPreContents(SwitchScanInfo& switchInfo, ScanInfo& info) const;

			void ScanPostContents(SwitchScanInfo& switchInfo, ScanInfo& info) const;

			void ScanCleanup(SwitchScanInfo& switchInfo, ScanInfo& info) const;

			ScanResult ScanNodes(ScanInfoStack& info) const;

			void CompileCaseMatches(SwitchCompileInfo& switchInfo, OldCompileInfo& info);
			void CompileCaseBodies(SwitchCompileInfo& switchInfo, OldCompileInfo& info);
			void CompileDefault(SwitchCompileInfo& switchInfo, OldCompileInfo& info);
		};

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline SwitchBaseNode<T, U>::SwitchBaseNode(Symbols::Symbol* const scope, const FileInfo& file) : T(scope, file) {

		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline SwitchBaseNode<T, U>::~SwitchBaseNode() {

		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline Symbols::TypeSymbol* SwitchBaseNode<T, U>::SwitchType() const {
			return nullptr;
		}

		template <>
		inline Symbols::TypeSymbol* SwitchBaseNode<Expression, Expression>::SwitchType() const {
			Symbols::TypeSymbol* type = nodes[0]->Type();

			for (Weak<Expression> node : nodes) {
				if (type != node->Type()) {
					ErrorLog::Error(LogMessage("error.type.switch"), this->File());
				}
			}

			return type;
		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline Boxx::UInt SwitchBaseNode<T, U>::GetSize() const {
			if constexpr (std::is_same<U, Expression>::value) return 0;

			Boxx::UInt size = 0;

			for (Weak<U> node : nodes) {
				size = Boxx::Math::Max(size, node->GetSize());
			}

			return size;
		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline void SwitchBaseNode<T, U>::CompileCaseMatches(SwitchCompileInfo& switchInfo, OldCompileInfo& info) {
			// Compile cases
			for (const Boxx::List<Ptr<Expression>>& values : cases) {
				Boxx::List<Boxx::UInt> jumps;

				// Compile case values
				for (Weak<Expression> node : values) {
					Symbols::FunctionSymbol* const sym = Symbols::SymbolTable::FindOperator(Symbols::Name::Equal, this->match->Type(), node->Type(), node->File());
					CompiledNode comp = sym->symbolNode->Compile(switchInfo.match, node, info);
					switchInfo.cn.AddInstructions(comp.instructions);

					KiwiOld::Instruction eq = KiwiOld::Instruction(KiwiOld::InstructionType::Ne, 1);
					eq.arguments.Add(comp.argument);
					eq.arguments.Add(KiwiOld::Argument(0));
					switchInfo.cn.instructions.Add(eq);

					jumps.Add(switchInfo.cn.instructions.Count() - 1);
				}

				switchInfo.caseJumps.Add(jumps);
			}
		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline void SwitchBaseNode<T, U>::CompileCaseBodies(SwitchCompileInfo& switchInfo, OldCompileInfo& info) {
			// Compile nodes
			for (Weak<U> expr : nodes) {
				// Add label for case
				switchInfo.cn.instructions.Add(KiwiOld::Instruction::Label(info.label));

				for (const Boxx::UInt i : switchInfo.caseJumps[switchInfo.caseIndex]) {
					switchInfo.cn.instructions[i].instruction.arguments.Add(KiwiOld::Argument(KiwiOld::ArgumentType::Label, info.label));
				}

				info.label++;
				switchInfo.caseIndex++;

				// Compile statements
				if constexpr (!std::is_same<U, Expression>::value) {
					CompiledNode compExpr = expr->Compile(info);

					// Check for scopewise breaks
					for (const Optimizing::OptimizerInstruction& in : compExpr.instructions) {
						if (in.instruction.type != KiwiOld::InstructionType::Custom) {
							switchInfo.cn.instructions.Add(in);
							continue;
						}

						const Boxx::String type = in.instruction.instructionName;

						if (type != BreakStatement::scopeBreakInstName) {
							switchInfo.cn.instructions.Add(in);
							continue;
						}

						if (in.instruction.sizes[0] > 1) {
							Optimizing::OptimizerInstruction inst = in;
							inst.instruction.sizes[0]--;
							switchInfo.cn.instructions.Add(inst);
						}
						else {
							switchInfo.endJumps.Add(switchInfo.cn.instructions.Count());
							switchInfo.cn.instructions.Add(KiwiOld::Instruction(KiwiOld::InstructionType::Jmp, 0));
						}
					}
				}
				// Compile expression
				else {
					switchInfo.cn.AddInstructions(this->CompileAssignment(switchInfo.resultNode, expr, info, expr->File()).instructions);
				}

				// Jump to end
				switchInfo.endJumps.Add(switchInfo.cn.instructions.Count());
				switchInfo.cn.instructions.Add(KiwiOld::Instruction(KiwiOld::InstructionType::Jmp, 0));
			}
		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline void SwitchBaseNode<T, U>::CompileDefault(SwitchCompileInfo& switchInfo, OldCompileInfo& info) {
			if (def) {
				// Compile statements
				if constexpr (!std::is_same<U, Expression>::value) {
					CompiledNode defNode = def->Compile(info);

					// Check for scopewise breaks
					for (const Optimizing::OptimizerInstruction& in : defNode.instructions) {
						if (in.instruction.type != KiwiOld::InstructionType::Custom) {
							switchInfo.cn.instructions.Add(in);
							continue;
						}

						const Boxx::String type = in.instruction.instructionName;

						if (type != BreakStatement::scopeBreakInstName) {
							switchInfo.cn.instructions.Add(in);
							continue;
						}

						if (in.instruction.sizes[0] > 1) {
							Optimizing::OptimizerInstruction inst = in;
							inst.instruction.sizes[0]--;
							switchInfo.cn.instructions.Add(inst);
						}
						else {
							switchInfo.endJumps.Add(switchInfo.cn.instructions.Count());
							switchInfo.cn.instructions.Add(KiwiOld::Instruction(KiwiOld::InstructionType::Jmp, 0));
						}
					}
				}
				// Compile expression
				else {
					switchInfo.cn.AddInstructions(this->CompileAssignment(switchInfo.resultNode, def, info, def->File()).instructions);
				}
			}
		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline CompiledNode SwitchBaseNode<T, U>::Compile(OldCompileInfo& info) {
			// Setup compile info
			SwitchCompileInfo switchInfo;
			switchInfo.caseIndex = 0;

			// Get size of return type
			if constexpr (std::is_same<U, Expression>::value) {
				switchInfo.cn.size = SwitchType()->Size();
			}

			// Compile match expression
			info.stack.PushExpr(this->match->Type()->Size(), switchInfo.cn);
			switchInfo.match = new KiwiMemoryExpression(info.stack.Offset(), this->match->Type()->AbsoluteName());

			const Boxx::UInt frame = info.stack.frame;
			switchInfo.cn.AddInstructions(this->CompileAssignment(switchInfo.match, this->match, info, this->match->File()).instructions);

			// Compile cases
			CompileCaseMatches(switchInfo, info);

			if constexpr (!std::is_same<U, Expression>::value) {
				info.stack.PopExpr(frame, switchInfo.cn);
			}

			info.stack.Pop(this->match->Type()->Size());

			// Jump to default
			KiwiOld::Instruction defJmp = KiwiOld::Instruction(KiwiOld::InstructionType::Jmp, 0);
			switchInfo.defaultJump = switchInfo.cn.instructions.Count();
			switchInfo.cn.instructions.Add(defJmp);

			if constexpr (std::is_same<U, Expression>::value) {
				info.stack.PushExpr(SwitchType()->Size(), switchInfo.cn);

				// Get result memory location
				switchInfo.result = KiwiOld::Argument(KiwiOld::MemoryLocation(info.stack.Offset()));
				switchInfo.resultNode = new KiwiMemoryExpression(switchInfo.result.mem.offset, SwitchType()->AbsoluteName());
			}

			// Compile case bodies
			CompileCaseBodies(switchInfo, info);

			// Add default label
			switchInfo.cn.instructions.Add(KiwiOld::Instruction::Label(info.label));
			switchInfo.cn.instructions[switchInfo.defaultJump].instruction.arguments.Add(KiwiOld::Argument(KiwiOld::ArgumentType::Label, info.label));
			info.label++;

			// Compile default case
			CompileDefault(switchInfo, info);

			// Add end label
			switchInfo.cn.instructions.Add(KiwiOld::Instruction::Label(info.label));

			for (const Boxx::UInt i : switchInfo.endJumps) {
				switchInfo.cn.instructions[i].instruction.arguments.Add(KiwiOld::Argument(KiwiOld::ArgumentType::Label, info.label));
			}

			info.label++;

			// Set result argument
			if constexpr (std::is_same<U, Expression>::value) {
				switchInfo.cn.argument = switchInfo.result;
				info.stack.Pop(SwitchType()->Size());
			}

			return switchInfo.cn;
		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline Ptr<Kiwi::Value> SwitchBaseNode<T, U>::Compile(CompileInfo& info) {
			Ptr<Kiwi::Value> matchValue = this->match->Compile(info);
			Ptr<Kiwi::Variable> match = new Kiwi::Variable(info.NewRegister());
			Ptr<KiwiVariable> kiwiVar = new KiwiVariable(match->Copy(), this->match->Type()->AbsoluteName());
			info.AddInstruction(new Kiwi::AssignInstruction(this->match->Type()->KiwiType(), match->Copy(), matchValue));

			Boxx::List<Boxx::String> labels;

			Ptr<Kiwi::Variable> result;

			if constexpr (std::is_same<U, Expression>::value) {
				result = new Kiwi::Variable(info.NewRegister());
				info.AddInstruction(new Kiwi::AssignInstruction(SwitchType()->KiwiType(), result->Copy(), nullptr));
			}
			
			for (const Boxx::List<Ptr<Expression>>& values : cases) {
				labels.Add(info.NewLabel());

				for (Weak<Expression> expr : values) {
					Symbols::FunctionSymbol* const sym = Symbols::SymbolTable::FindOperator(Symbols::Name::Equal, this->match->Type(), expr->Type(), expr->File()); 
					Ptr<Kiwi::Value> comp = sym->symbolNode->Compile(kiwiVar, expr, info, false);
					info.AddInstruction(new Kiwi::IfInstruction(comp, labels.Last()));
				}
			}

			Boxx::String endLbl = info.NewLabel();
			Boxx::String defaultLbl;
			
			if (def) {
				defaultLbl = info.NewLabel();
				info.AddInstruction(new Kiwi::GotoInstruction(defaultLbl));
			}
			else {
				info.AddInstruction(new Kiwi::GotoInstruction(endLbl));
			}

			for (Boxx::UInt i = 0; i < nodes.Count(); i++) {
				info.NewInstructionBlock(labels[i]);
				Ptr<Kiwi::Value> value = nodes[i]->Compile(info);

				if constexpr (std::is_same<U, Expression>::value) {
					info.AddInstruction(new Kiwi::AssignInstruction(result->Copy(), value));
				}

				info.AddInstruction(new Kiwi::GotoInstruction(endLbl));
			}

			if (def) {
				info.NewInstructionBlock(defaultLbl);
				Ptr<Kiwi::Value> value = def->Compile(info);

				if constexpr (std::is_same<U, Expression>::value) {
					info.AddInstruction(new Kiwi::AssignInstruction(result->Copy(), value));
				}
			}

			info.NewInstructionBlock(endLbl);

			return result;
		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline void SwitchBaseNode<T, U>::IncludeScan(Parsing::ParsingInfo& info) {
			match->IncludeScan(info);

			for (Weak<U> node : nodes) {
				node->IncludeScan(info);
			}

			for (Boxx::List<Ptr<Expression>> caseList : cases) {
				for (Weak<Expression> c : caseList) {
					c->IncludeScan(info);
				}
			}
		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline SwitchBaseNode<T, U>::SwitchScanInfo SwitchBaseNode<T, U>::ScanSetup(ScanInfo& info) const {
			SwitchScanInfo switchInfo;
			switchInfo.init = info.init;

			if constexpr (!std::is_same<U, Expression>::value) {
				switchInfo.willACaseRun = (bool)def;
				switchInfo.scope = info.scopeInfo.CopyBranch();

				if (switchInfo.init) {
					switchInfo.scope.unassigned = info.type->UnassignedMembers();
				}
			}

			return switchInfo;
		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline void SwitchBaseNode<T, U>::ScanPreContents(SwitchScanInfo& switchInfo, ScanInfo& info) const {
			if constexpr (!std::is_same<U, Expression>::value) {
				if (switchInfo.init) {
					info.init = true;

					for (const Symbols::Name& var : switchInfo.scope.unassigned) {
						if (Symbols::VariableSymbol* const v = info.type->Find<Symbols::VariableSymbol>(var, FileInfo())) {
							v->isAssigned = false;
						}
					}
				}

				info.scopeInfo = switchInfo.scope.CopyBranch();
			}
		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline void SwitchBaseNode<T, U>::ScanPostContents(SwitchScanInfo& switchInfo, ScanInfo& info) const {
			if constexpr (!std::is_same<U, Expression>::value) {
				if (switchInfo.init) {
					info.scopeInfo.unassigned = info.type->UnassignedMembers();
				}

				switchInfo.cases.Add(info.scopeInfo);
			}
		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline void SwitchBaseNode<T, U>::ScanCleanup(SwitchScanInfo& switchInfo, ScanInfo& info) const {
			if constexpr (!std::is_same<U, Expression>::value) {
				for (Boxx::UInt i = 0; i < switchInfo.cases.Count(); i++) {
					if (i == 0) {
						switchInfo.scope = switchInfo.cases[i];
					}
					else if (switchInfo.willACaseRun) {
						switchInfo.scope = ScopeInfo::BranchIntersection(switchInfo.scope, switchInfo.cases[i]);
					}
					else {
						switchInfo.scope = ScopeInfo::WeakBranchIntersection(switchInfo.scope, switchInfo.cases[i]);
					}
				}

				if (switchInfo.init) {
					for (const Symbols::Name& var : switchInfo.scope.unassigned) {
						if (Symbols::VariableSymbol* const v = info.type->Find<Symbols::VariableSymbol>(var, FileInfo())) {
							v->isAssigned = true;
						}
					}

					for (const Symbols::Name& var : switchInfo.scope.unassigned) {
						if (Symbols::VariableSymbol* const v = info.type->Find<Symbols::VariableSymbol>(var, FileInfo())) {
							v->isAssigned = false;
						}
					}
				}

				info.scopeInfo = switchInfo.scope;
			}
		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline ScanResult SwitchBaseNode<T, U>::ScanNodes(ScanInfoStack& info) const {
			ScanResult result;

			// Setup scan
			SwitchScanInfo switchInfo = ScanSetup(info.Get());

			// Get type of switch expression
			Symbols::TypeSymbol* const type = SwitchType();
			Ptr<TypeExpression> typeNode = nullptr;

			if (type) {
				typeNode = new TypeExpression(type->AbsoluteName());
			}

			// Scan regular case nodes
			for (Weak<U> node : nodes) {
				ScanPreContents(switchInfo, info.Get());

				ScanResult r = node->Scan(info);
				r.SelfUseCheck(info, node->File());
				result |= r;

				if constexpr (std::is_same<U, Expression>::value) {
					if (typeNode) {
						this->ScanAssignment(typeNode, node, info, node->File());
					}
				}

				ScanPostContents(switchInfo, info.Get());
			}

			// Scan default node
			if (def) {
				ScanPreContents(switchInfo, info.Get());

				ScanResult r = def->Scan(info);
				r.SelfUseCheck(info, def->File());
				result |= r;

				if constexpr (std::is_same<U, Expression>::value) {
					if (typeNode) {
						this->ScanAssignment(typeNode, def, info, def->File());
					}
				}

				ScanPostContents(switchInfo, info.Get());
			}

			// Cleanup scan
			ScanCleanup(switchInfo, info.Get());

			return result;
		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline ScanResult SwitchBaseNode<T, U>::Scan(ScanInfoStack& info) {
			ScanResult result = match->Scan(info);
			result.SelfUseCheck(info, match->File());

			// Scan match type
			Symbols::TypeSymbol* const matchType = match->Type();

			if (matchType) {
				Ptr<TypeExpression> type1 = new TypeExpression(matchType->AbsoluteName());
				Ptr<TypeExpression> type2 = new TypeExpression(matchType->AbsoluteName());
				this->ScanAssignment(type1, type2, info, match->File());
			}

			// Scan Nodes
			result |= ScanNodes(info);

			// Scan case values
			for (const Boxx::List<Ptr<Expression>>& nodeList : cases) {
				for (Weak<Expression> node : nodeList) {
					ScanResult r = node->Scan(info);
					r.SelfUseCheck(info, node->File());
					result |= r;

					Symbols::SymbolTable::FindOperator(Symbols::Name::Equal, matchType, node->Type(), node->File());
				}
			}

			return result;
		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline Symbols::NameList SwitchBaseNode<T, U>::FindSideEffectScope(const bool assign) {
			Symbols::NameList list = match->GetSideEffectScope(assign);

			for (Weak<U> node : nodes) {
				list = this->CombineSideEffects(list, node->GetSideEffectScope(assign));
			}

			for (Boxx::List<Ptr<Expression>>& caseList : cases) {
				for (Weak<Expression> node : caseList) {
					list = this->CombineSideEffects(list, node->GetSideEffectScope(assign));
				}
			}

			if (def) {
				list = this->CombineSideEffects(list, def->GetSideEffectScope(assign));
			}

			return list;
		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline Ptr<T> SwitchBaseNode<T, U>::OptimizeSwitch(OptimizeInfo& info) {
			Node::Optimize(match, info);

			for (Ptr<U>& node : nodes) {
				Node::Optimize(node, info);
			}

			for (Boxx::List<Ptr<Expression>>& caseList : cases) {
				for (Ptr<Expression>& node : caseList) {
					Node::Optimize(node, info);
				}
			}

			if (def) {
				Node::Optimize(def, info);
			}

			// TODO: Check for side effects in cases

			// Remove empty cases
			if constexpr (!std::is_same<U, Expression>::value) {
				for (Boxx::UInt i = 0; i < nodes.Count(); i++) {
					if (Node::IsEmpty(nodes[i])) {
						cases.RemoveAt(i);
						nodes.RemoveAt(i);
						i--;
						info.optimized = true;
					}
				}

				if (def && Node::IsEmpty(def)) {
					def = nullptr;
					info.optimized = true;
				}
			}

			return nullptr;
		}

		template <BaseSwitchType T, BaseSwitchType2 U>
		inline Boxx::StringBuilder SwitchBaseNode<T, U>::ToMelon(const Boxx::UInt indent) const {
			Boxx::StringBuilder sb = "switch ";
			sb += match->ToMelon(indent);

			Boxx::String tabs1 = Boxx::String('\t').Repeat(indent + 1);
			Boxx::String tabs2 = Boxx::String('\t').Repeat(indent + 2);

			for (Boxx::UInt i = 0; i < nodes.Count(); i++) {
				sb += "\n";
				sb += tabs1;
				sb += "case ";

				for (Boxx::UInt u = 0; u < cases[i].Count(); u++) {
					if (u > 0) sb += ", ";
					sb += cases[i][u]->ToMelon(indent + 1);
				}

				sb += " then\n";
				sb += tabs2;
				sb += nodes[i]->ToMelon(indent + 2);
				sb += "\n";
				sb += tabs1;
				sb += "end\n";
			}

			if (def) {
				sb += "\n";
				sb += tabs1;
				sb += "default\n";
				sb += tabs2;
				sb += def->ToMelon(indent + 2);
				sb += "\n";
				sb += tabs1;
				sb += "end\n";
			}

			sb += Boxx::String('\t').Repeat(indent);
			sb += "end";

			return sb;
		}
	}
}
