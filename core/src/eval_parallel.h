#pragma once

#include <mutex>
#include <thread>

#include "eval_mixer.h"
#include "util/jobs.h"

namespace mixer {

using test_job_return = std::vector<result>;
using test_jobs = jobs<test_job_return>;

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

inline test_result test_rrc_parallel(const mixer& mixer, uint64_t n) {

	test_result test_result{"rrc_parallel", mixer.name};
	const auto collect_job_results = [&](const test_job_return& results) {
		static std::mutex m;
		std::lock_guard lg(m);
		test_result.add(results);
	};

	const auto jobs = create_test_jobs(create_rrc_test_factories(mixer, n));
	run_jobs<test_job_return>(jobs, collect_job_results, std::thread::hardware_concurrency() - 2);
	return test_result;
}

}
