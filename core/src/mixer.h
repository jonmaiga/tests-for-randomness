#pragma once

namespace mixer {

struct mixer64 {
	std::string name;
	std::function<uint64_t(uint64_t)> mix;

	uint64_t operator()(uint64_t x) const {
		return mix(x);
	}
};

}