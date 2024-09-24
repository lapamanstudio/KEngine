[Setup]
AppName=KEngine
AppVersion=0.0.2
DefaultDirName={pf}\KEngine
OutputDir=.\out
OutputBaseFilename=KEngineInstaller
SetupIconFile=..\icon.ico

[Files]
Source: "..\out\KEngine.exe"; DestDir: "{app}"
Source: ".\bins\*.dll"; DestDir: "{app}\bins"
Source: "..\resources\*"; DestDir: "{app}\datafiles"; Flags: recursesubdirs
Source: "..\icon.ico"; DestDir: "{app}"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 6.1

[Icons]
Name: "{group}\KEngine"; Filename: "{app}\KEngine.exe"
Name: "{commondesktop}\KEngine"; Filename: "{app}\KEngine.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\KEngine"; Filename: "{app}\KEngine.exe"; Tasks: quicklaunchicon
