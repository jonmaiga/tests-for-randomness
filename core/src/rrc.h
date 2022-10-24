#pragma once
#include "util/assertion.h"

namespace mixer {

enum class rrc_type {
	identity,
	reverse,
	complement,
	reverse_complement
};

const auto rrc_types = {rrc_type::identity, rrc_type::reverse, rrc_type::complement, rrc_type::reverse_complement};

inline uint64_t ror64(uint64_t v, int r) {
	return (v >> r) | (v << (64 - r));
}

static uint64_t reverse64(uint64_t x) {
	x = (x & 0xaaaaaaaaaaaaaaaaull) >> 1 | (x & 0x5555555555555555ull) << 1;
	x = (x & 0xccccccccccccccccull) >> 2 | (x & 0x3333333333333333ull) << 2;
	x = (x & 0xf0f0f0f0f0f0f0f0ull) >> 4 | (x & 0x0f0f0f0f0f0f0f0full) << 4;
	x = (x & 0xff00ff00ff00ff00ull) >> 8 | (x & 0x00ff00ff00ff00ffull) << 8;
	x = (x & 0xffff0000ffff0000ull) >> 16 | (x & 0x0000ffff0000ffffull) << 16;
	return x >> 32 | x << 32;
}

inline uint64_t permute(uint64_t c, int rot, rrc_type type) {
	switch (type) {
	case rrc_type::identity: return ror64(c, rot);
	case rrc_type::reverse: return ror64(reverse64(c), rot);
	case rrc_type::complement: return ror64(~c, rot);
	case rrc_type::reverse_complement: return ror64(~reverse64(c), rot);
	}
	return 0;
}

inline rrc_type next_rrc(rrc_type type) {
	switch (type) {
	case rrc_type::identity: return rrc_type::reverse;
	case rrc_type::reverse: return rrc_type::complement;
	case rrc_type::complement: return rrc_type::reverse_complement;
	case rrc_type::reverse_complement: return rrc_type::identity;
	}
	return rrc_type::identity;
}

inline std::string to_string(rrc_type type) {
	switch (type) {
	case rrc_type::identity: return "identity";
	case rrc_type::reverse: return "reverse";
	case rrc_type::complement: return "complement";
	case rrc_type::reverse_complement: return "reverse_complement";
	}
	assertion(false);
	return "unknown";
}


}
