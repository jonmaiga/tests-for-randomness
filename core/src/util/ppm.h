#pragma once

#include <fstream>
#include <iostream>
#include <vector>

namespace tfr {

inline bool write_ppm(const std::string& path, const std::vector<char>& data, int width, int height, int max) {
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

template <typename T>
void write_ppm(const std::string& filename, const int width, const int height, const std::function<T(T x, T y)>& func) {
	std::vector<char> data;
	for (T y = 0; y < height; ++y) {
		for (T x = 0; x < width; ++x) {
			T p = func(x, y);
			for (auto byte = 0; byte < sizeof(T); ++byte) {
				const unsigned char grey = (p >> (8 * byte)) & 0xff;
				data.push_back(grey);
				data.push_back(grey);
				data.push_back(grey);
			}
		}
	}
	write_ppm(filename, data, width, height, 255);
}

template <typename T>
void write_ppm(const std::string& filename, stream<T> source) {
	constexpr int size = 512;
	write_ppm<T>(
		filename,
		size, size,
		[source](T, T) mutable { return source(); });


}

}
