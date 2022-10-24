#pragma once

#include <functional>
#include <optional>
#include <string>
#include <filesystem>
#include <fstream>

#include "assertion.h"

namespace mixer {

std::optional<std::string> readAsString(const std::string& filename, size_t maxSize = SIZE_MAX);
std::string readAsStringMustExist(const std::string& filename, size_t maxSize = SIZE_MAX);
std::string readPartOfFileMustExist(const std::string& filename, size_t size);

bool write(const std::string& filename, const std::string& data);

void forEachFile(const std::string& root, const std::string& matchExtension, const std::function<void(const std::filesystem::path&)>&);
void forEachFileRecursively(const std::string& root, const std::string& matchExtension, const std::function<void(const std::filesystem::path&)>&);

void createDirectories(const std::string& path);
void mustExist(const std::string& path);

std::size_t countFiles(const std::string& dir);

template <typename T>
std::vector<T> readBinaryMustExist(const std::string& path) {
	std::ifstream t(path, std::ios::binary);
	assertion_2(t.good(), "Could not open file ", path);

	t.seekg(0, std::ios::end);
	size_t size = t.tellg();
	t.seekg(0, std::ios::beg);

	const size_t remainder = size % sizeof(T);
	size -= remainder;

	size_t size_with_T = size / sizeof(T);
	std::vector<T> data(size_with_T);
	t.read(reinterpret_cast<char*>(data.data()), size);
	T last = data.back();
	return data;
}

}
