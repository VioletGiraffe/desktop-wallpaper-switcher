#pragma once

#include "../utility_lib.h"
#include <memory>

namespace SignalDetail {
	struct ConnectionBase;
}

class UTILITY_EXPORT Slot
{
public:
	Slot();
	~Slot();

	void destroy ();

	void operator= (std::shared_ptr<SignalDetail::ConnectionBase>& connPtr);

private:
	Slot (const Slot&) {}

private:
	std::shared_ptr<SignalDetail::ConnectionBase> _connection;
	bool                            _valid;
};
