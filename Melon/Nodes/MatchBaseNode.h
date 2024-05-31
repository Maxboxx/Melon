#pragma once

#include "Node.h"

#include "Boxx/Math.h"

#include "BreakStatement.h"
#include "TypeExpression.h"
#include "TypeConversion.h"
#include "IsExpression.h"
#include "KiwiVariable.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/ValueSymbol.h"
#include "Melon/Symbols/SymbolTable.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		template <class T>
		concept BaseMatchType = std::is_same<T, Expression>::value || std::is_same<T, Statement>::value;

		template <class T>
		concept BaseMatchType2 = std::is_same<T, Expression>::value || std::is_same<T, Statement>::value || std::is_same<T, Statements>::value;

		/// Abstract base node for {switch}.
		template <BaseMatchType T, BaseMatchType2 U = T>
		class MatchBaseNode : public T {
		public:
			/// The match expression.
			Ptr<Expression> match;

			/// The nodes for all the cases.
			Boxx::List<Ptr<U>> nodes;

			/// All case expressions.
			Boxx::List<Boxx::List<Ptr<Expression>>> cases;

			/// The default node.
			Ptr<U> def;

			MatchBaseNode(Symbols::Symbol* const scope, const FileInfo& file);
			~MatchBaseNode();

			/// Scan info for match nodes.
			struct MatchScanInfo {
				/// {true} if the {match} is in a constructor.
				bool init = false;

				/// Scope info.
				ScopeInfo scope;

				/// Scope info for all cases.
				Boxx::List<ScopeInfo> cases;

				/// {true} if at least one case is guaranteed to run.
				bool willACaseRun = false;
			};

			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			Symbols::TypeSymbol* SwitchType() const;
			Ptr<T> OptimizeSwitch(OptimizeInfo& info);

			virtual Symbols::NameList FindSideEffectScope(const bool assign);

			MatchScanInfo ScanSetup(ScanInfo& info) const;

			void ScanPreContents(MatchScanInfo& switchInfo, ScanInfo& info) const;

			void ScanPostContents(MatchScanInfo& switchInfo, ScanInfo& info) const;

			void ScanCleanup(MatchScanInfo& switchInfo, ScanInfo& info) const;

			ScanResult ScanNodes(ScanInfoStack& info) const;
		};

		template <BaseMatchType T, BaseMatchType2 U>
		inline MatchBaseNode<T, U>::MatchBaseNode(Symbols::Symbol* const scope, const FileInfo& file) : T(scope, file) {

		}

		template <BaseMatchType T, BaseMatchType2 U>
		inline MatchBaseNode<T, U>::~MatchBaseNode() {

		}

		template <BaseMatchType T, BaseMatchType2 U>
		inline Symbols::TypeSymbol* MatchBaseNode<T, U>::SwitchType() const {
			return nullptr;
		}

		template <>
		inline Symbols::TypeSymbol* MatchBaseNode<Expression, Expression>::SwitchType() const {
			Symbols::TypeSymbol* type = nodes[0]->Type();

			for (Weak<Expression> node : nodes) {
				if (type != node->Type()) {
					ErrorLog::Error(LogMessage("error.type.switch"), this->File());
				}
			}

			return type;
		}

		template <BaseMatchType T, BaseMatchType2 U>
		inline Ptr<Kiwi::Value> MatchBaseNode<T, U>::Compile(CompileInfo& info) {
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
					Ptr<Kiwi::Value> comp;

					if (Weak<TypeConversion> conv = expr.As<TypeConversion>()) {
						Ptr<IsExpression> isExpr = new IsExpression(expr->scope, expr->File());
						isExpr->expression = new KiwiVariable(match->Copy(), this->match->Type()->AbsoluteName());

						Symbols::ValueSymbol* const valueSym = conv->GetValueSymbol(true);
						
						if (valueSym) {
							isExpr->type = valueSym->AbsoluteName();
						}
						else {
							isExpr->type = conv->Type()->AbsoluteName();
						}

						comp = isExpr->Compile(info);
					}
					else if (Symbols::ValueSymbol* const valSym = expr->Symbol<Symbols::ValueSymbol>()) {
						Ptr<IsExpression> isExpr = new IsExpression(expr->scope, expr->File());
						isExpr->expression = new KiwiVariable(match->Copy(), this->match->Type()->AbsoluteName());
						isExpr->type = valSym->AbsoluteName();
						comp = isExpr->Compile(info);
					}
					else {
						Symbols::FunctionSymbol* const sym = Symbols::SymbolTable::FindOperator(Symbols::Name::Equal, this->match->Type(), expr->Type(), expr->File()); 
						comp = sym->symbolNode->Compile(kiwiVar, expr, info, false);
					}

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

				if (Weak<TypeConversion> conv = cases[i][0].As<TypeConversion>()) {
					Ptr<Kiwi::SubVariable> items = new Kiwi::SubVariable(match->Copy(), Symbols::Name::Items.name);
					Ptr<KiwiVariable> kiwi = new KiwiVariable(items, conv->expression->Type()->AbsoluteName());
					Node::CompileAssignment(conv->expression, kiwi, info, conv->File(), true);
				}

				LoopInfo scope = LoopInfo(i < nodes.Count() - 1 ? labels[i + 1] : (def ? defaultLbl : endLbl), endLbl);

				if constexpr (!std::is_same<U, Expression>::value) {
					info.PushScope(scope);
				}

				Ptr<Kiwi::Value> value = nodes[i]->Compile(info);

				if constexpr (std::is_same<U, Expression>::value) {
					info.AddInstruction(new Kiwi::AssignInstruction(result->Copy(), value));
				}
				else {
					info.PopScope();
				}

				info.AddInstruction(new Kiwi::GotoInstruction(endLbl));
			}

			if (def) {
				info.NewInstructionBlock(defaultLbl);

				LoopInfo scope = LoopInfo(endLbl);

				if constexpr (!std::is_same<U, Expression>::value) {
					info.PushScope(scope);
				}

				Ptr<Kiwi::Value> value = def->Compile(info);

				if constexpr (std::is_same<U, Expression>::value) {
					info.AddInstruction(new Kiwi::AssignInstruction(result->Copy(), value));
				}
				else {
					info.PopScope();
				}
			}

			info.NewInstructionBlock(endLbl);

			return result;
		}

		template <BaseMatchType T, BaseMatchType2 U>
		inline void MatchBaseNode<T, U>::IncludeScan(Parsing::ParsingInfo& info) {
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

		template <BaseMatchType T, BaseMatchType2 U>
		inline MatchBaseNode<T, U>::MatchScanInfo MatchBaseNode<T, U>::ScanSetup(ScanInfo& info) const {
			MatchScanInfo switchInfo;
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

		template <BaseMatchType T, BaseMatchType2 U>
		inline void MatchBaseNode<T, U>::ScanPreContents(MatchScanInfo& switchInfo, ScanInfo& info) const {
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

		template <BaseMatchType T, BaseMatchType2 U>
		inline void MatchBaseNode<T, U>::ScanPostContents(MatchScanInfo& switchInfo, ScanInfo& info) const {
			if constexpr (!std::is_same<U, Expression>::value) {
				if (switchInfo.init) {
					info.scopeInfo.unassigned = info.type->UnassignedMembers();
				}

				switchInfo.cases.Add(info.scopeInfo);
			}
		}

		template <BaseMatchType T, BaseMatchType2 U>
		inline void MatchBaseNode<T, U>::ScanCleanup(MatchScanInfo& switchInfo, ScanInfo& info) const {
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

		template <BaseMatchType T, BaseMatchType2 U>
		inline ScanResult MatchBaseNode<T, U>::ScanNodes(ScanInfoStack& info) const {
			ScanResult result;

			// Setup scan
			MatchScanInfo switchInfo = ScanSetup(info.Get());

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

		template <BaseMatchType T, BaseMatchType2 U>
		inline ScanResult MatchBaseNode<T, U>::Scan(ScanInfoStack& info) {
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
					if (Weak<TypeConversion> conv = node.As<TypeConversion>()) {
						continue;
					}

					ScanResult r = node->Scan(info);
					r.SelfUseCheck(info, node->File());
					result |= r;

					Symbols::SymbolTable::FindOperator(Symbols::Name::Equal, matchType, node->Type(), node->File());
				}
			}

			return result;
		}

		template <BaseMatchType T, BaseMatchType2 U>
		inline Symbols::NameList MatchBaseNode<T, U>::FindSideEffectScope(const bool assign) {
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

		template <BaseMatchType T, BaseMatchType2 U>
		inline Ptr<T> MatchBaseNode<T, U>::OptimizeSwitch(OptimizeInfo& info) {
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

		template <BaseMatchType T, BaseMatchType2 U>
		inline Boxx::StringBuilder MatchBaseNode<T, U>::ToMelon(const Boxx::UInt indent) const {
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
