#pragma once

#include <chrono>
#include "Types.h"

///N Time
namespace Boxx {
	///B Time
	/// A static class used to keep track of time
	class Time {
	public:
		///T Update Delta
		/// Updates the delta time. Call this once every frame.
		static void Update();

		///T Delta time
		/// Get the current delta time
		static float Delta();

		///T Maximum delta time
		/// Set the maximum value of the delta time. Defaults to <code>1.0</code>
		static void SetMaxDelta(const float f);

		///T Total time
		/// Gets the total running time of the program in seconds since <code>Start()</code> was called
		static float Total();

		///T Start time
		/// Sets the start time for total time
		static void Start();

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
