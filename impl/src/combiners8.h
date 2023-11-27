#pragma once

#include "combiner.h"

namespace tfr {
using combiner8 = combiner<uint8_t>;

namespace combine8 {
}

template <>
inline std::vector<combiner8> get_combiners() {
	return {};
}
}
