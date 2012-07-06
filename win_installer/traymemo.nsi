SetCompressor lzma
Name "TrayMemo"
OutFile "traymemo-0.5-setup.exe"
InstallDir $PROGRAMFILES\TrayMemo
Icon "..\src\images\traymemo.ico"
LicenseData "..\COPYING"
LicenseForceSelection checkbox "I agree"
RequestExecutionLevel user

Function .onInit
System::Call 'kernel32::CreateMutexA(i 0, i 0, t "mutex-traymemo-installer") i .r1 ?e'
Pop $R0
 
StrCmp $R0 0 +3
  MessageBox MB_OK|MB_ICONEXCLAMATION "The installer is already running."
  Abort
FunctionEnd

Page license
Page directory
Page instfiles

Section ""
SetOutPath $INSTDIR
SetShellVarContext all
File ..\bin\traymemo.exe
File QtCore4.dll
File QtGui4.dll
File QxtGui.dll
File QxtCore.dll
writeUninstaller $INSTDIR\uninstaller.exe

CreateDirectory "$SMPROGRAMS\TrayMemo"
createShortCut "$SMPROGRAMS\TrayMemo\TrayMemo.lnk" "$INSTDIR\traymemo.exe"
createShortCut "$DESKTOP\TrayMemo.lnk" "$INSTDIR\traymemo.exe"
createShortCut "$SMPROGRAMS\TrayMemo\Uninstall TrayMemo.lnk" "$INSTDIR\uninstaller.exe"
SectionEnd



section "Uninstall"
SetShellVarContext all
delete $INSTDIR\traymemo.exe
delete $INSTDIR\QtCore4.dll
delete $INSTDIR\QtGui4.dll
delete $INSTDIR\QxtGui.dll
delete $INSTDIR\QxtCore.dll
delete $INSTDIR\uninstaller.exe
RMDir $INSTDIR
delete "$SMPROGRAMS\TrayMemo\TrayMemo.lnk"
delete "$DESKTOP\TrayMemo.lnk"
delete "$SMPROGRAMS\TrayMemo\Uninstall TrayMemo.lnk"
RMDir "$SMPROGRAMS\TrayMemo"
DeleteRegKey HKEY_CURRENT_USER "Software\TrayMemo\TrayMemo"
sectionEnd
