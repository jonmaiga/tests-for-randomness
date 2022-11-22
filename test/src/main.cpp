#include <iostream>
#include "eval_parallel.h"
#include "format_result.h"
#include "mixers32.h"
#include "mixers64.h"
#include "trng_data.h"

#include "search/search_mixer_constants_setup.h"

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

void write_stream(const mixer64& m, uint64_t n) {
	std::vector<uint64_t> data;
	for (uint64_t i = 0; i < n; ++i) {
		data.push_back(m(i));
	}
	const auto from = reinterpret_cast<char*>(data.data());
	const std::string data_str(from, from + data.size() * sizeof(uint64_t));
	write(R"(/Users/jonkagstrom/root/github/bit_mixer_evaluation/mx3.bin)", data_str);
}

template <typename T>
using test_method = std::function<test_battery_result(const mixer<T>&, uint64_t)>;

template <typename T>
void run_tests() {
	const auto trng_stream = create_stream_from_data_by_ref_thread_safe<T>("trng", get_trng_data<T>());
	const auto trng1 = create_mixer_from_stream<T>("trng1", trng_stream);
	const auto trng2 = create_mixer_from_stream<T>("trng2", trng_stream);

	const auto test = test_rrc_parallel<T>;
	constexpr auto n = 20000;

	const mixer64 test_mixer = {
		"test", [](uint64_t x) {
			x ^= x >> 3;
			x *= 17192186266073230512ull;
			x ^= x >> 23;
			return x;
		}
	};

	std::cout << "n=" << n << "\n";

	result_analyzer analyzer;
	//analyzer.add(test(trng1, n));
	//analyzer.add(test(trng2, n));

	for (const auto& m : get_mixers<T>()) {
		analyzer.add(test(m, n));
	}

	analyzer.summarize_fails({}, {
		                         "trng",
		                         "counter-1",
		                         "counter-23",
		                         "graycode-",
		                         // "graycode-4",
		                         // "graycode-8",
		                         "bit-",
		                         // "reverse-",
		                         // "identity-",
		                         // "complement-",
		                         // "reverse_co-"
	                         });
	analyzer.summarize_fails({});
	std::cout << "done.";
}

}

int main(int argc, char** args) {
	try {
		using T = uint64_t;
		mixer::run_tests<T>();
		mixer::run_search<T>();
	}
	catch (std::runtime_error& e) {
		std::cout << "ERROR: " << e.what() << "\n";
	}
	return 0;
}
