#include "assertion.h"

#include <iostream>


namespace tfr_internal {

void assertion_impl(bool condition, const char* message, const char* conditionStr, const char* file, int line) {
	if (!condition) {
		std::cout << file << " (" << line << "): " << conditionStr << " " << message << "\n";
	}
}

void assertion_2_impl(bool condition, const char* message1, const char* message2, const char* conditionStr, const char* file, int line) {
	if (!condition) {
		std::cout << file << " (" << line << "): " << conditionStr << " " << message1 << " " << message2 << "\n";
	}
}

}
