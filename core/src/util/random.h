#pragma once
#include <random>


namespace hash {

class random {
public:
	random() {
		
		_counter = rd();
	}

	explicit random(uint64_t seed) : _counter(seed) {
	}

	uint64_t operator()() { return rd(); }
private:
	uint64_t _counter;
	std::random_device rd;
};


}
