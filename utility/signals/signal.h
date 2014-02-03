#pragma once

#include <deque>
#include <memory>

#include "../utility_lib.h"
#include "slot.h"

struct ConnectionBase {
	ConnectionBase () : _valid(true) {}
	bool _valid;
};

template <typename Argument> class UTILITY_EXPORT Signal
{
	struct ConnectionPrivate : ConnectionBase {
		virtual void invoke (Argument /*arg*/) {}
	};

	template <typename Receiver>
	struct ReceiverDummy {
		typedef void (Receiver::*Method) (Argument);
	};

	template <typename Receiver>
	struct Connection : ConnectionPrivate {
	public:
		Connection ( Receiver * recv, typename ReceiverDummy<Receiver>::Method methodToConnect) :
			_instance(recv), _method (methodToConnect) {
		}

		inline virtual void invoke (Argument arg) {
			if (_valid)
				(_instance->*_method)(arg);
		}

		Receiver * _instance;
		typename ReceiverDummy<Receiver>::Method _method;
	};

public:
	template <typename Receiver>
	inline std::shared_ptr<ConnectionBase>& connect (Receiver * receiver, typename ReceiverDummy<Receiver>::Method methodToConnect) {
		//TODO: Check for multiple connections
// 			for (size_t i = 0; i < _slots.size(); ++i)
// 				if (receiver == _slots[i]->_instance && meth)
// 					return; //already connected

		auto connection = std::shared_ptr<ConnectionBase>(new Connection<Receiver>(receiver, methodToConnect));
		_slots.push_back( connection );

		return _slots.back();
	}

	inline void invoke (Argument arg) {
		// Clean dead slots up
		for (auto it = _slots.begin(); it != _slots.end();)
		{
			if (!(*it)->_valid)
				it = _slots.erase(it);
			else
				++it;
		}
		for (size_t i = 0; i < _slots.size(); ++i)
		{
			if (_slots[i]->_valid)
				static_cast<ConnectionPrivate*>(_slots[i].get())->invoke(arg);
		}
	}

private:
	std::deque<std::shared_ptr<ConnectionBase> > _slots;
};
