#pragma once

namespace mixer_internal {

void __assertion(bool condition, const char* message, const char* conditionStr, const char* file, int line);
void __assertion_2(bool condition, const char* message1, const char* message2, const char* conditionStr, const char* file, int line);

}

#ifdef MIXER_DEVELOPMENT
#   define assertion(expression, message) (mixer_internal::__assertion(expression, message, #expression, __FILE__, __LINE__))
#   define assertion_2(expression, message1, message2) (mixer_internal::__assertion_2(expression, message1, message2, #expression, __FILE__, __LINE__))
#else
#   define assertion(expression, message) (0);
#   define assertion_2(expression, message1, message2) (0);
#endif