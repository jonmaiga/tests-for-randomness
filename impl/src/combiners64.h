#pragma once

#include "combiner.h"

namespace tfr {

using combiner64 = combiner<uint64_t>;

namespace combine64 {


}

template <>
inline std::vector<combiner64> get_combiners() {
	return {};
}

}
