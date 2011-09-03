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
  Name "VSX Ultra Artiste 0.3.0"
  OutFile "vsxu_artiste_install_030.exe"
  
  SetCompressor /SOLID lzma

  ;Default installation folder
  InstallDir "$PROGRAMFILES\vsxu_artiste_0.3.0"

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\vovoid\vsxu_artiste_030" ""
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
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Vovoid VSX Ultra Artiste 0.3.0" 
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

Section "VSX Ultra Artiste" secplayer

  SetOutPath "$INSTDIR\_gfx"
  File /oname=vsxu_logo.jpg ..\..\share\gfx\vsxu_logo.jpg

  SetOutPath "$INSTDIR"
  File ..\..\artiste\vsxu_artiste.exe
  File ..\..\player\vsxu_player.exe
  !include _vsxu_artiste.nsh


  ;Store installation folder
  WriteRegStr HKCU "Software\vovoid\vsxu_artiste" "" $INSTDIR

  ;Create uninstaller
  ;WriteUninstaller "$INSTDIR\Uninstall.exe"

; Start menu item
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
  SetOutPath "$INSTDIR\"
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\VSX Ultra Artiste.lnk" "$INSTDIR\vsxu_artiste.exe"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\VSX Ultra Artiste - enumerate displays for multimonitor mode.lnk" "$INSTDIR\vsxu_artiste.exe" -enum
    ;CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END


SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_secplayer ${LANG_ENGLISH} "Vsx Ultra Artiste"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${secplayer} $(DESC_secplayer)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

;Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...
	;Delete "$INSTDIR\vsxu_artiste.exe"
  ;Delete "$INSTDIR\Uninstall.exe"

  ;RMDir "$INSTDIR"

  ;DeleteRegKey /ifempty HKCU "Software\vovoid\vsxu_artiste"
  
  ;start of restore script
;!define Index "Line${__LINE__}"
 ; ReadRegStr $1 HKCR ".vsx" ""
  ;StrCmp $1 "VSX Ultra Visualization" 0 "${Index}-NoOwn" ; only do this if we own it
   ; ReadRegStr $1 HKCR ".vsx" "backup_val"
;    StrCmp $1 "" 0 "${Index}-Restore" ; if backup="" then delete the whole key
 ;     DeleteRegKey HKCR ".vsx"
  ;  Goto "${Index}-NoOwn"
;"${Index}-Restore:"
 ;     WriteRegStr HKCR ".vsx" "" $1
  ;    DeleteRegValue HKCR ".vsx" "backup_val"
   
    ;DeleteRegKey HKCR "VSX Ultra Visualization" ;Delete key with association settings
 
    ;System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)'
;"${Index}-NoOwn:"
;!undef Index  

;SectionEnd
