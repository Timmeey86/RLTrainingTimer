#pragma once

#include <external/stduuid/uuid.h>

/** This is a wrapper around stduuid (https://github.com/mariusbancila/stduuid) which generates random uuids with a default generator. */
class uuid_generator
{
public:

	/** Generates a new UUID. */
	static std::string generateUUID();

private:
	/** Initializes the generator with a random seed. */
	static void initialize();
	static std::shared_ptr<uuids::uuid_random_generator> _generator;

};