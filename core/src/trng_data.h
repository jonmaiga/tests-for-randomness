#pragma once

#include <iostream>
#include <span>

#include "config.h"
#include "util/fileutil.h"

namespace tfr {namespace detail {
inline std::vector<uint8_t> read_binary_data(const std::string& file_path) {
	std::cout << "Reading entire '" << file_path << "' from disk...";
	const auto& data = read_binary_must_exist_skip_remainder<uint8_t>(file_path);
	std::cout << " done!\n";
	return data;
}

inline const std::vector<uint8_t>& get_or_read_trng_data() {
	static const auto trng_data = read_binary_data(get_config().trng_file_path());
	return trng_data;
}

inline const std::vector<uint8_t>& get_or_read_drng_data() {
	static const auto drng_data = read_binary_data(get_config().drng_file_path());
	return drng_data;
}
}

template <typename T>
std::optional<std::span<const T>> get_trng_data() {
	if (!file_exists(get_config().trng_file_path())) {
		return {};
	}
	const auto& data = detail::get_or_read_trng_data();
	const auto* ptr = reinterpret_cast<const T*>(data.data());
	const std::size_t size = data.size() / sizeof(T);
	return std::span<const T>{ptr, size};
}

template <typename T>
std::optional<std::span<const T>> get_drng_data() {
	if (!file_exists(get_config().drng_file_path())) {
		return {};
	}
	const auto& data = detail::get_or_read_drng_data();
	const auto* ptr = reinterpret_cast<const T*>(data.data());
	const std::size_t size = data.size() / sizeof(T);
	return std::span<const T>{ptr, size};
}

template <typename T>
std::optional<stream<T>> create_trng_stream(std::size_t start_index = 0) {
	if (const auto trng_data = get_trng_data<T>()) {
		return create_stream_from_data_by_ref("trng", *trng_data, start_index);
	}
	return {};
}

template <typename T>
std::optional<stream<T>> create_drng_stream(std::size_t start_index = 0) {
	if (const auto drng_data = get_drng_data<T>()) {
		return create_stream_from_data_by_ref("drng", *drng_data, start_index);
	}
	return {};
}
}
