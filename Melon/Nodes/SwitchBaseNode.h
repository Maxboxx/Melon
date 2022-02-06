#pragma once

#include "Node.h"
#include "MemoryNode.h"

#include "Boxx/Math.h"

#include "MemoryNode.h"
#include "BreakNode.h"
#include "TypeNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/SymbolTable.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Abstract base node for {switch}.
		template <class T>
		class SwitchBaseNode : public T {
		public:
			/// The match expression.
			_Expression_ match;

			/// The nodes for all the cases.
			Boxx::List<Boxx::Pointer<T>> nodes;

			/// All case expressions.
			Boxx::List<Boxx::List<_Expression_>> cases;

			/// The default node.
			Boxx::Pointer<T> def;

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
				Boxx::Pointer<KiwiMemoryExpression> match;

				/// Indices for jump to end.
				Boxx::List<Boxx::UInt> endJumps;

				/// Indicies for jump to case contents.
				Boxx::List<Boxx::List<Boxx::UInt>> caseJumps;

				/// The current case index.
				Boxx::UInt caseIndex;

				/// The jump to the default case.
				Boxx::UInt defaultJump;

				/// Result argument.
				Kiwi::Argument result;

				/// Result node.
				Boxx::Pointer<KiwiMemoryExpression> resultNode;
			};


			virtual Boxx::UInt GetSize() const override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			Symbols::TypeSymbol* SwitchType() const;
			Boxx::Pointer<T> OptimizeSwitch(OptimizeInfo& info);

			virtual Symbols::NameList FindSideEffectScope(const bool assign);

			SwitchScanInfo ScanSetup(ScanInfo& info) const;

			void ScanPreContents(SwitchScanInfo& switchInfo, ScanInfo& info) const;

			void ScanPostContents(SwitchScanInfo& switchInfo, ScanInfo& info) const;

			void ScanCleanup(SwitchScanInfo& switchInfo, ScanInfo& info) const;

			ScanResult ScanNodes(ScanInfoStack& info) const;

			void CompileCaseMatches(SwitchCompileInfo& switchInfo, CompileInfo& info);
			void CompileCaseBodies(SwitchCompileInfo& switchInfo, CompileInfo& info);
			void CompileDefault(SwitchCompileInfo& switchInfo, CompileInfo& info);

			bool expr;
		};

		template <class T>
		inline SwitchBaseNode<T>::SwitchBaseNode(Symbols::Symbol* const scope, const FileInfo& file) : T(scope, file) {

		}

		template <class T>
		inline SwitchBaseNode<T>::~SwitchBaseNode() {

		}

		template <class T>
		inline Symbols::TypeSymbol* SwitchBaseNode<T>::SwitchType() const {
			if (!expr) return nullptr;

			Symbols::TypeSymbol* type = nodes[0]->Type();

			for (Boxx::Pointer<T>& node : nodes) {
				if (type != node->Type()) {
					ErrorLog::Error(LogMessage("error.type.switch"), File());
				}
			}

			return type;
		}

		template <class T>
		inline Boxx::UInt SwitchBaseNode<T>::GetSize() const {
			if (expr) return 0;

			Boxx::UInt size = 0;

			for (const Boxx::Pointer<Node>& node : nodes) {
				size = Boxx::Math::Max(size, node->GetSize());
			}

			return size;
		}

		template <class T>
		inline void SwitchBaseNode<T>::CompileCaseMatches(SwitchCompileInfo& switchInfo, CompileInfo& info) {
			// Compile cases
			for (const Boxx::List<Boxx::Pointer<Node>>& values : cases) {
				Boxx::List<Boxx::UInt> jumps;

				// Compile case values
				for (_Expression_ node : values) {
					Boxx::List<_Expression_> nodeArgs;
					nodeArgs.Add(switchInfo.match);
					nodeArgs.Add(node);

					CompiledNode comp = Symbols::SymbolTable::FindOperator(Symbols::Name::Equal, this->match->Type(), node->Type(), node->File())->symbolNode->Compile(nodeArgs, info);
					switchInfo.cn.AddInstructions(comp.instructions);

					Kiwi::Instruction eq = Kiwi::Instruction(Kiwi::InstructionType::Ne, 1);
					eq.arguments.Add(comp.argument);
					eq.arguments.Add(Kiwi::Argument(0));
					switchInfo.cn.instructions.Add(eq);

					jumps.Add(switchInfo.cn.instructions.Size() - 1);
				}

				switchInfo.caseJumps.Add(jumps);
			}
		}

		template <class T>
		inline void SwitchBaseNode<T>::CompileCaseBodies(SwitchCompileInfo& switchInfo, CompileInfo& info) {
			// Compile nodes
			for (Boxx::Pointer<Node> expr : nodes) {
				// Add label for case
				switchInfo.cn.instructions.Add(Kiwi::Instruction::Label(info.label));

				for (const Boxx::UInt i : switchInfo.caseJumps[switchInfo.caseIndex]) {
					switchInfo.cn.instructions[i].instruction.arguments.Add(Kiwi::Argument(Kiwi::ArgumentType::Label, info.label));
				}

				info.label++;
				switchInfo.caseIndex++;

				// Compile statements
				if (!this->expr) {
					CompiledNode compExpr = expr->Compile(info);

					// Check for scopewise breaks
					for (const Optimizing::OptimizerInstruction& in : compExpr.instructions) {
						if (in.instruction.type != Kiwi::InstructionType::Custom) {
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
							switchInfo.endJumps.Add(switchInfo.cn.instructions.Size());
							switchInfo.cn.instructions.Add(Kiwi::Instruction(Kiwi::InstructionType::Jmp, 0));
						}
					}
				}
				// Compile expression
				else {
					switchInfo.cn.AddInstructions(CompileAssignment(switchInfo.resultNode, expr, info, expr->file).instructions);
				}

				// Jump to end
				switchInfo.endJumps.Add(switchInfo.cn.instructions.Size());
				switchInfo.cn.instructions.Add(Kiwi::Instruction(Kiwi::InstructionType::Jmp, 0));
			}
		}

		template <class T>
		inline void SwitchBaseNode<T>::CompileDefault(SwitchCompileInfo& switchInfo, CompileInfo& info) {
			if (def) {
				// Compile statements
				if (!this->expr) {
					CompiledNode defNode = def->Compile(info);

					// Check for scopewise breaks
					for (const Optimizing::OptimizerInstruction& in : defNode.instructions) {
						if (in.instruction.type != Kiwi::InstructionType::Custom) {
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
							switchInfo.endJumps.Add(switchInfo.cn.instructions.Size());
							switchInfo.cn.instructions.Add(Kiwi::Instruction(Kiwi::InstructionType::Jmp, 0));
						}
					}
				}
				// Compile expression
				else {
					switchInfo.cn.AddInstructions(CompileAssignment(switchInfo.resultNode, def, info, def->file).instructions);
				}
			}
		}

		template <class T>
		inline CompiledNode SwitchBaseNode<T>::Compile(CompileInfo& info) {
			// Setup compile info
			SwitchCompileInfo switchInfo;
			switchInfo.caseIndex = 0;

			// Get size of return type
			if (this->expr) {
				switchInfo.cn.size = Type()->Size();
			}

			// Compile match expression
			info.stack.PushExpr(this->match->Type()->Size(), switchInfo.cn);
			switchInfo.match = new KiwiMemoryExpression(info.stack.Offset());
			switchInfo.match->type = this->match->Type()->AbsoluteName();

			const Boxx::UInt frame = info.stack.frame;
			switchInfo.cn.AddInstructions(CompileAssignment(switchInfo.match, this->match, info, this->match->file).instructions);

			// Compile cases
			CompileCaseMatches(switchInfo, info);

			if (!this->expr) {
				info.stack.PopExpr(frame, switchInfo.cn);
			}

			info.stack.Pop(this->match->Type()->Size());

			// Jump to default
			Kiwi::Instruction defJmp = Kiwi::Instruction(Kiwi::InstructionType::Jmp, 0);
			switchInfo.defaultJump = switchInfo.cn.instructions.Size();
			switchInfo.cn.instructions.Add(defJmp);

			if (expr) {
				info.stack.PushExpr(SwitchType()->Size(), switchInfo.cn);

				// Get result memory location
				switchInfo.result = Kiwi::Argument(Kiwi::MemoryLocation(info.stack.Offset()));
				switchInfo.resultNode = new KiwiMemoryExpression(switchInfo.result.mem.offset);
				switchInfo.resultNode->type = SwitchType()->AbsoluteName();
			}

			// Compile case bodies
			CompileCaseBodies(switchInfo, info);

			// Add default label
			switchInfo.cn.instructions.Add(Kiwi::Instruction::Label(info.label));
			switchInfo.cn.instructions[switchInfo.defaultJump].instruction.arguments.Add(Kiwi::Argument(Kiwi::ArgumentType::Label, info.label));
			info.label++;

			// Compile default case
			CompileDefault(switchInfo, info);

			// Add end label
			switchInfo.cn.instructions.Add(Kiwi::Instruction::Label(info.label));

			for (const Boxx::UInt i : switchInfo.endJumps) {
				switchInfo.cn.instructions[i].instruction.arguments.Add(Kiwi::Argument(Kiwi::ArgumentType::Label, info.label));
			}

			info.label++;

			// Set result argument
			if (expr) {
				switchInfo.cn.argument = switchInfo.result;
				info.stack.Pop(Type()->Size());
			}

			return switchInfo.cn;
		}

		template <class T>
		inline void SwitchBaseNode<T>::IncludeScan(Parsing::ParsingInfo& info) {
			match->IncludeScan(info);

			for (Boxx::Pointer<Node> node : nodes) {
				node->IncludeScan(info);
			}

			for (Boxx::List<Boxx::Pointer<Node>>& caseList : cases) {
				for (Boxx::Pointer<Node> c : caseList) {
					c->IncludeScan(info);
				}
			}
		}

		template <class T>
		inline SwitchBaseNode<T>::SwitchScanInfo SwitchBaseNode<T>::ScanSetup(ScanInfo& info) const {
			SwitchScanInfo switchInfo;
			switchInfo.init = info.init;

			if (!expr) {
				switchInfo.willACaseRun = def != nullptr;
				switchInfo.scope = info.scopeInfo.CopyBranch();

				if (switchInfo.init) {
					switchInfo.scope.unassigned = info.type->UnassignedMembers();
				}
			}

			return switchInfo;
		}

		template <class T>
		inline void SwitchBaseNode<T>::ScanPreContents(SwitchScanInfo& switchInfo, ScanInfo& info) const {
			if (!expr) {
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

		template <class T>
		inline void SwitchBaseNode<T>::ScanPostContents(SwitchScanInfo& switchInfo, ScanInfo& info) const {
			if (!expr) {
				if (switchInfo.init) {
					info.scopeInfo.unassigned = info.type->UnassignedMembers();
				}

				switchInfo.cases.Add(info.scopeInfo);
			}
		}

		template <class T>
		inline void SwitchBaseNode<T>::ScanCleanup(SwitchScanInfo& switchInfo, ScanInfo& info) const {
			if (!expr) {
				for (Boxx::UInt i = 0; i < switchInfo.cases.Size(); i++) {
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

		template <class T>
		inline ScanResult SwitchBaseNode<T>::ScanNodes(ScanInfoStack& info) const {
			ScanResult result;

			// Setup scan
			SwitchScanInfo switchInfo = ScanSetup(info.Get());

			// Get type of switch expression
			Symbols::TypeSymbol* const type = SwitchType();
			Boxx::Pointer<TypeNode> typeNode = nullptr;

			if (type) {
				typeNode = new TypeNode(type->AbsoluteName());
			}

			// Scan regular case nodes
			for (const Boxx::Pointer<Node>& node : nodes) {
				ScanPreContents(switchInfo, info.Get());

				ScanResult r = node->Scan(info);
				r.SelfUseCheck(info, node->file);
				result |= r;

				if (expr && typeNode) {
					ScanAssignment(typeNode, node, info, node->file);
				}

				ScanPostContents(switchInfo, info.Get());
			}

			// Scan default node
			if (def) {
				ScanPreContents(switchInfo, info.Get());

				ScanResult r = def->Scan(info);
				r.SelfUseCheck(info, def->file);
				result |= r;

				if (expr && typeNode) {
					ScanAssignment(typeNode, def, info, def->file);
				}

				ScanPostContents(switchInfo, info.Get());
			}

			// Cleanup scan
			ScanCleanup(switchInfo, info.Get());

			return result;
		}

		template <class T>
		inline ScanResult SwitchBaseNode<T>::Scan(ScanInfoStack& info) {
			ScanResult result = match->Scan(info);
			result.SelfUseCheck(info, match->file);

			// Scan match type
			Symbols::TypeSymbol* const matchType = match->Type();

			if (matchType) {
				ScanAssignment(new TypeNode(matchType->AbsoluteName()), new TypeNode(matchType->AbsoluteName()), info, match->file);
			}

			// Scan Nodes
			result |= ScanNodes(info);

			// Scan case values
			for (const Boxx::List<Boxx::Pointer<Node>>& nodeList : cases) {
				for (const Boxx::Pointer<Node>& node : nodeList) {
					ScanResult r = node->Scan(info);
					r.SelfUseCheck(info, node->file);
					result |= r;

					SymbolTable::FindOperator(Symbols::Name::Equal, matchType, node->Type(), node->file);
				}
			}

			return result;
		}

		template <class T>
		inline Symbols::NameList SwitchBaseNode<T>::FindSideEffectScope(const bool assign) {
			Symbols::NameList list = match->GetSideEffectScope(assign);

			for (Boxx::Pointer<Node>& node : nodes) {
				list = CombineSideEffects(list, node->GetSideEffectScope(assign));
			}

			for (Boxx::List<Boxx::Pointer<Node>>& caseList : cases) {
				for (Boxx::Pointer<Node>& node : caseList) {
					list = CombineSideEffects(list, node->GetSideEffectScope(assign));
				}
			}

			if (def) {
				list = CombineSideEffects(list, def->GetSideEffectScope(assign));
			}

			return list;
		}

		template <class T>
		inline Boxx::Pointer<T> SwitchBaseNode<T>::OptimizeSwitch(OptimizeInfo& info) {
			Node::Optimize(match, info);

			for (Boxx::Pointer<T>& node : nodes) {
				Node::Optimize(node, info);
			}

			for (Boxx::List<_Expression_>& caseList : cases) {
				for (_Expression_& node : caseList) {
					Node::Optimize(node, info);
				}
			}

			if (def) {
				Node::Optimize(def, info);
			}

			// TODO: Check for side effects in cases

			// Remove empty cases
			if (!expr) {
				for (Boxx::UInt i = 0; i < nodes.Size(); i++) {
					if (IsEmpty(nodes[i])) {
						cases.RemoveAt(i);
						nodes.RemoveAt(i);
						i--;
						info.optimized = true;
					}
				}

				if (def && IsEmpty(def)) {
					def = nullptr;
					info.optimized = true;
				}
			}

			return nullptr;
		}

		template <class T>
		inline Boxx::StringBuilder SwitchBaseNode<T>::ToMelon(const Boxx::UInt indent) const {
			Boxx::StringBuilder sb = "switch ";
			sb += match->ToMelon(indent);

			Boxx::String tabs1 = Boxx::String('\t').Repeat(indent + 1);
			Boxx::String tabs2 = Boxx::String('\t').Repeat(indent + 2);

			for (Boxx::UInt i = 0; i < nodes.Size(); i++) {
				sb += "\n";
				sb += tabs1;
				sb += "case ";

				for (Boxx::UInt u = 0; u < cases[i].Size(); u++) {
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
