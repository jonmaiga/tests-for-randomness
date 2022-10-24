#pragma once

#include <exception>
#include <string>
#include <vector>
#include "../types.h"

namespace mixer {

struct kolmogorov_stats {
    double d_max;
    std::size_t i_max;
};

struct kolmogorov_result {
    std::string stream_name;
    std::string mixer_name;
    kolmogorov_stats stats;
};

inline kolmogorov_stats kolmogorov_test(std::vector<double> data) {
    std::sort(data.begin(), data.end());
    const double n = data.size();
    double max_distance = 0;
    std::size_t max_index = 0;
    for (std::size_t i = 0; i < data.size(); ++i) {
        const double e0 = i / n;
        const double e1 = (i + 1) / n;
        const double d = data[i];
        const double distance = std::max(std::abs(e0-d), std::abs(e1-d));
        if (distance > max_distance) {
            max_distance = distance;
            max_index = i;
        }
    }
    return {max_distance, max_index};
}

inline kolmogorov_result kolmogorov_test(const stream& stream, const mixer& mixer) {
    std::vector<double> data;
    try {
        while (auto x = stream()) {
            data.push_back(mixer(x));
        }
    } catch (const std::runtime_error&) {
    }

    for (std::size_t i = 0; i < data.size(); ++i) {
        data[i] /= std::numeric_limits<uint64_t>::max();
    }
    return {stream.name, mixer.name, kolmogorov_test(data)};
}

}
