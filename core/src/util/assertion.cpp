#include "assertion.h"

#include <iostream>


namespace mixer_internal {

void __assertion(bool condition, const char* message, const char* conditionStr, const char* file, int line) {
	if (!condition) {
		std::cout << file << " (" << line << "): " << conditionStr << " " << message << "\n";
	}
}

void __assertion_2(bool condition, const char* message1, const char* message2, const char* conditionStr, const char* file, int line) {
	if (!condition) {
		std::cout << file << " (" << line << "): " << conditionStr << " " << message1 << " " << message2 << "\n";
	}
}

}
