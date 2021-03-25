#pragma once

#include <chrono>
#include "Types.h"
#include "String.h"

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	///[Heading] Static Classes

	///[Title] Time
	/// A static class used to keep track of time.
	///[Block] Time
	class Time final {
	public:
		Time() = delete;

		/// Starts the time.
		static void Start();

		/// Updates the delta time. Call this once every frame.
		static void Update();

		/// Get the current delta time.
		static float Delta();

		/// Set the maximum value of the delta time. Defaults to {1.0}.
		static void SetMaxDelta(const float f);

		/// Gets the total running time elapsed in seconds since {Start()} was called.
		static float Total();

	private:
		typedef std::chrono::high_resolution_clock::time_point time;
		typedef std::chrono::high_resolution_clock clock;
		typedef std::chrono::microseconds micro;
		typedef std::chrono::duration<float> seconds;

		static float& dt() {static float dt = 0.f; return dt;}
		static float& maxdt() {static float maxdt = 1.f; return maxdt;}
		static time& deltaTime() {static time dt; return dt;}
		static time& totalTime() {static time tt; return tt;}
	};

	inline void Time::Update() {
		const auto c = clock::now();
		dt() = seconds(c - deltaTime()).count();
		if (dt() > maxdt()) dt() = maxdt();
		deltaTime() = c;
	}

	inline float Time::Delta() {
		return dt();
	}

	inline void Time::SetMaxDelta(const float f) {
		maxdt() = f;
	}

	inline float Time::Total() {
		const auto c = clock::now();
		return seconds(c - totalTime()).count();
	}

	inline void Time::Start() {
		totalTime() = clock::now();
	}
}
