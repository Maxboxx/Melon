#pragma once

#include "Boxx/String.h"
#include "Boxx/List.h"
#include "Boxx/Map.h"
#include "Boxx/Mango.h"

///N Melon
namespace Melon {
	/// A message for the error log.
	struct LogMessage {
	private:
		typedef Boxx::List<Boxx::String> StrList;
		typedef Boxx::List<Boxx::Long> IntList;
		typedef Boxx::List<Boxx::List<Boxx::String>> ListList;

	public:

		/// The message.
		Boxx::String message;

		/// A map of all messages.
		static Boxx::Map<Boxx::String, Boxx::String> messages;

		/// Adds quotes around a string.
		static Boxx::String Quote(const Boxx::String& str) {
			return '\'' + str + '\'';
		}

		/// Loads messages from a mango file.
		static void LoadMessages(const Boxx::String& filename);

		/// Creates a log message with the specified message.
		static LogMessage Message(const Boxx::String& message) {
			LogMessage msg;
			msg.message = message;
			return msg;
		}

		/// Creates an empty message
		LogMessage() {
			message = "";
		}

		LogMessage(const LogMessage& msg) {
			message = msg.message;
		}

		/// Creates a message from a message id.
		explicit LogMessage(const Boxx::String& id) {
			CreateMessage(id, StrList(), IntList(), ListList());
		}

		///H String Constructors

		/// Creates a message from a string.
		LogMessage(const Boxx::String& id, const Boxx::String& str1) {
			StrList str;
			str.Add(str1);

			CreateMessage(id, str, IntList(), ListList());
		}

		/// Creates a message from two strings.
		LogMessage(const Boxx::String& id, const Boxx::String& str1, const Boxx::String& str2) {
			StrList str;
			str.Add(str1);
			str.Add(str2);

			CreateMessage(id, str, IntList(), ListList());
		}

		/// Creates a message from three strings.
		LogMessage(const Boxx::String& id, const Boxx::String& str1, const Boxx::String& str2, const Boxx::String& str3) {
			StrList str;
			str.Add(str1);
			str.Add(str2);
			str.Add(str3);

			CreateMessage(id, str, IntList(), ListList());
		}

		///H Integer Constructors

		/// Creates a message from an integer.
		LogMessage(const Boxx::String& id, const Boxx::Long& int1) {
			IntList int_;
			int_.Add(int1);

			CreateMessage(id, StrList(), int_, ListList());
		}

		/// Creates a message from two integers.
		LogMessage(const Boxx::String& id, const Boxx::Long& int1, const Boxx::Long& int2) {
			IntList int_;
			int_.Add(int1);
			int_.Add(int2);

			CreateMessage(id, StrList(), int_, ListList());
		}

		/// Creates a message from three integers.
		LogMessage(const Boxx::String& id, const Boxx::Long& int1, const Boxx::Long& int2, const Boxx::Long& int3) {
			IntList int_;
			int_.Add(int1);
			int_.Add(int2);
			int_.Add(int3);

			CreateMessage(id, StrList(), int_, ListList());
		}

		///H List Constructors

		/// Creates a message from a list.
		LogMessage(const Boxx::String& id, const Boxx::List<Boxx::String>& list1) {
			ListList list;
			list.Add(list1);

			CreateMessage(id, StrList(), IntList(), list);
		}

		/// Creates a message from two lists.
		LogMessage(const Boxx::String& id, const Boxx::List<Boxx::String>& list1, const Boxx::List<Boxx::String>& list2) {
			ListList list;
			list.Add(list1);
			list.Add(list2);

			CreateMessage(id, StrList(), IntList(), list);
		}

		/// Creates a message from three lists.
		LogMessage(const Boxx::String& id, const Boxx::List<Boxx::String>& list1, const Boxx::List<Boxx::String>& list2, const Boxx::List<Boxx::String>& list3) {
			ListList list;
			list.Add(list1);
			list.Add(list2);
			list.Add(list3);

			CreateMessage(id, StrList(), IntList(), list);
		}

		///H String and Integer Constructors

		/// Creates a message from a string and an integer.
		LogMessage(const Boxx::String& id, const Boxx::String& str1, const Boxx::Long int1) {
			StrList str;
			str.Add(str1);

			IntList int_;
			int_.Add(int1);

			CreateMessage(id, str, int_, ListList());
		}

		/// Creates a message from two strings and an integer.
		LogMessage(const Boxx::String& id, const Boxx::String& str1, const Boxx::String& str2, const Boxx::Long int1) {
			StrList str;
			str.Add(str1);
			str.Add(str2);

			IntList int_;
			int_.Add(int1);

			CreateMessage(id, str, int_, ListList());
		}

