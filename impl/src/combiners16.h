#pragma once

#include "combiner.h"

namespace tfr {
using combiner16 = combiner<uint16_t>;

namespace combine16 {
}

template <>
inline std::vector<combiner16> get_combiners() {
	return {};
}
}
