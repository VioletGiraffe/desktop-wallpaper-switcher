rem set QTDIR=c:\Development\Qt\4.8.5\

RMDIR  /S /Q binaries\

xcopy /R /Y ..\bin\WallpaperSwitcher.exe binaries\

xcopy /R /Y %QTDIR%\bin\Qt5Core.dll binaries\Qt\
xcopy /R /Y %QTDIR%\bin\Qt5Gui.dll binaries\Qt\
xcopy /R /Y %QTDIR%\bin\Qt5Widgets.dll binaries\Qt\

xcopy /R /Y %QTDIR%\bin\icudt52.dll binaries\Qt\
xcopy /R /Y %QTDIR%\bin\icuin52.dll binaries\Qt\
xcopy /R /Y %QTDIR%\bin\icuuc52.dll binaries\Qt\

xcopy /R /Y %QTDIR%\plugins\imageformats\qgif.dll binaries\Qt\imageformats\
xcopy /R /Y %QTDIR%\plugins\imageformats\qico.dll binaries\Qt\imageformats\
xcopy /R /Y %QTDIR%\plugins\imageformats\qjpeg.dll binaries\Qt\imageformats\
xcopy /R /Y %QTDIR%\plugins\imageformats\qtiff.dll binaries\Qt\imageformats\

xcopy /R /Y %QTDIR%\plugins\platforms\qwindows.dll binaries\Qt\platforms\

xcopy /R /Y %SystemRoot%\SysWOW64\msvcr110.dll binaries\msvcr\
xcopy /R /Y %SystemRoot%\SysWOW64\msvcp110.dll binaries\msvcr\
xcopy /R /Y %SystemRoot%\SysWOW64\vcomp110.dll binaries\msvcr\

"c:\Program Files (x86)\Inno Setup 5\compil32" /cc setup.iss