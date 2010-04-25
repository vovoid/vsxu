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
  Name "VSX Ultra developer"
  OutFile "vsxu_developer_install_0116.exe"
  
  SetCompressor /SOLID lzma

  ;Default installation folder
  ;StrCpy $0 "$PROGRAMFILES" 3
  InstallDir "C:\vsxu_developer"
  ;StrCpy $0 "$PROGRAMFILES" 3
  ;#"$PROGRAMFILES\vsxu_developer"

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\vovoid\vsxu_developer" ""
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
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Vovoid VSX Ultra developer" 
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


Section "VSX Ultra developer" secplayer
;  SetOutPath "$INSTDIR\_gfx"
;  File /oname=vsxu_logo.jpg ..\..\_gfx\vsxu_logo_developer.jpg

	SetOutPath "$INSTDIR"
  File ..\..\vsxu_developer.exe

!include vsxu_artiste.nsh

  SetOutPath "$INSTDIR\plugin_base_pub\"
  File /r /x "*.pdom" /x "*.history*" "..\..\plugin_base_pub\*"
	
	SetOutPath "$INSTDIR\_lib"
	File "..\..\_lib\*.a"

  SetOutPath "$INSTDIR\vsx_avector\"
  File "..\..\vsx_avector\*.h"
  SetOutPath "$INSTDIR\vsx_bitmap\"
  File "..\..\vsx_bitmap\*.h"
  SetOutPath "$INSTDIR\vsx_gl\"
  File /r "..\..\vsx_gl\*.h"
  File /r "..\..\vsx_gl\*.cpp"
  SetOutPath "$INSTDIR\vsx_math\"
  File "..\..\vsx_math\*.h"
  File "..\..\vsx_math\*.cpp"
  SetOutPath "$INSTDIR\vsx_mesh\"
  File "..\..\vsx_mesh\*.h"
  SetOutPath "$INSTDIR\vsx_module_param\"
  File "..\..\vsx_module_param\*.h"
  File "..\..\vsx_module_param\*.cpp"
  SetOutPath "$INSTDIR\vsx_sequence\"
  File "..\..\vsx_sequence\*.h"
  File "..\..\vsx_sequence\*.cpp"
  SetOutPath "$INSTDIR\vsx_texture\"
  File /r "..\..\vsx_texture\*.h"
  File /r "..\..\vsx_texture\*.cpp"


  SetOutPath "$INSTDIR\_system_win32\pthread"
	File "..\..\_system_win32\pthread\*.h"

  SetOutPath "$INSTDIR\vsxfst\"
  File "..\..\vsxfst\vsxfst.h"
  SetOutPath "$INSTDIR\vsxfst\Release"
  File "..\..\vsxfst\Release\libvsxfst.a"


  ;Store installation folder
  WriteRegStr HKCU "Software\vovoid\vsxu_developer" "" $INSTDIR

  ;Create uninstaller
  ;WriteUninstaller "$INSTDIR\Uninstall.exe"

; Start menu item
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    SetOutPath "$INSTDIR\"
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\VSX Ultra developer.lnk" "$INSTDIR\vsxu_developer.exe"
    ;CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END


SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_secplayer ${LANG_ENGLISH} "Vsx Ultra developer"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${secplayer} $(DESC_secplayer)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

;Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...
;	Delete "$INSTDIR\vsxu_developer.exe"
  ;Delete "$INSTDIR\Uninstall.exe"

  ;RMDir "$INSTDIR"

  ;DeleteRegKey /ifempty HKCU "Software\vovoid\vsxu_developer"
  
  ;start of restore script
;!define Index "Line${__LINE__}"
  ;ReadRegStr $1 HKCR ".vsx" ""
  ;StrCmp $1 "VSX Ultra Visualization" 0 "${Index}-NoOwn" ; only do this if we own it
    ;ReadRegStr $1 HKCR ".vsx" "backup_val"
    ;StrCmp $1 "" 0 "${Index}-Restore" ; if backup="" then delete the whole key
      ;DeleteRegKey HKCR ".vsx"
    ;Goto "${Index}-NoOwn"
;"${Index}-Restore:"
 ;     WriteRegStr HKCR ".vsx" "" $1
      ;DeleteRegValue HKCR ".vsx" "backup_val"
   
    ;DeleteRegKey HKCR "VSX Ultra Visualization" ;Delete key with association settings
 
    ;System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)'
;"${Index}-NoOwn:"
;!undef Index  

;SectionEnd
