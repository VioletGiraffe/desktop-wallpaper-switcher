#include "ctime.h"

#include <time.h>
#include "hlassert.h"

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

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
	timespec diff (const CTimePrivate &start) const;

	timespec m_tspec;
	bool     m_bIsValid;
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
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	m_tspec.tv_sec = mts.tv_sec;
	m_tspec.tv_nsec = mts.tv_nsec;
	m_bIsValid = true;
#else
	m_bIsValid = clock_gettime(CLOCK_MONOTONIC, &m_tspec) == 0;
#endif
	hlassert_x("Error calling clock_gettime", m_bIsValid);
}

// Сохраняет текущее время в этот экземпляр CTime
int CTimePrivate::operator -(const CTimePrivate& start) const
{
	hlassert_x("Invalid time sample", m_bIsValid);

	int msec = -1;
	if (m_bIsValid)
	{
		timespec dff = diff (start);
		msec = dff.tv_sec * 1000 + dff.tv_nsec / 1000000;
	}

	return msec;
}

timespec CTimePrivate::diff(const CTimePrivate& start) const
{
	timespec temp;
	if (m_tspec.tv_nsec - start.m_tspec.tv_nsec < 0) {
		temp.tv_sec  = m_tspec.tv_sec - start.m_tspec.tv_sec - 1;
		temp.tv_nsec = 1000000000 + m_tspec.tv_nsec - start.m_tspec.tv_nsec;
	} else {
		temp.tv_sec  = m_tspec.tv_sec - start.m_tspec.tv_sec;
		temp.tv_nsec = m_tspec.tv_nsec - start.m_tspec.tv_nsec;
	}
	return temp;
}

CTimePrivate& CTimePrivate::operator=(const CTimePrivate& other)
{
	m_tspec    = other.m_tspec;
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
