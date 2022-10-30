#include <iostream>
#include "eval_mixer.h"
#include "format_result.h"
#include "mixers.h"

namespace mixer {

void build_trng() {
	std::vector<char> data;
	forEachFileRecursively(R"(C:\tmp\random.org\raw\random.org-pregenerated-2022-09-bin\)", ".bin", [&data](const auto& path) {
		auto r = readBinaryMustExist<char>(path.string());
		data.insert(data.end(), r.begin(), r.end());
		std::cout << path.string() << "\n";
	});

	const std::string data_str(data.begin(), data.end());
	write(R"(C:\tmp\random.org\raw\trng_small.bin)", data_str);
}

void write_stream(const mixer& m, uint64_t n) {
	std::vector<uint64_t> data;
	for (uint64_t i = 0; i < n; ++i) {
		data.push_back(m(i));
	}
	const auto from = reinterpret_cast<char*>(data.data());
	const std::string data_str(from, from + data.size() * sizeof(uint64_t));
	write(R"(/Users/jonkagstrom/root/github/bit_mixer_evaluation/mx3.bin)", data_str);
}

inline void run_tests() {
	using test_method = std::function<test_result(const mixer&, uint64_t)>;

	const auto trng_stream = create_stream_from_data("trng", get_trng_data());
	const auto trng = create_mixer_from_stream("trng", trng_stream);

	const test_method test = test_rrc;
	constexpr auto n = 1000;

	std::cout << "n=" << n << "\n";
	result_analyzer analyzer;
	analyzer.add(test(trng, n));
	analyzer.add(test(mx3, n));
	analyzer.add(test(nasam, n));
	analyzer.add(test(xmx, n));
	analyzer.add(test(xmxmxm, n));
	analyzer.add(test(moremur, n));
	analyzer.add(test(lea64, n));
	analyzer.add(test(degski64, n));
	analyzer.add(test(murmur3, n));
	analyzer.add(test(xxh3, n));
	analyzer.add(test(fast_hash, n));
}

}

int main(int argc, char** args) {
	try {
		//write_stream(mixer::mx3, 100000000ull);
		mixer::run_tests();
	}
	catch (std::runtime_error& e) {
		std::cout << "ERROR: " << e.what() << "\n";
	}
	return 0;
}
