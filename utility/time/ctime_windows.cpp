#include "ctime.h"
#include <assert.h>

#include <Windows.h>

class CTimePrivate
{
public:
	CTimePrivate ();
	// Сохраняет текущее время в этот экземпляр CTime
	void captureTime ();
	// Возвращает в милисекундах разницу во времени между двумя временными отсчётами
	int operator-(const CTimePrivate& start) const;

	CTimePrivate& operator=(const CTimePrivate& other);

private:
	LARGE_INTEGER m_time;
	LARGE_INTEGER m_freq;
	bool          m_bIsValid;
};


//////////////////////////////////////////////////////
//                  CTimePrivate
//////////////////////////////////////////////////////

CTimePrivate::CTimePrivate () : m_bIsValid(false)
{
}

// Возвращает в милисекундах разницу во времени между двумя временными отсчётами
void CTimePrivate::captureTime()
{
	m_bIsValid = false;
	BOOL succ = QueryPerformanceFrequency(&m_freq);
	if (!succ)
	{
		assert("Error calling QueryPerformanceFrequency" == 0);
		return;
	}

	succ = QueryPerformanceCounter(&m_time);
	if (!succ)
	{
		assert("Error calling QueryPerformanceCounter" == 0);
		return;
	}

	m_bIsValid = true;
}

// Сохраняет текущее время в этот экземпляр CTime
int CTimePrivate::operator -(const CTimePrivate& start) const
{
	assert(m_bIsValid);
	assert(start.m_bIsValid);

	if (m_freq.QuadPart != start.m_freq.QuadPart)
	{
		assert("Samples were taken at different frequencies" == 0);
		return -1;
	}

	int msec = -1;
	if (m_bIsValid && start.m_bIsValid)
	{
		msec = static_cast<int>( (m_time.QuadPart - start.m_time.QuadPart) * 1000 / m_freq.QuadPart );
	}

	return msec;
}

CTimePrivate& CTimePrivate::operator=(const CTimePrivate& other)
{
	m_freq     = other.m_freq;
	m_time     = other.m_time;
	m_bIsValid = other.m_bIsValid;
	return *this;
}

/////////////////////////////////////////////////////
//                    CTime
/////////////////////////////////////////////////////

CTime::CTime()
{
	m_time = new CTimePrivate;
	captureTime();
}

CTime::~CTime()
{
	if (m_time)
		delete m_time;
}

// Возвращает в милисекундах разницу во времени между двумя временными отсчётами
int CTime::operator-(const CTime& start) const
{
	return *m_time - *start.m_time;
}

// Сохраняет текущее время в этот экземпляр CTime
void CTime::captureTime ()
{
	m_time->captureTime();
}

CTime& CTime::operator=(const CTime& other)
{
	*m_time = *other.m_time;
	return *this;
}
