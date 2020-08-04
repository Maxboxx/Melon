#include "Boxx/List.h"
#include "Boxx/String.h"

#if BOXX_VERSION_LT(2, 3, 0)
	#error Boxx version of 2.3.0 or newer required
#endif

#include "Kiwi/Kiwi.h"
#include "Kiwi/x86_64Converter.h"

#include "Melon/Errors.h"
#include "Melon/MelonCompiler.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;

int main() {
	CompilerOptions options = CompilerOptions::LoadFromFile("tests/options.mango");
	options.converter = new x86_64Converter(x86_64Syntax::Intel, ErrorLog::logger);

	MelonCompiler::Compile(options);

	return 0;
}
