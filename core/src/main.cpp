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
	app(table, baseline);
	app(table, evaluate_rrc(mx3, n));
	app(table, evaluate_rrc(nasam, n));
	app(table, evaluate_rrc(xmxmxm, n));
	app(table, evaluate_rrc(moremur, n));
	app(table, evaluate_rrc(lea64, n));
	app(table, evaluate_rrc(degski64, n));
	app(table, evaluate_rrc(murmur3, n));
	app(table, evaluate_rrc(xxh3, n));
	app(table, evaluate_rrc(fast_hash, n));
	return 0;
}
