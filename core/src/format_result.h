#pragma once

#include <iostream>
#include "util/Table.h"

namespace mixer {

inline Table& col(Table& table, const avalanche_stats& stats) {
	return table.col(stats.std_dev_bias).
	             col(stats.mean_bias).
	             col(stats.max_bias);
}

inline void app(Table& table, const test_result& r) {
	avalanche_result worst;
	avalanche_result summed;
	for (const auto& rr : r.results) {
		if (rr.bic.max_bias > worst.bic.max_bias) {
			worst = rr;
		}
		summed.bic.max_bias += rr.bic.max_bias;
		summed.bic.mean_bias += rr.bic.mean_bias;
		summed.bic.std_dev_bias += rr.bic.std_dev_bias;

		summed.sac.max_bias += rr.sac.max_bias;
		summed.sac.mean_bias += rr.sac.mean_bias;
		summed.sac.std_dev_bias += rr.sac.std_dev_bias;

		summed.n += rr.n;
	}

	table.col(worst.mixer_name).col(worst.stream_name);
	col(table, worst.sac);
	col(table, worst.bic);
	table.col(worst.n).row();

	std::cout << table.to_string() << "\n";
}

}
