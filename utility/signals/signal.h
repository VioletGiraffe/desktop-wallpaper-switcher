#pragma once

#include <deque>
#include <memory>

#include "../utility_lib.h"
#include "slot.h"

namespace SignalDetail
{
	struct ConnectionBase {
		ConnectionBase() : _valid(true) {}
		bool _valid;
	};

	template <typename Argument>
	struct ConnectionPrivate : ConnectionBase {
		virtual void invoke(Argument /*arg*/) {}
	};

	struct VoidConnectionPrivate : ConnectionBase {
		virtual void invoke() {}
	};

	template <typename Receiver, typename Argument>
	struct ReceiverDummy {
		typedef void (Receiver::*Method) (Argument);
	};

	template <typename Receiver>
	struct VoidReceiverDummy {
		typedef void (Receiver::*Method)();
	};

	template <typename Receiver, typename Argument>
	struct Connection : ConnectionPrivate<Argument> {
	public:
		Connection(Receiver * recv, typename ReceiverDummy<Receiver, Argument>::Method methodToConnect) :
			_instance(recv), _method(methodToConnect) {
		}

		inline virtual void invoke(Argument arg) {
			if (_valid)
				(_instance->*_method)(arg);
		}

		Receiver * _instance;
		typename ReceiverDummy<Receiver, Argument>::Method _method;
	};

	template <typename Receiver>
	struct VoidConnection : VoidConnectionPrivate {
	public:
		VoidConnection(Receiver * recv, typename VoidReceiverDummy<Receiver>::Method methodToConnect) :
			_instance(recv), _method(methodToConnect) {
		}

		inline virtual void invoke() {
			if (_valid)
				(_instance->*_method)();
		}

		Receiver * _instance;
		typename VoidReceiverDummy<Receiver>::Method _method;
	};
}



template <typename Argument> class UTILITY_EXPORT Signal1
{
public:
	template <typename Receiver>
	inline std::shared_ptr<SignalDetail::ConnectionBase>& connect (Receiver * receiver, typename SignalDetail::ReceiverDummy<Receiver, Argument>::Method methodToConnect) {
		//TODO: Check for multiple connections
// 			for (size_t i = 0; i < _slots.size(); ++i)
// 				if (receiver == _slots[i]->_instance && meth)
// 					return; //already connected

		auto connection = std::shared_ptr<SignalDetail::ConnectionBase>(new SignalDetail::Connection<Receiver, Argument>(receiver, methodToConnect));
		_slots.push_back( connection );

		return _slots.back();
	}

	void invoke (Argument arg) {
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
				static_cast<SignalDetail::ConnectionPrivate<Argument>*>(_slots[i].get())->invoke(arg);
		}
	}

private:
	std::deque<std::shared_ptr<SignalDetail::ConnectionBase> > _slots;
};

class UTILITY_EXPORT Signal0
{
public:
	template <typename Receiver>
	inline std::shared_ptr<SignalDetail::ConnectionBase>& connect(Receiver * receiver, typename SignalDetail::VoidReceiverDummy<Receiver>::Method methodToConnect) {
		//TODO: Check for multiple connections
		// 			for (size_t i = 0; i < _slots.size(); ++i)
		// 				if (receiver == _slots[i]->_instance && meth)
		// 					return; //already connected

		auto connection = std::shared_ptr<SignalDetail::ConnectionBase>(new SignalDetail::VoidConnection<Receiver>(receiver, methodToConnect));
		_slots.push_back(connection);

		return _slots.back();
	}

	void invoke() {
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
				static_cast<SignalDetail::VoidConnectionPrivate*>(_slots[i].get())->invoke();
		}
	}

private:
	std::deque<std::shared_ptr<SignalDetail::ConnectionBase> > _slots;
};

