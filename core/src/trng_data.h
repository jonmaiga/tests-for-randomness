#pragma once

#include <iostream>

#include "config.h"
#include "util/fileutil.h"

namespace tfr {

template <typename T>
std::vector<T> read_binary_data(const std::string& file_path) {
	std::cout << "Reading entire '" << file_path << "' from disk...";
	const auto& data = read_binary_must_exist_skip_remainder<T>(file_path);
	std::cout << " done!\n";
	return data;
}

template <typename T>
const std::vector<T>* get_trng_data() {
	if (!file_exists(get_config().trng_file_path())) {
		return nullptr;
	}
	static const auto trng_data = std::move(read_binary_data<T>(get_config().trng_file_path()));
	return &trng_data;
}

template <typename T>
const std::vector<T>* get_drng_data() {
	if (!file_exists(get_config().drng_file_path())) {
		return nullptr;
	}
	static const auto drng_data = std::move(read_binary_data<T>(get_config().drng_file_path()));
	return &drng_data;
}

template <typename T>
std::optional<stream<T>> create_trng_stream(std::size_t start_index = 0) {
	if (const auto* trng_data = get_trng_data<T>()) {
		return create_stream_from_data_by_ref<T>("trng", *trng_data, start_index);
	}
	return {};
}

template <typename T>
std::optional<stream<T>> create_drng_stream(std::size_t start_index = 0) {
	if (const auto* drng_data = get_drng_data<T>()) {
		return create_stream_from_data_by_ref<T>("drng", *drng_data, start_index);
	}
	return {};
}

}
