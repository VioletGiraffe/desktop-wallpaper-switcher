RMDIR  /S /Q binaries\

call set_qt_paths.bat

set VS_TOOLS_DIR=%VS140COMNTOOLS%

pushd ..\
%QTDIR32%\bin\qmake.exe -tp vc -r
popd

call "%VS_TOOLS_DIR%VsDevCmd.bat" x86
msbuild ..\WallpaperSwitcher.sln /t:Build /p:Configuration=Release;PlatformToolset=v140

xcopy /R /Y ..\bin\release\WallpaperSwitcher.exe binaries\

SETLOCAL
SET PATH=%QTDIR32%\bin\
%QTDIR32%\bin\windeployqt.exe --dir binaries\Qt --force --no-translations --release --no-compiler-runtime --no-angle binaries\WallpaperSwitcher.exe
ENDLOCAL

del binaries\Qt\opengl*.*

xcopy /R /Y %SystemRoot%\SysWOW64\msvcp140.dll binaries\msvcr\
xcopy /R /Y %SystemRoot%\SysWOW64\vcruntime140.dll binaries\msvcr\
xcopy /R /Y %SystemRoot%\SysWOW64\vcomp140.dll binaries\msvcr\
xcopy /R /Y "%programfiles(x86)%\Windows Kits\10\Redist\ucrt\DLLs\x86\*" binaries\msvcr\

"c:\Program Files (x86)\Inno Setup 5\iscc" setup.iss