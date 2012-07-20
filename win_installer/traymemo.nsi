SetCompressor lzma
Name "TrayMemo"
OutFile "traymemo-0.73-setup.exe"
InstallDir $PROGRAMFILES\TrayMemo
Icon "..\src\images\traymemo.ico"
LicenseData "..\COPYING"
LicenseForceSelection checkbox "I agree"
RequestExecutionLevel user

# default section start
Section
 
    # call userInfo plugin to get user info.  The plugin puts the result in the stack
    userInfo::getAccountType
   
    # pop the result from the stack into $0
    pop $0
 
    # compare the result with the string "Admin" to see if the user is admin.
    # If match, jump 3 lines down.
    strCmp $0 "Admin" +3
 
    # if there is not a match, print message and return
    messageBox MB_OK "Installation requires administrator rights"
    return    
 
# default section end
SectionEnd

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
File ..\..\traymemo-build-desktop-Qt_4_8_1_for_Desktop_-_MinGW__Qt_SDK__Release\src\release\traymemo.exe
File QtGui4.dll
File QtCore4.dll
File QtSvg4.dll
File libQxtCore.a
File QxtCore.dll
File libQxtGui.a
File QxtGui.dll
File mingwm10.dll
File libgcc_s_dw2-1.dll
SetOutPath $INSTDIR\imageformats
File qsvg4.dll
SetOutPath $INSTDIR\iconengines
File qsvgicon4.dll
writeUninstaller $INSTDIR\uninstaller.exe

CreateDirectory "$SMPROGRAMS\TrayMemo"
createShortCut "$SMPROGRAMS\TrayMemo\TrayMemo.lnk" "$INSTDIR\traymemo.exe"
createShortCut "$DESKTOP\TrayMemo.lnk" "$INSTDIR\traymemo.exe"
createShortCut "$SMPROGRAMS\TrayMemo\Uninstall TrayMemo.lnk" "$INSTDIR\uninstaller.exe"
SectionEnd

Section "Uninstall"
SetShellVarContext all
delete $INSTDIR\traymemo.exe
delete $INSTDIR\QtGui4.dll
delete $INSTDIR\QtCore4.dll
delete $INSTDIR\QtSvg4.dll
delete $INSTDIR\imageformats\qsvg4.dll
delete $INSTDIR\iconengines\qsvgicon4.dll
delete $INSTDIR\libQxtCore.a
delete $INSTDIR\QxtCore.dll
delete $INSTDIR\libQxtGui.a
delete $INSTDIR\QxtGui.dll
delete $INSTDIR\mingwm10.dll
delete $INSTDIR\libgcc_s_dw2-1.dll
delete $INSTDIR\uninstaller.exe
RMDir $INSTDIR
delete "$SMPROGRAMS\TrayMemo\TrayMemo.lnk"
delete "$DESKTOP\TrayMemo.lnk"
delete "$SMPROGRAMS\TrayMemo\Uninstall TrayMemo.lnk"
RMDir "$SMPROGRAMS\TrayMemo"
DeleteRegKey HKEY_CURRENT_USER "Software\TrayMemo\TrayMemo"
SectionEnd
