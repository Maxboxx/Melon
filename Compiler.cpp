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

// TODO: Cleanup code

// TODO: Pointerify Symbol
// TODO: Remove Set<ScanType>
// TODO: Kiwi number constant smallest size
// TODO: Kiwi number comparison update sign
// TODO: Kiwi data/code reposition
// TODO: Combined assignment operators
// TODO: Combined comarison operator
// TODO: Add specific error for single assignment
// TODO: Static variables
// TODO: Improve error messages
// TODO: Error for unwrap operator
// TODO: FindSideEffectScope for assignments

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
