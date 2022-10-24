#pragma once

#include "util/fileutil.h"
#include "rrc.h"

namespace mixer {


struct stream {
	std::string name;
	std::function<std::optional<uint64_t>()> next;

	std::optional<uint64_t> operator()() const {
		return next();
	}
};

using stream_factory = std::function<stream()>;

struct generated_stream {
	std::vector<uint64_t> data;
	std::size_t index = 0;

	std::optional<uint64_t> operator()() {
		if (index == data.size()) {
			return {};
		}
		return data[index++];
	}
};

struct counter_stream {
	uint64_t increment{};
	uint64_t n{};
	uint64_t index{};

	std::optional<uint64_t> operator()() {
		if (n == 0) {
			return {};
		}
		--n;
		index += increment;
		return index;
	}
};

struct test_stream {
	uint64_t n{};
	uint64_t index{};

	std::optional<uint64_t> operator()() {
		if (index == n) {
			return {};
		}
		return 2 ^ index++ + 1;
	}
};

inline stream create_counter_stream(uint64_t increment, std::size_t n) {
	return {"counter-" + std::to_string(increment), counter_stream{increment, n}};
}

inline stream create_xor_stream(std::size_t n) {
	return {"A256008", test_stream{n}};
}


inline stream createStreamFromBinaryFile(const std::filesystem::path& path, int n) {
	auto data = readBinaryMustExist<uint64_t>(path.string());
	if (n < data.size()) {
		data.resize(n);
	}
	return {path.filename().string(), generated_stream{data}};
}


inline stream add_rrc(const stream& source, int rotation, rrc_type type) {
	const auto name = to_string(type) + "-" + std::to_string(rotation) + "(" + source.name + ")";
	return {
		name, [source, rotation, type]()-> std::optional<uint64_t> {
			if (const auto x = source()) {
				return permute(*x, rotation, type);
			}
			return {};
		}
	};
}


}