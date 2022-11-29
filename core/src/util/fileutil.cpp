#include "fileutil.h"
#include "assertion.h"

#include <algorithm>
#include <fstream>

namespace mixer {

std::optional<std::string> readAsString(const std::string& filename, size_t maxSize) {
	std::ifstream t(filename, std::ios::binary);
	if (!t.good()) {
		return {};
	}

	t.seekg(0, std::ios::end);
	const size_t g = t.tellg();
	const auto size = std::min(g, maxSize);
	t.seekg(0, std::ios::beg);

	std::string str(size, '\0');
	t.read(str.data(), size);
	assertion_2(str.size() == size, "Unexpected size of read string ", filename.c_str());
	return str;
}

std::string readAsStringMustExist(const std::string& filename, size_t maxSize) {
	const auto result = readAsString(filename, maxSize);
	assertion_2(result.has_value(), "File not found: ", filename.c_str());
	return *result;
}

std::string readPartOfFileMustExist(const std::string& filename, size_t size) {
	const std::string& data = readAsStringMustExist(filename, size);
	const auto lastNl = data.rfind('\n');
	assertion_2(lastNl != std::string::npos, "Could not find a last new line: ", filename.c_str());
	return data.substr(0, lastNl);
}

bool write(const std::string& filename, const std::string& data, bool append) {
	std::ofstream file(filename, std::ios::out | (append ? std::ios::app : std::ios::trunc) | std::ios::binary);
	file << data;
	file.close();
	return !file.fail();
}

bool write(const std::string& filename, const std::string& data) {
	return write(filename, data, false);
}

bool write_append(const std::string& filename, const std::string& data) {
	return write(filename, data, true);
}

void forEachFile(const std::string& root, const std::string& matchExtension, const std::function<void(const std::filesystem::path&)>& callback) {
	for (const auto& dirEntry : std::filesystem::directory_iterator(root)) {
		if (!is_regular_file(dirEntry)) {
			continue;
		}
		if (dirEntry.path().extension() != matchExtension) {
			continue;
		}
		callback(dirEntry.path());
	}
}

void forEachFileRecursively(const std::string& root, const std::string& matchExtension, const std::function<void(const std::filesystem::path&)>& callback) {
	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(root)) {
		if (!is_regular_file(dirEntry)) {
			continue;
		}
		if (dirEntry.path().extension() != matchExtension) {
			continue;
		}
		callback(dirEntry.path());
	}
}

void createDirectories(const std::string& path) {
	if (std::filesystem::is_directory(path)) {
		return;
	}
	std::filesystem::create_directories(path);
}

void mustExist(const std::string& path) {
	if (!std::filesystem::exists(path)) {
		throw std::invalid_argument(path + " doesn't exist.");
	}
}

std::size_t countFiles(const std::string& dir) {
	using std::filesystem::directory_iterator;
	return std::distance(directory_iterator(dir), directory_iterator{});
}

}
