#include "fileutil.h"
#include <fstream>

namespace tfr {

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

void for_each_file(const std::string& root, const std::string& matchExtension, const std::function<void(const std::filesystem::path&)>& callback) {
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

void for_each_file_recursively(const std::string& root, const std::string& matchExtension, const std::function<void(const std::filesystem::path&)>& callback) {
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

void create_directories(const std::string& path) {
	if (std::filesystem::is_directory(path)) {
		return;
	}
	std::filesystem::create_directories(path);
}

bool file_exists(const std::string& path) {
	return std::filesystem::exists(path);
}

}
