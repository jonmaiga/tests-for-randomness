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

	using test_method = std::function<test_result(const ::mixer::mixer&, uint64_t)>;

	const auto trng_stream = create_stream_from_data("trng", get_trng_data());
	const auto trng = create_mixer_from_stream("trng", trng_stream);

	const test_method test = evaluate_rrc;
	constexpr auto n = 1000;

	result_analyzer analyzer;

	analyzer.add(test(trng, n));
	analyzer.add(test(mx3, n));
	analyzer.add(test(nasam, n));
	analyzer.add(test(xmxmxm, n));
	analyzer.add(test(moremur, n));
	analyzer.add(test(lea64, n));
	analyzer.add(test(degski64, n));
	analyzer.add(test(murmur3, n));
	analyzer.add(test(xxh3, n));
	analyzer.add(test(fast_hash, n));

	std::cout << analyzer.summarize_avalanche() << "\n";
	std::cout << analyzer.summarize_ks() << "\n";
	return 0;
}
