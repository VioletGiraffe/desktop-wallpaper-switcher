RMDIR  /S /Q binaries\

call set_qt_paths.bat

pushd ..\
%QTDIR32%\bin\qmake.exe -tp vc -r
popd

call "%VS120COMNTOOLS%VsDevCmd.bat"
msbuild ..\WallpaperSwitcher.sln /t:Build /p:Configuration=Release;PlatformToolset=v120_xp

xcopy /R /Y ..\bin\WallpaperSwitcher.exe binaries\

SETLOCAL
SET PATH=%QTDIR32%\bin\
%QTDIR32%\bin\windeployqt.exe --dir binaries\Qt --force --no-translations --release --no-compiler-runtime --no-angle binaries\WallpaperSwitcher.exe
ENDLOCAL

del binaries\Qt\opengl*.*

xcopy /R /Y %SystemRoot%\SysWOW64\msvcr120.dll binaries\msvcr\
xcopy /R /Y %SystemRoot%\SysWOW64\msvcp120.dll binaries\msvcr\

"c:\Program Files (x86)\Inno Setup 5\compil32" /cc setup.iss