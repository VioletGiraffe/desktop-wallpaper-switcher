#pragma once

#include "../utility_lib.h"

class UTILITY_EXPORT CTime
{
public:
	CTime();
	~CTime();

	// Возвращает в милисекундах разницу во времени между двумя временными отсчётами
	int operator-(const CTime& start) const;

	// Сохраняет текущее время в этот экземпляр CTime
	void captureTime ();

	CTime& operator=(const CTime& other);

private:
	class CTimePrivate * m_time;
};
