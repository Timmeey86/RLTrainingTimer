#include <pch.h>
#include "uuid_generator.h"
#include <string>

std::shared_ptr<uuids::uuid_system_generator> uuid_generator::_generator = nullptr;

std::string uuid_generator::generateUUID()
{
	if (_generator == nullptr) { initialize(); }
	uuids::uuid const id = (*_generator)();
	return uuids::to_string(id);
}

void uuid_generator::initialize()
{
	/*std::random_device rd;
	auto seed_data = std::array<int, std::mt19937::state_size> {};
	std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
	std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
	std::mt19937 generator(seq);
	_generator = std::make_shared<uuids::uuid_random_generator>( generator );*/
	_generator = std::make_shared<uuids::uuid_system_generator>();
}
