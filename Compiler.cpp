#include "Boxx/List.h"
#include "Boxx/String.h"

#if BOXX_VERSION_LT(2, 1, 1)
	#error Boxx version of 2.1.1 or newer required
#endif

#include "Kiwi/Kiwi.h"
#include "Kiwi/x86_64Converter.h"

#include "Melon/Errors.h"
#include "Melon/MelonCompiler.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;

int main() {
	CompilerOptions options;
	options.outputAST = true;
	options.outputSymbols = true;
	options.outputKiwi = true;
	options.outputAssembly = true;
	options.outputExecutable = false;
	options.converter = new x86_64Converter(x86_64Syntax::Intel, ErrorLog::logger);

	MelonCompiler::Compile("tests/test.melon", options);

	return 0;
}
