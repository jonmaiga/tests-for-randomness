#include <iostream>
#include "eval_parallel.h"
#include "format_result.h"
#include "mixers32.h"
#include "mixers64.h"
#include "trng_data.h"
#include "command/test_command.h"

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

	constexpr auto n = 100000;

	result_analyzer analyzer;
	//analyzer.add(test_parallel<T>({
	//	trng1,
	//	create_rrc_test_factories(trng1, n),
	//	all_test_types
	//}));

	for (const auto& mixer : get_mixers<T>()) {
		auto ts = test_setup<T>{
			mixer,
			create_rrc_sources<T>(),
			all_test_types
		};
		analyzer.add(test_parallel(n, ts));
	}

	analyzer.summarize_fails({}, {"trng", "counter-1", "graycode-"});
	analyzer.summarize_fails({});
	std::cout << "done.";
}

}

int main(int argc, char** args) {
	using namespace mixer;
	try {
		if (argc != 3) {
			std::cout << "Usage: mixer.exe trng-filename command\n";
			return 1;
		}
		const std::string trng_path = args[1];
		set_config({trng_path});

		const std::string command = args[2];
		std::cout << "Executing command " << command << "\n";
		if (command == "-test") {
			test_command();
			return 0;
		}

		std::cout << "Unknown command\n";
		return 1;

		using T = uint32_t;
		run_tests<T>();
		run_search<T>();
		//return 0;

	}
	catch (std::runtime_error& e) {
		std::cout << "ERROR: " << e.what() << "\n";
	}
	return 0;
}
