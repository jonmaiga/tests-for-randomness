#include <iostream>
#include "evaluate.h"
#include "trng_data.h"
#include "command/exhaust_command.h"
#include "command/inspect_test_command.h"
#include "command/ppm_command.h"
#include "command/test_command.h"
#include "search/search_setup.h"

namespace tfr {

void build_trng() {
	std::vector<char> data;
	forEachFileRecursively(R"(C:\tmp\random.org\raw\)", ".bin", [&data](const auto& path) {
		auto r = readBinaryMustExist<char>(path.string());
		data.insert(data.end(), r.begin(), r.end());
		std::cout << path.string() << "\n";
	});
	write_binary(R"(C:\tmp\random.org\trng.bin)", data, false);
}

void export_rdrand(int power_of_two) {
	const auto n = 1ull << power_of_two;
	const auto& cfg = get_config();
	std::vector<uint64_t> data;
	data.reserve(n);
	for (std::size_t i = 0; i < n; ++i) {
		data.push_back(rdrand_64());
		if (i % 10000000 == 0) {
			std::cout << ".";
		}
	}
	write_binary(cfg.drng_path(), data, true);
}

}

int main(int argc, char** args) {
	using namespace tfr;
	try {
		if (argc != 3) {
			std::cout << "Usage: tfr-tool.exe root-path command\n";
			return 1;
		}
		const std::string root_path = args[1];
		set_config({root_path});

		const std::string command = args[2];
		std::cout << "Executing command " << command << "\n";
		if (command == "-test") {
			test_command();
			return 0;
		}
		if (command == "-sffs") {
			using T = uint32_t;
			run_sffs<T>();
			return 0;
		}
		if (command == "-inspect-test") {
			inspect_test_command<uint32_t>();
			inspect_test_command<uint64_t>();
			inspect_per_test_command<uint32_t>();
			inspect_per_test_command<uint64_t>();
			return 0;
		}
		if (command == "-exhaust") {
			exhaust_command();
			return 0;
		}
		if (command == "-ppm") {
			using T = uint32_t;
			ppm_command<T>();
			return 0;
		}


		std::cout << "Unknown command\n";
		return 1;
	}
	catch (std::runtime_error& e) {
		std::cout << "ERROR: " << e.what() << "\n";
	}
	return 0;
}
