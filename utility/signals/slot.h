#pragma once

#include "../utility_lib.h"
#include <memory>

struct ConnectionBase;

class UTILITY_EXPORT Slot
{
public:
	Slot();
	~Slot();

	void destroy ();

	void operator= (std::shared_ptr<ConnectionBase>& connPtr);

private:
	Slot (const Slot&) {}

private:
	std::shared_ptr<ConnectionBase> _connection;
	bool                            _valid;
};
