;NSIS Modern User Interface
;Welcome/Finish Page Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI.nsh"
	!include _general.nsh
;--------------------------------
;General

  ;Name and file
  Name "VSX Ultra Player 0.1.18"
  OutFile "vsxu_player_install_0118.exe"
  
  SetCompressor /SOLID lzma

  ;Default installation folder
  InstallDir "$PROGRAMFILES\vsxu_player_0.1.18"

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\vovoid\vsxu_player" ""
;--------------------------------
;Variables

  Var STARTMENU_FOLDER

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "license.txt"

  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Vovoid VSX Ultra Player" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
	
	
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
	!insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER  
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

;  !insertmacro MUI_UNPAGE_WELCOME
;  !insertmacro MUI_UNPAGE_CONFIRM
;  !insertmacro MUI_UNPAGE_INSTFILES
;  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "VSX Ultra Player" secplayer

  SetOutPath "$INSTDIR"

  ;ADD YOUR OWN FILES HERE...
  File ..\..\vsxu_player.exe
  File ..\..\vsxu_engine.dll
  File ..\..\vsxfst.dll
  File ..\..\vsxg.dll
  File ..\..\pthreadGCE.dll
  File ..\..\fmodex.dll
  File ..\..\glew_shared.dll
  File ..\..\mingwm10.dll
  
  SetOutPath "$INSTDIR\_visuals_player"
  File ..\..\_visuals_player\*.vsx

  SetOutPath "$INSTDIR\_visuals_faders"
  File ..\..\_visuals_faders\*.vsx

!include _plugins.nsh

  SetOutPath "$INSTDIR\_gfx\vsxu\font"
  File ..\..\_gfx\vsxu\font\font-ascii.png

  ;Store installation folder
  WriteRegStr HKCU "Software\vovoid\vsxu_player" "" $INSTDIR

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ; back up old value of .vsx
!define Index "Line${__LINE__}"
  ReadRegStr $1 HKCR ".vsx" ""
  StrCmp $1 "" "${Index}-NoBackup"
    StrCmp $1 "OptionsFile" "${Index}-NoBackup"
    WriteRegStr HKCR ".vsx" "backup_val" $1
"${Index}-NoBackup:"
  WriteRegStr HKCR ".vsx" "" "VSX Ultra Visualization"
  ReadRegStr $0 HKCR "VSX Ultra Visualization" ""
  StrCmp $0 "" 0 "${Index}-Skip"
	WriteRegStr HKCR "VSX Ultra Visualization" "" "Program Options File"
	WriteRegStr HKCR "VSX Ultra Visualization\shell" "" "open"
	WriteRegStr HKCR "VSX Ultra Visualization\DefaultIcon" "" "$INSTDIR\vsxu_player.exe,0"
"${Index}-Skip:"
  WriteRegStr HKCR "VSX Ultra Visualization\shell\open\command" "" \
    '$INSTDIR\vsxu_player.exe "%1"'
  WriteRegStr HKCR "VSX Ultra Visualization\shell\edit" "" "Edit Options File"
  WriteRegStr HKCR "VSX Ultra Visualization\shell\edit\command" "" \
    '$INSTDIR\vsxu_player.exe "%1"'
 
  System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)'
!undef Index  

; Start menu item
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\VSX Ultra Player.lnk" "$INSTDIR\vsxu_player.exe"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\VSX Ultra Player - enumerate displays for multimonitor mode.lnk" "$INSTDIR\vsxu_player.exe" -enum
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END


SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_secplayer ${LANG_ENGLISH} "Vsx Ultra Player 0.1.18"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${secplayer} $(DESC_secplayer)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

	Delete "$INSTDIR\vsxu_player.exe"
  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR"

  DeleteRegKey /ifempty HKCU "Software\vovoid\vsxu_player"
  
  ;start of restore script
!define Index "Line${__LINE__}"
  ReadRegStr $1 HKCR ".vsx" ""
  StrCmp $1 "VSX Ultra Visualization" 0 "${Index}-NoOwn" ; only do this if we own it
    ReadRegStr $1 HKCR ".vsx" "backup_val"
    StrCmp $1 "" 0 "${Index}-Restore" ; if backup="" then delete the whole key
      DeleteRegKey HKCR ".vsx"
    Goto "${Index}-NoOwn"
"${Index}-Restore:"
      WriteRegStr HKCR ".vsx" "" $1
      DeleteRegValue HKCR ".vsx" "backup_val"
   
    DeleteRegKey HKCR "VSX Ultra Visualization" ;Delete key with association settings
 
    System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)'
"${Index}-NoOwn:"
!undef Index  

SectionEnd
