#include "LogMessage.h"

#include "Boxx/File.h"
#include "Boxx/StringBuilder.h"

using namespace Boxx;

using namespace Melon;

Map<String, String> LogMessage::messages;

void LogMessage::LoadMessages(const String& filename) {
	FileReader file = FileReader(filename);
	Mango mango = Mango::Decode(file.ReadAll());
	file.Close();

	LoadMangoMap(mango, "");
}

void LogMessage::LoadMangoMap(const Mango& map, const String& key) {
	for (const Pair<String, Mango>& mango : (MangoMap)map) {
		if (mango.value.Type() == MangoType::String) {
			messages.Add(key + mango.key, (String)mango.value);
		}
		else if (mango.value.Type() == MangoType::Map) {
			LoadMangoMap(mango.value, key + mango.key + '.');
		}
	}
}

void LogMessage::CreateMessage(const String& id, const List<String>& strList, const List<Long>& intList, const List<List<String>>& listList) {
	String format;

	if (!messages.Contains(id, format)) {
		message = id;
		return;
	}

	UInt strIndex = 0, intIndex = 0, listIndex = 0;
	bool escape = false;

	StringBuilder msg;

	for (const char c : format) {
		if (escape) {
			switch (c) {
				case 's': {
					if (strIndex < strList.Count()) {
						msg += strList[strIndex];
						strIndex++;
					}

					break;
				}

				case 'i': {
					if (intIndex < intList.Count()) {
						msg += String::ToString(intList[intIndex]);
						intIndex++;
					}

					break;
				}

				case 'l': {
					if (listIndex < listList.Count()) {
						List<String> list = listList[listIndex];

						for (UInt i = 0; i < list.Count(); i++) {
							if (i > 0) msg += ", ";
							msg += list[i];
						}

						intIndex++;
					}

					break;
				}

				default: {
					msg += c;
					break;
				}
			}

			escape = false;
		}
		else if (c == '%') {
			escape = true;
		}
		else {
			msg += c;
		}
	}

	message = (String)msg;
}