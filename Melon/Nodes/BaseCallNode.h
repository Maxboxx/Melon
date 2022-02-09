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

		/// Base node for call nodes.
		template <class T>
		class BaseCallNode : public T {
		public:
			/// The expression to call.
			Ptr<Expression> expression;

			/// The arguments for the call.
			Boxx::List<Ptr<Expression>> arguments;

			/// The argument attributes for the call.
			Boxx::List<CallArgAttributes> attributes;

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

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<T> Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			bool isStatement;

			Boxx::List<Symbols::TypeSymbol*> GetReturnTypes() const;

			Symbols::Name GetFuncName() const;

			Boxx::Optional<Boxx::List<Symbols::TypeSymbol*>> GetTemplateTypes(const Boxx::Optional<Boxx::List<Symbols::NameList>>& types) const;
			Boxx::Optional<Boxx::List<Symbols::TypeSymbol*>> GetArgumentTypes() const;

			Symbols::FunctionSymbol* GetFunctionSymbol(const Boxx::Optional<Boxx::List<Symbols::TypeSymbol*>>& templateTypes, const Boxx::List<Symbols::TypeSymbol*>& argTypes) const;
			Symbols::FunctionSymbol* GetInitFunction(const Boxx::Optional<Boxx::List<Symbols::TypeSymbol*>>& templateTypes, const Boxx::List<Symbols::TypeSymbol*>& argTypes) const;
			Symbols::FunctionSymbol* GetStaticOrPlainFunction(const Boxx::Optional<Boxx::List<Symbols::TypeSymbol*>>& templateTypes, const Boxx::List<Symbols::TypeSymbol*>& argTypes) const;
			Symbols::FunctionSymbol* GetMethod(const Boxx::Optional<Boxx::List<Symbols::TypeSymbol*>>& templateTypes, const Boxx::List<Symbols::TypeSymbol*>& argTypes) const;

			struct CallInfo {
				Symbols::FunctionSymbol* func;
				bool isInit;

				Boxx::Int retSize, argSize;
				Boxx::UInt tempSize, pushSize;

				Boxx::List<Boxx::UInt> memoryOffsets;
				Boxx::List<bool> assignFirst;

				Boxx::UInt frame, initialTop, stackIndex;

				StackPtr initialStack;

				CompiledNode cn;
			};

			Boxx::UInt CalculateReturnSize(CallInfo& callInfo);
			Boxx::UInt CalculateArgumentSize(CallInfo& callInfo, CompileInfo& info);
			Boxx::UInt CalculateTemporarySize(CallInfo& callInfo, CompileInfo info);
			Boxx::UInt CalculatePushSize(CallInfo& callInfo, CompileInfo& info);

			void SetupStackFrame(CallInfo& callInfo, CompileInfo& info);
			void CompileArguments(CallInfo& callInfo, CompileInfo& info);
			void CompileRefArgument(CallInfo& callInfo, CompileInfo& info, Symbols::TypeSymbol* const type, Boxx::Int index);
			Ptr<Expression> GetRefArgument(CallInfo& callInfo, CompileInfo& info, Symbols::TypeSymbol* const type, Boxx::Int index);
			void CompileCopyArgument(CallInfo& callInfo, CompileInfo& info, Symbols::TypeSymbol* const type, Boxx::Int index);
			void CompileCall(CallInfo& callInfo, CompileInfo& info);
		};
	}
}
