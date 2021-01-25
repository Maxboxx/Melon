#include "Boxx/List.h"
#include "Boxx/String.h"

#if BOXX_VERSION_LT(3, 2, 1)
	#error Boxx version of 3.2.1 or newer required
#endif

#include "Kiwi/Kiwi.h"
#include "Kiwi/x86_64Converter.h"

#include "Melon/Errors.h"
#include "Melon/MelonCompiler.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;

int main() {
	try {
		CompilerOptions options = CompilerOptions::LoadFromFile("code/options.mango");
		options.converter = new x86_64Converter(x86_64Syntax::Intel, ErrorLog::logger);

		MelonCompiler::Compile(options);
	}
	catch (FatalLoggerError e) {
		ErrorLog::LogErrors();
	}
	catch (Error& e) {
		try {
			ErrorLog::Fatal(PlainError(e.Name() + ": " + e.Message()));
		}
		catch (FatalLoggerError e) {
			ErrorLog::LogErrors();
		}
	}

	return 0;
}
