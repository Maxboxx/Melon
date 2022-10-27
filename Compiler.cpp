#include "Boxx/List.h"
#include "Boxx/String.h"
#include "Boxx/Dragonfruit.h"
#include "Boxx/Mango.h"

#include "Ptr.h"

#include "Kiwi/Old/Kiwi.h"
#include "Kiwi/Old/x86_64Converter.h"

#include "Melon/Errors.h"
#include "Melon/MelonCompiler.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;

int main() {
	/*
	DragonfruitInfo info;
	info.docComment = "///";
	info.namespaceSeparator = "::";
	info.blockMode = DragonfruitBlockMode::Indent;
	info.extensions.Add("h");

	info.blockPatterns.Add(Regex("%b{enum%n+class|class|struct|enum}%n+(%w+)%n*{%:%n*{public|protected|private}?%n+[%w%:]+}?%n*[;,{]%n*$"));
	info.titlePatterns.Add(Regex("%b{enum%n+class|class|struct|enum}%n+(%w+)%n*{%:%n*{public|protected|private}?%n+[%w%:]+}?%n*[;,{]%n*$"));
	info.codePatterns.Add(Regex("(./)[;,{]%n*$"));

	info.blockHeadingPatterns.Add(Pair<String, Regex>("Constructors", Regex("^%n*{explicit%n+}?%w+%n*%(")));
	info.blockHeadingPatterns.Add(Pair<String, Regex>("Operators", Regex("%boperator%b")));
	info.blockHeadingPatterns.Add(Pair<String, Regex>("Static Functions", Regex("%bstatic%n+./%w+%n*%(")));
	info.blockHeadingPatterns.Add(Pair<String, Regex>("Static Members", Regex("%bstatic%b")));
	info.blockHeadingPatterns.Add(Pair<String, Regex>("Methods", Regex("^~[=]/%w+%n*%(")));
	info.blockHeadingPatterns.Add(Pair<String, Regex>("Types", Regex("%b{enum%n+class|class|struct|enum}%n+(%w+)")));
	info.blockHeadingPatterns.Add(Pair<String, Regex>("Values", Regex("^%n*%w+%n*{[,=]|$}")));
	info.blockHeadingPatterns.Add(Pair<String, Regex>("Members", Regex("%w+")));
	
	Dragonfruit doc = Dragonfruit::Generate("./", info);
	doc.GenerateHTML("../Documentation");
	/*/
	try {
		CompilerOptions options = CompilerOptions::LoadFromFile("code/options.mango");
		options.converter = new x86_64Converter(x86_64Syntax::Intel, ErrorLog::logger);

		MelonCompiler::Compile(options);
	}
	catch (FatalLoggerError& e) {
		ErrorLog::LogErrors();
	}
	catch (Error& e) {
		try {
			ErrorLog::Fatal(LogMessage::Message(e.Name() + ": " + e.Message()), FileInfo());
		}
		catch (FatalLoggerError e) {
			ErrorLog::LogErrors();
		}
	}
	//*/

	System::Execute("pause");
	return 0;
}
