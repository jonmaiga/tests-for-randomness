#pragma once

#include "combiners32.h"
#include "combiners64.h"
#include "evaluate.h"
#include "mixers32.h"
#include "mixers64.h"
#include "mixers8.h"
#include "prngs32.h"
#include "prngs64.h"
#include "streams.h"
#include "util/test_setups.h"

namespace mixer {

template <typename T>
streams<T> create_fail_sources() {
	const auto& mix = get_default_mixer<T>();

	streams<T> sources;

	// constant
	sources.push_back(create_constant_stream<T>(0));
	sources.push_back(create_constant_stream<T>(1));
	sources.push_back(create_constant_stream<T>(mix(123)));

	// counter-1
	sources.push_back(create_counter_stream<T>(1));

	return sources;
}

template <typename T>
streams<T> create_pass_sources() {
	const auto trng = [] {
		return create_stream_from_data_by_ref<T>("trng", get_trng_data<T>());
		//return create_stream_from_mixer<T>(create_counter_stream<T>(1), get_default_mixer<T>());
	};
	return {trng()};
}

inline auto create_test_inspect_callback(int max_power) {
	return [max_power](const test_battery_result& br) {
		if (const auto meta = get_worst_meta_analysis(br)) {
			return meta->pass();
		}
		return br.power_of_two() < max_power;
	};
}

template <typename T>
struct inspect_result {
	streams<T> passed;
	streams<T> failed;
};

template <typename T>
inspect_result<T> inspect_test(const test_definition<T>& test_def, const streams<T>& sources) {
	const auto callback = create_test_inspect_callback(20);

	const mixer<T> identity_mixer = {
		"identity", [](T x) {
			return x;
		}
	};

	inspect_result<T> result;
	for (const auto& source : sources) {
		auto setup = test_setup<T>{test_def.name, {source}, {test_def.type}, identity_mixer};
		auto br = evaluate_multi_pass(callback, setup);
		auto meta = get_worst_meta_analysis(br);
		if (meta->pass()) {
			result.passed.push_back(source);
		}
		else {
			result.failed.push_back(source);
		}
	}
	return result;
}

template <typename T>
void inspect_tests(const streams<T>& sources, bool all_should_pass) {
	auto rrc_sources = create_rrc_sources(sources);
	const auto get_sources = [rrc_sources, sources](const test_definition<T>& t) {
		if (t.type == test_type::mean) {
			return sources;
		}
		return rrc_sources;
	};

	const auto is_ok = [](const test_definition<T>& def, streams<T> fail_sources, bool all_should_pass) {
		if (fail_sources.empty()) {
			return true;
		}

		if (def.name == "ww" && !all_should_pass) {
			return fail_sources.size() == 4;
		}

		return false;
	};

	for (const auto& test_def : get_tests<T>()) {
		if (all_should_pass && test_def.test_mixer) {
			continue;
		}
		auto result = inspect_test<T>(test_def, get_sources(test_def));
		const auto& fail_sources = all_should_pass ? result.failed : result.passed;
		if (is_ok(test_def, fail_sources, all_should_pass)) {
			std::cout << "PASS: " << test_def.name << "\n";
		}
		else {
			for (const auto& s : fail_sources) {
				std::cout << "FAIL: " << test_def.name << ": " << s.name << "\n";
			}
		}
	}
}

template <typename T>
void inspect_test_command() {
	std::cout << "==============================================\n";
	std::cout << "Fail sources, " << bit_sizeof<T>() << " bits\n";
	std::cout << "==============================================\n";
	inspect_tests<T>(create_fail_sources<T>(), false);

	std::cout << "==============================================\n";
	std::cout << "Pass sources, " << bit_sizeof<T>() << " bits\n";
	std::cout << "==============================================\n";
	inspect_tests<T>(create_pass_sources<T>(), true);
}

using per_test_result = std::map<std::string, std::map<std::string, std::string>>;

inline auto create_per_test_callback(per_test_result& result, int max_power, bool print_intermediate_results = true) {
	return [&result, max_power, print_intermediate_results](const test_battery_result& br) {
		bool proceed = br.power_of_two() < max_power;
		if (proceed) {
			if (const auto meta = get_worst_meta_analysis(br)) {
				proceed = meta->pass();
			}
		}
		if (print_intermediate_results || !proceed) {
			print_battery_result(br);
		}
		if (!proceed) {
			const auto test_name = get_test_name(br.results.begin()->first.type);
			result[br.test_subject_name][test_name] = std::to_string(br.power_of_two());
		}
		return proceed;
	};
}


template <typename T>
void write(const per_test_result& result) {
	std::stringstream ss;
	ss << ";";
	for (const auto& test : get_tests<T>()) {
		ss << test.name << ";";
	}
	ss << "\n";
	for (const auto& e : result) {
		ss << e.first << ";";
		for (const auto& test : get_tests<T>()) {
			auto it = e.second.find(test.name);
			if (it != e.second.end()) {
				ss << it->second;
			}
			ss << ";";
		}
		ss << "\n";
	}
	write(get_config().result_path() + "per_test" + std::to_string(bit_sizeof<T>()) + ".txt", ss.str());
}


template <typename T>
void inspect_per_test_command() {
	per_test_result result;

	const auto callback = create_per_test_callback(result, 20, false);

	for (const auto& test : get_tests<T>()) {
		std::cout << "========================\n";
		std::cout << test.name << "\n";
		std::cout << "========================\n";

		// trng
		evaluate_multi_pass(callback, create_trng_test_setup<T>().set_tests({test.type}));

		// mixers
		for (const auto& m : get_mixers<T>()) {
			evaluate_multi_pass(callback, create_test_setup(m).set_tests({test.type}));
		}

		// combiners
		for (const auto& combiner : get_combiners<T>()) {
			evaluate_multi_pass(callback, create_combiner_test_setup<T>(combiner).set_tests({test.type}));
		}

		// prngs
		for (const auto& prng : get_prngs<T>()) {
			evaluate_multi_pass(callback, create_prng_setup<T>(prng).set_tests({test.type}));
		}
		write<T>(result);
	}
}

}