		/// Creates a message from a string and two integers.
		LogMessage(const Boxx::String& id, const Boxx::String& str1, const Boxx::Long int1, const Boxx::Long int2) {
			StrList str;
			str.Add(str1);

			IntList int_;
			int_.Add(int1);
			int_.Add(int2);

			CreateMessage(id, str, int_, ListList());
		}

		///H String and List Constructors

		/// Creates a message from a string and a list.
		LogMessage(const Boxx::String& id, const Boxx::String& str1, const Boxx::List<Boxx::String>& list1) {
			StrList str;
			str.Add(str1);

			ListList list;
			list.Add(list1);

			CreateMessage(id, str, IntList(), list);
		}

		/// Creates a message from two strings and a list.
		LogMessage(const Boxx::String& id, const Boxx::String& str1, const Boxx::String& str2, const Boxx::List<Boxx::String>& list1) {
			StrList str;
			str.Add(str1);
			str.Add(str2);

			ListList list;
			list.Add(list1);

			CreateMessage(id, str, IntList(), list);
		}

		/// Creates a message from a string and two lists.
		LogMessage(const Boxx::String& id, const Boxx::String& str1, const Boxx::List<Boxx::String>& list1, const Boxx::List<Boxx::String>& list2) {
			StrList str;
			str.Add(str1);

			ListList list;
			list.Add(list1);
			list.Add(list2);

			CreateMessage(id, str, IntList(), list);
		}

		///H Integer and List Constructors

		/// Creates a message from an integer and a list.
		LogMessage(const Boxx::String& id, const Boxx::Long int1, const Boxx::List<Boxx::String>& list1) {
			IntList int_;
			int_.Add(int1);

			ListList list;
			list.Add(list1);

			CreateMessage(id, StrList(), int_, list);
		}

		/// Creates a message from two integers and a list.
		LogMessage(const Boxx::String& id, const Boxx::Long int1, const Boxx::Long int2, const Boxx::List<Boxx::String>& list1) {
			IntList int_;
			int_.Add(int1);
			int_.Add(int2);

			ListList list;
			list.Add(list1);

			CreateMessage(id, StrList(), int_, list);
		}

		/// Creates a message from an integer and two lists.
		LogMessage(const Boxx::String& id, const Boxx::Long int1, const Boxx::List<Boxx::String>& list1, const Boxx::List<Boxx::String>& list2) {
			IntList int_;
			int_.Add(int1);

			ListList list;
			list.Add(list1);
			list.Add(list2);

			CreateMessage(id, StrList(), int_, list);
		}

		///H Combined Constructors

		/// Creates a message from a string, an integer, and a list.
		LogMessage(const Boxx::String& id, const Boxx::String& str1, const Boxx::Long int1, const Boxx::List<Boxx::String>& list1) {
			StrList str;
			str.Add(str1);
			
			IntList int_;
			int_.Add(int1);

			ListList list;
			list.Add(list1);

			CreateMessage(id, str, int_, list);
		}

		///H Constructors with Lists

		/// Creates a message from an integer list.
		LogMessage(const Boxx::String& id, const Boxx::List<Boxx::Long>& intList) {
			CreateMessage(id, StrList(), intList, ListList());
		}

		/// Creates a message from a list of lists.
		LogMessage(const Boxx::String& id, const Boxx::List<Boxx::List<Boxx::String>>& listList) {
			CreateMessage(id, StrList(), IntList(), listList);
		}

		/// Creates a message from a string list and an integer list.
		LogMessage(const Boxx::String& id, const Boxx::List<Boxx::String>& strList, const Boxx::List<Boxx::Long>& intList) {
			CreateMessage(id, strList, intList, ListList());
		}

		/// Creates a message from a string list and a list of lists.
		LogMessage(const Boxx::String& id, const Boxx::List<Boxx::String>& strList, const Boxx::List<Boxx::List<Boxx::String>>& listList)  {
			CreateMessage(id, strList, IntList(), listList);
		}

		/// Creates a message from an integer list and a list of lists.
		LogMessage(const Boxx::String& id, const Boxx::List<Boxx::Long>& intList, const Boxx::List<Boxx::List<Boxx::String>>& listList) {
			CreateMessage(id, StrList(), intList, listList);
		}

		/// Creates a message from a string list, an integer list, and a list of lists.
		LogMessage(const Boxx::String& id, const Boxx::List<Boxx::String>& strList, const Boxx::List<Boxx::Long>& intList, const Boxx::List<Boxx::List<Boxx::String>>& listList)  {
			CreateMessage(id, strList, intList, listList);
		}

		~LogMessage() {}

	private:
		static void LoadMangoMap(const Boxx::Mango& map, const Boxx::String& key);
		void CreateMessage(const Boxx::String& id, const Boxx::List<Boxx::String>& strList, const Boxx::List<Boxx::Long>& intList, const Boxx::List<Boxx::List<Boxx::String>>& listList);
	};
}
