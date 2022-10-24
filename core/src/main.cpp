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
	//build_trng();

	constexpr auto n = 10000;
	const auto baseline = evaluate_trng(n);

	Table table({"mixer", "stream", "sac_std_bias", "sac_avg_bias", "sac_max_bias", "bic_std_bias", "bic_avg_bias", "bic_max_bias", "n"});
	add_worst(table, baseline);
	add_worst(table, evaluate(mx3, n));
	add_worst(table, evaluate(nasam, n));
	add_worst(table, evaluate(xmxmxm, n));
	add_worst(table, evaluate(moremur, n));
	add_worst(table, evaluate(lea64, n));
	add_worst(table, evaluate(degski64, n));
	add_worst(table, evaluate(murmur3, n));
	add_worst(table, evaluate(xxh3, n));
	add_worst(table, evaluate(fast_hash, n));

	return 0;
}
