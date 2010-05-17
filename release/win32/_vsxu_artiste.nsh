  SetOutPath "$INSTDIR"
SetOverwrite off  
  File ..\..\share\vsxu.conf
SetOverwrite on

  File ..\..\engine\libvsxu_engine.dll
  File ..\..\engine_graphics\libvsxu_engine_graphics.dll
  File ..\..\lib\win32\pthreadGCE.dll
  File ..\..\lib\win32\fmodex.dll
  File ..\..\lib\win32\glew_shared.dll
  File ..\..\lib\win32\mingwm10.dll
  File ..\..\lib\win32\zlib1.dll
  File ..\..\lib\win32\freetype6.dll
  
 
 ; VSXU GRAPHICS
  SetOutPath "$INSTDIR\doc"
  File ..\..\share\doc\course_intro.txt
  SetOutPath "$INSTDIR\gfx"
  File ..\..\share\gfx\luna.png
  File ..\..\share\gfx\vsxu_logo.jpg
  File ..\..\share\gfx\vsxu_logo_bkg.jpg
  SetOutPath "$INSTDIR\font"
  File ..\..\share\font\*.png

; 'luna' skin
  SetOutPath "$INSTDIR\gfx\vsxu_luna"
  File ..\..\share\gfx\vsxu_luna\*.conf
  File ..\..\share\gfx\vsxu_luna\*.png
  File ..\..\share\gfx\vsxu_luna\*.jpg
  SetOutPath "$INSTDIR\gfx\vsxu_luna\component_types"
  File ..\..\share\gfx\vsxu_luna\component_types\*.png
  SetOutPath "$INSTDIR\gfx\vsxu_luna\controllers"
  File ..\..\share\gfx\vsxu_luna\controllers\*.png
  SetOutPath "$INSTDIR\gfx\vsxu_luna\datatypes"
  File ..\..\share\gfx\vsxu_luna\datatypes\*.png
  SetOutPath "$INSTDIR\gfx\vsxu_luna\interface_extras"
  File ..\..\share\gfx\vsxu_luna\interface_extras\*.png
	
	
; example files
  SetOutPath "$INSTDIR\data\macros\examples\"
  File /r ..\..\share\example-macros\*.*
  SetOutPath "$INSTDIR\data\prods\examples\"
  File /r ..\..\share\example-prods\*.*
  SetOutPath "$INSTDIR\data\resources\examples\"
  File /r ..\..\share\example-resources\*.*
  SetOutPath "$INSTDIR\data\states\examples\"
  File /r ..\..\share\example-states\*.*
  SetOutPath "$INSTDIR\data\states\debug\"
  File /r ..\..\share\debug-states\*.*
  SetOutPath "$INSTDIR\data\visuals\examples\"
  File /r ..\..\share\example-visuals\*.*
  SetOutPath "$INSTDIR\data\visuals-faders\examples\"
  File /r ..\..\share\example-faders\*.*
  SetOutPath "$INSTDIR\data\screenshots\examples\"
  File /r ..\..\share\screenshots\0.2.0\*.*

; player files
  SetOutPath "$INSTDIR\visuals_player"
  File ..\..\share\visuals_player\*.vsx
  SetOutPath "$INSTDIR\visuals_faders"
  File ..\..\share\visuals_faders\*.vsx

!include _plugins.nsh
