#pragma once

#include "Expression.h"

#include "Melon/Symbols/FunctionSymbol.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Argument attributes for function calls.
		enum class CallArgAttributes : Boxx::UByte {
			/// No attribute.
			None,

			/// {ref} attribute.
			Ref,

			/// {noref} attribute.
			NoRef
		};

		BOXX_ENUM_FLAGS(CallArgAttributes);

		template <class T>
		concept BaseCallType = std::is_same<T, Expression>::value || std::is_same<T, Statement>::value;

		/// Base node for call nodes.
		template <BaseCallType T>
		class BaseCallNode : public T {
		public:
			/// The operator function to call.
			Symbols::FunctionSymbol* operatorFunction = nullptr;

			/// The expression to call.
			Ptr<Expression> expression;

			/// The arguments for the call.
			Boxx::List<Ptr<Expression>> arguments;

			/// The argument attributes for the call.
			Boxx::List<CallArgAttributes> modifiers;

			BaseCallNode(Symbols::Symbol* const scope, const FileInfo& file);
			~BaseCallNode();

			/// {true} if the call is a method call.
			bool IsMethod() const;

			/// Gets the function to call.
			Symbols::FunctionSymbol* GetFunc() const;

			/// {true} if {self} should be passed as an argument.
			bool IsSelfPassing() const;

			/// {true} if the function is a constructor.
			bool IsInit() const;

			/// Compiles a call expression.
			Ptr<Kiwi::CallExpression> CompileCallExpression(CompileInfo& info);

			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<T> Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			bool isStatement;

			struct CompileResult {
				Ptr<Kiwi::CallExpression> call;
				Ptr<Kiwi::Value> instance;
				Symbols::FunctionSymbol* func;
			};

			CompileResult CompileWithResult(CompileInfo& info);

			Boxx::List<Symbols::TypeSymbol*> GetReturnTypes() const;

			Symbols::Name GetFuncName() const;

			Boxx::Optional<Boxx::List<Symbols::TypeSymbol*>> GetTemplateTypes(const Boxx::Optional<Boxx::List<Symbols::NameList>>& types) const;
			Boxx::Optional<Boxx::List<Symbols::TypeSymbol*>> GetArgumentTypes() const;

			Symbols::FunctionSymbol* GetFunctionSymbol(const Boxx::Optional<Boxx::List<Symbols::TypeSymbol*>>& templateTypes, const Boxx::List<Symbols::TypeSymbol*>& argTypes) const;
			Symbols::FunctionSymbol* GetInitFunction(const Boxx::Optional<Boxx::List<Symbols::TypeSymbol*>>& templateTypes, const Boxx::List<Symbols::TypeSymbol*>& argTypes) const;
			Symbols::FunctionSymbol* GetStaticOrPlainFunction(const Boxx::Optional<Boxx::List<Symbols::TypeSymbol*>>& templateTypes, const Boxx::List<Symbols::TypeSymbol*>& argTypes) const;
			Symbols::FunctionSymbol* GetMethod(const Boxx::Optional<Boxx::List<Symbols::TypeSymbol*>>& templateTypes, const Boxx::List<Symbols::TypeSymbol*>& argTypes) const;
		};
	}
}
