#pragma once

#include "String.h"

#include <stdlib.h>

///N System

namespace Boxx {
	///B System
	/// Static class for communicating with the operating system
	class System {
	public:
		///T Execute
		/// Executes a system command
		static void Execute(const String& command);
	};

	inline void System::Execute(const String& command) {
		system(command);
	}
}