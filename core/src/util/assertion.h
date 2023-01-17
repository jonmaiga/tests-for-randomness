#pragma once

namespace mixer_internal {

void assertion_impl(bool condition, const char* message, const char* conditionStr, const char* file, int line);
void assertion_2_impl(bool condition, const char* message1, const char* message2, const char* conditionStr, const char* file, int line);

}

#ifdef MIXER_DEVELOPMENT
#   define assertion(expression, message) (mixer_internal::assertion_impl(expression, message, #expression, __FILE__, __LINE__))
#   define assertion_2(expression, message1, message2) (mixer_internal::assertion_2_impl(expression, message1, message2, #expression, __FILE__, __LINE__))
#else
#   define assertion(expression, message) (0);
#   define assertion_2(expression, message1, message2) (0);
#endif