#pragma once

#include <chrono>
#include <functional>
#include <utility>
#include <iostream>

namespace noise {

class Timer {
public:
	Timer() : _start(std::chrono::steady_clock::now()) {
	}

	long long millieSeconds() const {
		return _passed<std::chrono::milliseconds>(std::chrono::steady_clock::now());
	}

	long long nanoSeconds() const {
		return _passed<std::chrono::nanoseconds>(std::chrono::steady_clock::now());
	}

	long long seconds() const {
		return _passed<std::chrono::seconds>(std::chrono::steady_clock::now());
	}

private:
	template <typename T>
	long long _passed(std::chrono::steady_clock::time_point stop) const {
		using namespace std::chrono;
		return duration_cast<T>(stop - _start).count();
	}

	std::chrono::steady_clock::time_point _start;
};

}
