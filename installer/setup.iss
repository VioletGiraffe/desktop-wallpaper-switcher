#define AppVerStr GetFileVersion("binaries/WallpaperSwitcher.exe")

[Setup]
AppVerName=Wallpaper Switcher {#AppVerStr}
AppName=Wallpaper Switcher
AppId=WallpaperSwitcher
DefaultDirName={pf}\Wallpaper Switcher
DefaultGroupName=Wallpaper Switcher
AllowNoIcons=true
OutputDir=.
OutputBaseFilename=WallpaperSwitcher

;SetupIconFile=icon.ico
UninstallDisplayIcon={app}\WallpaperSwitcher.exe

AppCopyright=VioletGiraffe
WizardImageBackColor=clWhite
ShowTasksTreeLines=yes

SolidCompression=true
Compression=lzma2/ultra64
LZMANumBlockThreads=4
LZMAUseSeparateProcess=yes
LZMABlockSize=8192

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: startup; Description: "Automatically run the program when Windows starts"; GroupDescription: "{cm:AdditionalIcons}";

[Files]

;Main binaries
Source: binaries/WallpaperSwitcher.exe; DestDir: {app}; Flags: ignoreversion;

;3rdparty binaries

;Qt binaries
Source: binaries/Qt/*; DestDir: {app}; Flags: ignoreversion;

;Qt plugins 
Source: binaries/Qt/imageformats\*; DestDir: {app}\imageformats; Flags: ignoreversion;
Source: binaries/Qt/platforms\*; DestDir: {app}\platforms; Flags: ignoreversion skipifsourcedoesntexist;

;MSVC binaries
Source: binaries/msvcr/*; DestDir: {app}; Flags: ignoreversion;

[Icons]
Name: {group}\Wallpaper Switcher; Filename: {app}\WallpaperSwitcher.exe;
Name: {group}\{cm:UninstallProgram,Wallpaper Switcher}; Filename: {uninstallexe}
Name: {userstartup}\Wallpaper Switcher; Filename: {app}\WallpaperSwitcher.exe; Tasks: startup

Name: {userdesktop}\Wallpaper Switcher; Filename: {app}\WallpaperSwitcher.exe; Tasks: desktopicon;

[Run]
Filename: {app}\WallpaperSwitcher.exe; Description: {cm:LaunchProgram,Wallpaper Switcher}; Flags: nowait postinstall skipifsilent