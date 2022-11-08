#pragma once

#include <mutex>
#include <thread>

#include "eval_mixer.h"

namespace mixer {

using job = std::function<std::vector<result>()>;
using jobs = std::vector<job>;

inline jobs create_stream_jobs(const stream_test& test, const std::vector<test_factory>& test_factories) {
	jobs js;
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

inline jobs create_mixer_jobs(const mixer_test& test, const std::vector<test_factory>& test_factories) {
	jobs js;
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

inline jobs create_test_jobs(const std::vector<test_factory>& test_factories) {
	jobs jobs;
	append(jobs, create_stream_jobs(basic_test, test_factories));
	append(jobs, create_stream_jobs(chi2_test, test_factories));
	append(jobs, create_stream_jobs(kolmogorov_test, test_factories));
	append(jobs, create_stream_jobs(anderson_darling_test, test_factories));
	append(jobs, create_stream_jobs(wald_wolfowitz_test, test_factories));

	append(jobs, create_mixer_jobs(avalanche_mixer_sac_test, test_factories));
	append(jobs, create_mixer_jobs(avalanche_mixer_bic_test, test_factories));
	append(jobs, create_mixer_jobs(pearson_correlation_mixer_test, test_factories));
	append(jobs, create_mixer_jobs(spearman_correlation_mixer_test, test_factories));
	//append(jobs, create_mixer_jobs(kendall_correlation_mixer_test, test_factories));
	return jobs;
}

inline test_result test_rrc_parallel(const mixer& mixer, uint64_t n) {
	auto jobs = create_test_jobs(create_rrc_test_factories(mixer, n));

	const auto job_queue = [&jobs]()-> std::optional<job> {
		static std::mutex m;
		std::lock_guard lg(m);
		if (jobs.empty()) {
			return {};
		}
		auto j = jobs.back();
		jobs.pop_back();
		return j;
	};

	std::vector<result> results;
	const auto result_queue = [&results](const std::vector<result>& rs) {
		static std::mutex m;
		std::lock_guard lg(m);
		append(results, rs);
	};

	std::vector<std::thread> threads;
	for (std::size_t i = 0; i < std::thread::hardware_concurrency() - 4; ++i) {
		threads.emplace_back([&job_queue, &result_queue]() {
			while (const auto& job = job_queue()) {
				result_queue((*job)());
			}
		});
	}

	for (auto& t : threads) {
		t.join();
	}

	test_result test_result{"rrc_parallel", mixer.name};
	test_result.add(results);
	return test_result;
}

}
