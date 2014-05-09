#define AppVerStr GetFileVersion("binaries/WallpaperSwitcher.exe")

[Setup]
AppVerName=Wallpaper Switcher {#AppVerStr}
AppName=Wallpaper Switcher
AppId=WallpaperSwitcher
DefaultDirName={pf}\Wallpaper Switcher
DefaultGroupName=Wallpaper Switcher
AllowNoIcons=true
;LicenseFile=license.rtf
PrivilegesRequired=admin
OutputDir=.
OutputBaseFilename=WallpaperSwitcher

;SetupIconFile=icon.ico
UninstallDisplayIcon={app}\WallpaperSwitcher.exe

AppCopyright=VioletGiraffe
WizardImageBackColor=clWhite
;WizardImageFile=setup\large_logo.bmp
;WizardSmallImageFile=setup\small_logo.bmp
ShowTasksTreeLines=yes

;ArchitecturesInstallIn64BitMode=x64

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

;License
Source: license.rtf; DestDir: {app}; 

[Icons]
Name: {group}\Wallpaper Switcher; Filename: {app}\WallpaperSwitcher.exe;
Name: {group}\{cm:UninstallProgram,Wallpaper Switcher}; Filename: {uninstallexe}
Name: {userstartup}\Wallpaper Switcher; Filename: {app}\WallpaperSwitcher.exe; Tasks: startup

Name: {userdesktop}\Wallpaper Switcher; Filename: {app}\WallpaperSwitcher.exe; Tasks: desktopicon;

[Run]
Filename: {app}\WallpaperSwitcher.exe; Description: {cm:LaunchProgram,Wallpaper Switcher}; Flags: nowait postinstall skipifsilent

[UninstallRun]

[Languages]
Name: English; MessagesFile: compiler:Default.isl
Name: German; MessagesFile: compiler:Languages\German.isl
Name: Russian; MessagesFile: compiler:Languages\Russian.isl
Name: BrazilianPortuguese; MessagesFile: compiler:Languages\BrazilianPortuguese.isl
Name: Catalan; MessagesFile: compiler:Languages\Catalan.isl
Name: Czech; MessagesFile: compiler:Languages\Czech.isl
Name: Danish; MessagesFile: compiler:Languages\Danish.isl
Name: Dutch; MessagesFile: compiler:Languages\Dutch.isl
Name: Finnish; MessagesFile: compiler:Languages\Finnish.isl
Name: French; MessagesFile: compiler:Languages\French.isl
Name: Hebrew; MessagesFile: compiler:Languages\Hebrew.isl
Name: Hungarian; MessagesFile: compiler:Languages\Hungarian.isl
Name: Italian; MessagesFile: compiler:Languages\Italian.isl
Name: Japanese; MessagesFile: compiler:Languages\Japanese.isl
Name: Norwegian; MessagesFile: compiler:Languages\Norwegian.isl
Name: Polish; MessagesFile: compiler:Languages\Polish.isl
Name: Portuguese; MessagesFile: compiler:Languages\Portuguese.isl
Name: Slovenian; MessagesFile: compiler:Languages\Slovenian.isl
Name: Spanish; MessagesFile: compiler:Languages\Spanish.isl

[Registry]

[Code]
function GetUninstallString(): String;
var
  sUnInstPath: String;
  sUnInstallString: String;
begin
  sUnInstPath := ExpandConstant('Software\Microsoft\Windows\CurrentVersion\Uninstall\{#emit SetupSetting("AppId")}_is1');
  sUnInstallString := '';
  if not RegQueryStringValue(HKLM, sUnInstPath, 'UninstallString', sUnInstallString) then
    RegQueryStringValue(HKCU, sUnInstPath, 'UninstallString', sUnInstallString);
  Result := sUnInstallString;
end;

function IsUpgrade(): Boolean;
begin
  Result := (GetUninstallString() <> '');
end;

function UnInstallOldVersion(): Integer;
var
  sUnInstallString: String;
  iResultCode: Integer;
begin
// Return Values:
// 1 - uninstall string is empty
// 2 - error executing the UnInstallString
// 3 - successfully executed the UnInstallString

  // default return value
  Result := 0;

  // get the uninstall string of the old app
  sUnInstallString := GetUninstallString();
  if sUnInstallString <> '' then begin
    sUnInstallString := RemoveQuotes(sUnInstallString);
    if Exec(sUnInstallString, '/SILENT /NORESTART /SUPPRESSMSGBOXES','', SW_HIDE, ewWaitUntilTerminated, iResultCode) then
      Result := 3
    else
      Result := 2;
  end else
    Result := 1;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if (CurStep=ssInstall) then
  begin
    if (IsUpgrade()) then
    begin
      UnInstallOldVersion();
    end;
  end;
end;