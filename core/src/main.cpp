#include <iostream>
#include "eval_mixer.h"
#include "format_result.h"
#include "mixers.h"


void build_trng() {
	std::vector<char> data;
	//C:\tmp\random.org\raw\
	
	mixer::forEachFileRecursively(R"(C:\tmp\random.org\raw\random.org-pregenerated-2022-09-bin\)", ".bin", [&data](const auto& path) {
		auto r = mixer::readBinaryMustExist<char>(path.string());
		data.insert(data.end(), r.begin(), r.end());
		std::cout << path.string() << "\n";
	});

	const std::string data_str(data.begin(), data.end());
	mixer::write(R"(C:\tmp\random.org\raw\trng_small.bin)", data_str);
}


int main(int argc, char** args) {
	using namespace mixer;

	const auto trng_stream = create_stream_from_data("trng", get_trng_data());
	const auto trng = create_mixer_from_stream("trng", trng_stream);

	constexpr auto n = 1000;
	result_analyzer analyzer;
	analyzer.add(evaluate_rrc(trng, n));
	analyzer.add(evaluate_rrc(mx3, n));
	analyzer.add(evaluate_rrc(nasam, n));
	analyzer.add(evaluate_rrc(xmxmxm, n));
	analyzer.add(evaluate_rrc(moremur, n));
	analyzer.add(evaluate_rrc(lea64, n));
	analyzer.add(evaluate_rrc(degski64, n));
	analyzer.add(evaluate_rrc(murmur3, n));
	analyzer.add(evaluate_rrc(xxh3, n));
	analyzer.add(evaluate_rrc(fast_hash, n));

	std::cout << analyzer.summarize_ks() << "\n";
	return 0;
}
