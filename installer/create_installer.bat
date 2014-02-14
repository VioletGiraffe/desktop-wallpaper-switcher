rem set QTDIR=c:\Development\Qt\4.8.5\

xcopy /R /Y ..\bin\WallpaperSwitcher.exe binaries\

xcopy /R /Y %QTDIR%\bin\phonon4.dll binaries\Qt\
xcopy /R /Y %QTDIR%\bin\QtCore4.dll binaries\Qt\
xcopy /R /Y %QTDIR%\bin\QtGui4.dll binaries\Qt\

xcopy /R /Y %QTDIR%\plugins\imageformats\qgif4.dll binaries\Qt\imageformats\
xcopy /R /Y %QTDIR%\plugins\imageformats\qico4.dll binaries\Qt\imageformats\
xcopy /R /Y %QTDIR%\plugins\imageformats\qjpeg4.dll binaries\Qt\imageformats\
xcopy /R /Y %QTDIR%\plugins\imageformats\qmng4.dll binaries\Qt\imageformats\
xcopy /R /Y %QTDIR%\plugins\imageformats\qsvg4.dll binaries\Qt\imageformats\
xcopy /R /Y %QTDIR%\plugins\imageformats\qtga4.dll binaries\Qt\imageformats\
xcopy /R /Y %QTDIR%\plugins\imageformats\qtiff4.dll binaries\Qt\imageformats\

xcopy /R /Y %SystemRoot%\SysWOW64\msvcr100.dll binaries\msvcr\
xcopy /R /Y %SystemRoot%\SysWOW64\msvcp100.dll binaries\msvcr\
xcopy /R /Y %SystemRoot%\SysWOW64\vcomp100.dll binaries\msvcr\

"c:\Program Files (x86)\Inno Setup 5\compil32" /cc setup.iss