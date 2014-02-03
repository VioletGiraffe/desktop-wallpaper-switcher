HEADERS += \
	time/ctime.h

windows* {
	SOURCES += \
		time/ctime_windows.cpp
}
else {
	SOURCES += \
		time/ctime_unix.cpp
}
