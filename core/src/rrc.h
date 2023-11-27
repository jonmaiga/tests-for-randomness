#pragma once

#include <string>
#include <vector>

#include "util/assertion.h"
#include "util/bitwise.h"

namespace tfr {
enum class rrc_type {
	identity,
	reverse,
	complement,
	reverse_complement
};

constexpr auto rrc_types = {rrc_type::identity, rrc_type::reverse, rrc_type::complement, rrc_type::reverse_complement};

template <typename T>
T permute(T c, int rot, rrc_type type) {
	switch (type) {
	case rrc_type::identity: return ror<T>(c, rot);
	case rrc_type::reverse: return ror<T>(reverse_bits(c), rot);
	case rrc_type::complement: return ror<T>(~c, rot);
	case rrc_type::reverse_complement: return ror<T>(~reverse_bits(c), rot);
	}
	assertion(false, "unknown type");
	return 0;
}

inline std::string to_string(rrc_type type) {
	switch (type) {
	case rrc_type::identity: return "identity";
	case rrc_type::reverse: return "reverse";
	case rrc_type::complement: return "complement";
	case rrc_type::reverse_complement: return "reverse_co";
	}
	assertion(false, "unknown type");
	return "unknown";
}

template <typename T>
std::vector<T> get_rrc_permutations(T x) {
	std::vector<T> xs;
	for (const auto rrc_type : rrc_types) {
		for (int rot = 0; rot < bit_sizeof<T>(); ++rot) {
			xs.push_back(permute<T>(x, rot, rrc_type));
		}
	}
	return xs;
}
}
