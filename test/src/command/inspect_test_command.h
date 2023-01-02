#pragma once

#include "evaluate.h"
#include "streams.h"

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
		return create_stream_from_mixer<T>(create_counter_stream<T>(1), get_default_mixer<T>());
	};
	return {trng()};
}	

inline auto create_test_inspect_callback(int max_power, bool print_intermediate_results = true) {
	return [max_power, print_intermediate_results](const test_battery_result& br) {
		const auto meta = get_meta_analysis(br);
		const bool proceed = meta->pass() && br.power_of_two() < max_power;
		return proceed;
	};
}

template<typename T>
struct inspect_result {
	streams<T> passed;
	streams<T> failed;
};

template<typename T>
inline inspect_result<T> inspect_test(const test_definition<T>& test_def, const streams<T>& sources) {
	const auto callback = create_test_inspect_callback(20, false);

	const mixer<T> identity_mixer = {"identity", [](T x) {
		return x;
	}};

	inspect_result<T> result;
	for (const auto& source : sources) {
		auto setup = test_setup<T>{test_def.name, {source}, {test_def.type}, identity_mixer};
		auto br = evaluate_multi_pass(callback, setup);
		auto meta = get_meta_analysis(br);
		if (meta->pass()) {
			result.passed.push_back(source);
		} else {
			result.failed.push_back(source);
		}
	}
	return result;
}

template<typename T>
void inspect_tests(const streams<T>& sources, bool all_should_pass) {
	auto rrc_sources = create_rrc_sources(sources);
	const auto get_sources = [rrc_sources, sources](const test_definition<T>& t) {
		if (t.type == test_type::mean) {
			return sources;
		}
		return rrc_sources;
	};

	for (const auto& test_def : get_tests<T>()) {
		if (all_should_pass && test_def.test_mixer) {
			continue;
		}
		auto result = inspect_test<T>(test_def, get_sources(test_def));
		const auto& fail_sources = all_should_pass ? result.failed : result.passed;
		bool passed = fail_sources.empty();
		if (passed) {
			std::cout << "PASS: " << test_def.name << "\n";
		} else {
			for (const auto& s : fail_sources) {
				std::cout << "FAIL: " << test_def.name << ": " << s.name << "\n";
			}
		}
	}	
}

template<typename T>
inline void inspect_test_command() {
	std::cout << "==============================================\n";
	std::cout << "Fail sources, " << bit_sizeof<T>() << " bits\n";
	std::cout << "==============================================\n";
	inspect_tests<T>(create_fail_sources<T>(), false);

	std::cout << "==============================================\n";
	std::cout << "Fail sources, " << bit_sizeof<T>() << " bits\n";
	std::cout << "==============================================\n";
	inspect_tests<T>(create_pass_sources<T>(), true);
}

}
