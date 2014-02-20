#include "slot.h"
#include "signal.h"

using namespace SignalDetail;

Slot::Slot () : _valid(false)
{

}

Slot::~Slot()
{
	destroy();
}

void Slot::operator=(std::shared_ptr<ConnectionBase>& connPtr)
{
	_connection = connPtr;
	_valid      = connPtr->_valid;
}

void Slot::destroy()
{
	if (_connection != nullptr)
	{
		_connection->_valid = false;
	}
}
