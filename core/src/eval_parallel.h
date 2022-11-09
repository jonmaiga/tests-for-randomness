#pragma once

#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include "test_streams.h"
#include "statistics/andersondarling.h"
#include "statistics/avalanche.h"
#include "statistics/basic.h"
#include "statistics/chi2.h"
#include "statistics/correlation.h"
#include "statistics/kolmogorov.h"
#include "statistics/waldwolfowitz.h"
#include "util/jobs.h"

namespace mixer {

using test_job_return = std::vector<result>;
using test_jobs = jobs<test_job_return>;

namespace internal {

inline stream create_stream(const test_config& cfg) {
	auto s = create_stream_from_mixer(cfg.source, cfg.mixer);
	if (cfg.append_stream_factory) {
		return cfg.append_stream_factory(s);
	}
	return s;
}

inline test_jobs create_stream_jobs(const stream_test& test, const std::vector<test_factory>& test_factories) {
	test_jobs js;
	for (const auto& factory : test_factories) {
		js.push_back([test, factory]() {
			const auto cfg = factory();
			std::vector<result> results;
			const auto s = internal::create_stream(cfg);
			for (const auto& r : test(cfg.n, s)) {
				results.push_back({cfg.source.name, cfg.mixer.name, r});
			}
			return results;
		});
	}
	return js;
}

inline test_jobs create_mixer_jobs(const mixer_test& test, const std::vector<test_factory>& test_factories) {
	test_jobs js;
	for (const auto& factory : test_factories) {
		js.push_back([test, factory]() {
			const auto cfg = factory();
			std::vector<result> results;
			for (const auto& r : test(cfg.n, cfg.source, cfg.mixer)) {
				results.push_back({cfg.source.name, cfg.mixer.name, r});
			}
			return results;
		});
	}
	return js;
}

inline test_jobs create_test_jobs(const std::vector<test_factory>& test_factories) {
	test_jobs jobs;
	append(jobs, create_stream_jobs(basic_test, test_factories));
	append(jobs, create_stream_jobs(chi2_test, test_factories));
	append(jobs, create_stream_jobs(kolmogorov_test, test_factories));
	append(jobs, create_stream_jobs(anderson_darling_test, test_factories));
	append(jobs, create_stream_jobs(wald_wolfowitz_test, test_factories));
	append(jobs, create_stream_jobs(pearson_correlation_test, test_factories));
	append(jobs, create_stream_jobs(spearman_correlation_test, test_factories));
	append(jobs, create_stream_jobs(kendall_correlation_test, test_factories));

	append(jobs, create_mixer_jobs(avalanche_mixer_sac_test, test_factories));
	append(jobs, create_mixer_jobs(avalanche_mixer_bic_test, test_factories));
	return jobs;
}

inline test_result test_rrc_parallel(const mixer& mixer, uint64_t n, unsigned int num_threads) {
	test_result test_result{"rrc", mixer.name};
	const auto collect_job_results = [&](const test_job_return& results) {
		static std::mutex m;
		std::lock_guard lg(m);
		test_result.add(results);
	};

	const auto jobs = internal::create_test_jobs(create_rrc_test_factories(mixer, n));
	run_jobs<test_job_return>(jobs, collect_job_results, num_threads);
	return test_result;
}

}

inline test_result test_rrc_parallel(const mixer& mixer, uint64_t n) {
	const auto hw_threads = std::thread::hardware_concurrency();
	const auto num_threads = hw_threads > 1 ? hw_threads - 1 : hw_threads;
	return internal::test_rrc_parallel(mixer, n, num_threads);
}

inline test_result test_rrc(const mixer& mixer, uint64_t n) {
	return internal::test_rrc_parallel(mixer, n, 1);
}

}
