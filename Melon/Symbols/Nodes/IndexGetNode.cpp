#include "IndexGetNode.h"

#include "Melon/Symbols/TemplateTypeSymbol.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> IndexGetNode::Compile(List<Weak<Expression>> operands, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Variable> arr = operands[0]->Compile(info).AsPtr<Kiwi::Variable>();

	Kiwi::Type type = operands[0]->Type()->Cast<TemplateTypeSymbol>()->TemplateArgument(0)->KiwiType();

	Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());

	info.AddInstruction(new Kiwi::AssignInstruction(
		type, var->Copy(), 
		new Kiwi::OffsetExpression(
			new Kiwi::SubVariable(new Kiwi::DerefVariable(arr->name), Name::Items.name),
			type, type, operands[1]->Compile(info))
		)
	);

	return var;
}
