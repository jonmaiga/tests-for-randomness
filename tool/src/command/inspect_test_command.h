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

namespace tfr {
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
	streams<T> sources;
	if (const auto trng_stream = create_trng_stream<T>()) {
		sources.push_back(*trng_stream);
	}
	if (const auto drng_stream = create_drng_stream<T>()) {
		sources.push_back(*drng_stream);
	}
	if (sources.empty()) {
		std::cout << "Warning: No passable trng/drng sources was found\n";
	}
	return sources;
}

inline auto create_test_inspect_callback() {
	return [](const test_battery_result& br, bool) {
		if (const auto analysis = get_worst_statistic_analysis(br)) {
			return analysis->pass();
		}
		return true;
	};
}

template <typename T>
struct inspect_result {
	streams<T> passed;
	streams<T> failed;
};

template <typename T>
inspect_result<T> inspect_test(const test_definition<T>& test_def, const streams<T>& sources) {
	const auto callback = create_test_inspect_callback();

	const mixer<T> identity_mixer = {
		"identity", [](T x) {
			return x;
		}
	};

	inspect_result<T> result;
	for (const auto& source : sources) {
		auto setup = test_setup<T>{test_def.name, {source}, {test_def.type}, identity_mixer};
		auto br = evaluate_multi_pass(callback, setup);
		if (auto analysis = get_worst_statistic_analysis(br)) {
			if (analysis->pass()) {
				result.passed.push_back(source);
			}
			else {
				result.failed.push_back(source);
			}
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

inline auto create_per_test_callback(per_test_result& result,
                                     const std::function<std::string(const test_battery_result&)>& extract_property,
                                     bool print_intermediate_results = true) {
	return [&result, print_intermediate_results, extract_property](const test_battery_result& br, bool is_last) {
		bool proceed = !is_last;
		if (proceed) {
			if (const auto analysis = get_worst_statistic_analysis(br)) {
				proceed = analysis->pass();
			}
		}
		if (print_intermediate_results || !proceed) {
			print_battery_result(br);
		}
		if (!proceed && !br.results.empty()) {
			const auto test_name = get_test_name(br.results.begin()->first.type);
			result[br.test_subject_name][test_name] = extract_property(br);
		}
		return proceed;
	};
}

template <typename T>
void write(const std::string& name, const per_test_result& result) {
	std::stringstream ss;
	ss << "name|";
	for (const auto& test : get_tests<T>()) {
		ss << test.name << "|";
	}
	ss << "\n";
	for (int i = 0; i < get_tests<T>().size(); ++i) {
		ss << "-|";
	}
	ss << "-\n";

	for (const auto& e : result) {
		ss << e.first << "|";
		for (const auto& test : get_tests<T>()) {
			auto it = e.second.find(test.name);
			if (it != e.second.end()) {
				ss << it->second;
			}
			ss << "|";
		}
		ss << "\n";
	}
	write(get_config().result_dir() + name + "_per_test" + std::to_string(bit_sizeof<T>()) + ".md", ss.str());
}


template <typename T>
void inspect_per_test_command() {
	constexpr auto max_power_of_two = 25;

	uint64_t test_score = 0;
	per_test_result result;
	const auto extract_power_of_two = [&test_score](const test_battery_result& br)-> std::string {
		const auto ras = get_analysis(br);
		if (ras.empty()) {
			return "NA";
		}
		const auto worst_analysis = ras.front().analysis;
		test_score += worst_analysis.pass() ? 0 : (1 + max_power_of_two - br.power_of_two());
		const std::string result = ras.front().analysis.pass() ? ">" : "";
		return result + std::to_string(br.power_of_two());
	};
	const auto callback = create_per_test_callback(result, extract_power_of_two, false);

	for (const auto& test : get_tests<T>()) {
		std::cout << "========================\n";
		std::cout << test.name << "\n";
		std::cout << "========================\n";
		timer t;
		// trng
		if (const auto* data = get_trng_data<T>()) {
			evaluate_multi_pass(callback, create_data_test_setup<T>("trng", *data).set_tests({test.type}).range(10, std::min(max_power_of_two, 22)));
		}

		// drng
		if (const auto* data = get_drng_data<T>()) {
			evaluate_multi_pass(callback, create_data_test_setup<T>("drng", *data).set_tests({test.type}));
		}

		// mixers
		for (const auto& m : get_mixers<T>()) {
			evaluate_multi_pass(callback, create_mixer_test_setup(m).set_tests({test.type}).range(10, max_power_of_two));
		}

		// combiners
		for (const auto& combiner : get_combiners<T>()) {
			evaluate_multi_pass(callback, create_combiner_test_setup<T>(combiner).set_tests({test.type}).range(10, max_power_of_two));
		}

		// prngs
		for (const auto& prng : get_prngs<T>()) {
			evaluate_multi_pass(callback, create_prng_test_setup<T>(prng).set_tests({test.type}).range(10, max_power_of_two));
		}
		const auto test_time = t.seconds();
		result["!score"][test.name] = std::to_string(test_score);
		result["!time"][test.name] = std::to_string(test_time) + "s";
		write<T>("power_of_two", result);
		test_score = 0;
	}
}

template <typename T>
void inspect_test_speed_command() {
	per_test_result result;
	const auto extract_time = [](const test_battery_result& br) {
		return std::to_string(br.passed_milliseconds);
	};
	const auto callback = create_per_test_callback(result, extract_time, false);

	for (const auto& test : get_tests<T>()) {
		evaluate_multi_pass(callback, create_mixer_test_setup(get_default_mixer<T>())
		                              .range(10, 25)
		                              .set_tests({test.type}));
		write<T>("speed", result);
	}
}
}
