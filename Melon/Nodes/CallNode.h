#pragma once

#include "Node.h"

#include "Melon/Symbols/FunctionSymbol.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for calling a value.
		class CallNode : public Node {
		public:
			/// Argument attributes for the call.
			enum class ArgAttributes : Boxx::UByte {
				/// No attribute.
				None,

				/// {ref} attribute.
				Ref,

				/// {noref} attribute.
				NoRef
			};

			/// The node to call.
			NodePtr node;

			/// The arguments for the call.
			Boxx::List<NodePtr> args;

			/// The argument attributes for the call.
			Boxx::List<ArgAttributes> attributes;

			/// {true} if the call is a method call.
			bool isMethod = false;

			/// The method name.
			///p Only used if {isMethod} is {true}.
			Symbols::Name methodName;

			/// {true} if the call is a call operator.
			bool op = false;

			/// {true} if the call is a call statement.
			bool isStatement = false;

			CallNode(Symbols::Symbol* const scope, const FileInfo& file);
			~CallNode();

			/// Finds the function to call.
			Symbols::FunctionSymbol* GetFunc() const;

			/// Checks if the function should pass self as an argument.
			bool IsSelfPassing() const;

			/// Checks if the function is a constructor.
			bool IsInit() const;

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Boxx::List<Symbols::TypeSymbol*> Types() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			Symbols::Name GetFuncName() const;

			Boxx::Optional<Boxx::List<Symbols::TypeSymbol*>> GetTemplateTypes(const Boxx::Optional<Boxx::List<Symbols::NameList>>& types) const;
			Boxx::Optional<Boxx::List<Symbols::TypeSymbol*>> GetArgumentTypes() const;

			Symbols::FunctionSymbol* GetFunctionSymbol(const Boxx::Optional<Boxx::List<Symbols::TypeSymbol*>>& templateTypes, const Boxx::List<Symbols::TypeSymbol*>& argTypes) const;
			Symbols::FunctionSymbol* GetOperatorFunction(const Boxx::Optional<Boxx::List<Symbols::TypeSymbol*>>& templateTypes, const Boxx::List<Symbols::TypeSymbol*>& argTypes) const;
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
			NodePtr GetRefArgument(CallInfo& callInfo, CompileInfo& info, Symbols::TypeSymbol* const type, Boxx::Int index);
			void CompileCopyArgument(CallInfo& callInfo, CompileInfo& info, Symbols::TypeSymbol* const type, Boxx::Int index);
			void CompileCall(CallInfo& callInfo, CompileInfo& info);
		};
	}
}