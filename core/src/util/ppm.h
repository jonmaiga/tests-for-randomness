#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

namespace noise {

inline bool writePpm(const std::string& path, const std::vector<char>& data, int width, int height, int max) {
	std::ofstream outfile(path, std::ofstream::binary);
	if (outfile.fail()) {
		std::cout << "Failed to open " << path << std::endl;
		return false;
	}

	outfile << "P6" << "\n" << width << " " << height << "\n" << max << "\n";
	outfile.write(data.data(), data.size());
	outfile.close();
	return !outfile.fail();
}

inline void write(const std::string& filename, const int width, const int height, std::function<double(double x, double y)> func) {
	std::vector<char> data;
	unsigned char max = 0;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			uint64_t p = static_cast<uint64_t>(func(x, y));
			unsigned char grey = p & 0xff;
			data.push_back(grey);
			data.push_back(grey);
			data.push_back(grey);
			max = std::max(grey, max);
		}
	}
	writePpm(filename, data, width, height, max);
}

}