[Setup]
AppId={{4de4a96a-a39d-47a4-b863-859b5ecd0943}
AppName=SolarMonitor
AppVersion=1.0.0
AppVerName=SolarMonitor v1.0.0
AppPublisher=marcocipriani01
AppPublisherURL=https://marcocipriani01.github.io/
AppSupportURL=https://marcocipriani01.github.io/
AppUpdatesURL=https://marcocipriani01.github.io/
DefaultDirName={autopf}\SolarMonitor
DisableDirPage=yes
DefaultGroupName=ThunderFocus
DisableProgramGroupPage=yes
DisableReadyPage=yes
LicenseFile=".\SolarMonitor-source\LICENSE.txt"
Compression=lzma
SolidCompression=yes
WizardStyle=modern
OutputBaseFilename=SolarMonitor_Win64bit
OutputDir=./
SetupIconFile=".\SolarMonitor-source\SolarMonitor.ico"
UninstallDisplayIcon=".\SolarMonitor-source\SolarMonitor.ico"

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Dirs]
Name: "{app}"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: ".\SolarMonitor\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\SolarMonitor-source\SolarMonitor.ico"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\SolarMonitor"; Filename: "{app}\SolarMonitor.exe"; IconFilename: "{app}\SolarMonitor.ico"
Name: "{autodesktop}\SolarMonitor"; Filename: "{app}\SolarMonitor.exe"; Tasks: desktopicon; IconFilename: "{app}\SolarMonitor.ico"
