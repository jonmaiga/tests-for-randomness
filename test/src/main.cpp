#include <iostream>
#include "evaluate.h"
#include "mixers64.h"
#include "trng_data.h"
#include "command/ppm_command.h"
#include "command/exhaust_command.h"
#include "command/inspect_test_command.h"
#include "command/test_command.h"
#include "command/tune_command.h"

#include "search/search_setup.h"

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

}

int main(int argc, char** args) {
	using namespace mixer;
	try {
		if (argc != 3) {
			std::cout << "Usage: mixer.exe root-path command\n";
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
			using T = uint64_t;
			run_sffs<T>();
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
		if (command == "-tune") {
			using T = uint32_t;
			tune_command();
			return 0;
		}
		if (command == "-inspect-test") {
			inspect_test_command();
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
