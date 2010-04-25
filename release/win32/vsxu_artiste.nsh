  SetOutPath "$INSTDIR"
SetOverwrite off  
  File ..\..\_doc\vsxu.conf
SetOverwrite on

  File ..\..\vsxu_engine.dll
  File ..\..\vsxfst.dll
  File ..\..\vsxg.dll
  File ..\..\pthreadGCE.dll
  File ..\..\fmodex.dll
  File ..\..\glew_shared.dll
  File ..\..\mingwm10.dll
  
 
 ; VSXU GRAPHICS
  SetOutPath "$INSTDIR\_doc"
  File ..\..\_doc\course_intro.txt
  SetOutPath "$INSTDIR\_gfx"
  File ..\..\_gfx\luna.png
  File ..\..\_gfx\vsxu_logo_artiste.jpg
  File ..\..\_gfx\vsxu_logo_bkg.jpg
	
;  File ..\..\_gfx\vsxu_logo_bkg.jpg
;  SetOutPath "$INSTDIR\_gfx\vsxu_luna\font"
;  File ..\..\_gfx\vsxu_luna\font\*.png
;  SetOutPath "$INSTDIR\_gfx\vsxu_luna"
;  File ..\..\_gfx\vsxu_luna\*.conf
;  File ..\..\_gfx\vsxu_luna\*.png
;  File ..\..\_gfx\vsxu_luna\*.jpg
;  SetOutPath "$INSTDIR\_gfx\vsxu_luna\component_types"
;  File ..\..\_gfx\vsxu_luna\component_types\*.png
;  SetOutPath "$INSTDIR\_gfx\vsxu_luna\controllers"
;  File ..\..\_gfx\vsxu_luna\controllers\*.png
;  SetOutPath "$INSTDIR\_gfx\vsxu_luna\datatypes"
;  File ..\..\_gfx\vsxu_luna\datatypes\*.png
;  SetOutPath "$INSTDIR\_gfx\vsxu_luna\interface_extras"
;  File ..\..\_gfx\vsxu_luna\interface_extras\*.png

; regular skin
  SetOutPath "$INSTDIR\_gfx\vsxu\font"
  File ..\..\_gfx\vsxu\font\*.png
  SetOutPath "$INSTDIR\_gfx\vsxu"
  File ..\..\_gfx\vsxu\*.conf
  File ..\..\_gfx\vsxu\*.png
  File ..\..\_gfx\vsxu\*.jpg
  SetOutPath "$INSTDIR\_gfx\vsxu\component_types"
  File ..\..\_gfx\vsxu\component_types\*.png
  SetOutPath "$INSTDIR\_gfx\vsxu\controllers"
  File ..\..\_gfx\vsxu\controllers\*.png
  SetOutPath "$INSTDIR\_gfx\vsxu\datatypes"
  File ..\..\_gfx\vsxu\datatypes\*.png
  SetOutPath "$INSTDIR\_gfx\vsxu\interface_extras"
  File ..\..\_gfx\vsxu\interface_extras\*.png
	
	
; macros
  SetOutPath "$INSTDIR\_macros\maths"
  File ..\..\_macros\maths\colors\hue_sine
  File ..\..\_macros\maths\xy_blob_sine_render
  File ..\..\_macros\maths\xy_position_oscillation

; states
  SetOutPath "$INSTDIR\_states"
  File _default
  SetOutPath "$INSTDIR\_states\visuals"
  File ..\..\_states\visuals\*
  SetOutPath "$INSTDIR\_states\examples"
  File ..\..\_states\examples\*



  SetOutPath "$INSTDIR\_visuals"
  File ..\..\_visuals\*.vsx

  SetOutPath "$INSTDIR\_visuals_faders"
  File ..\..\_visuals_faders\*.vsx

  SetOutPath "$INSTDIR\_prods"
  File ..\..\_prods\glowtext.vsx
  File ..\..\_prods\dolphin.vsx
  File ..\..\_prods\clock.vsx

!include _plugins.nsh

  SetOutPath "$INSTDIR\resources\fonts"
  File /nonfatal /r ..\..\resources\fonts\font-arial.ttf

  SetOutPath "$INSTDIR\resources\images"
  File /nonfatal /r ..\..\resources\images\*.jpg
  File /nonfatal /r ..\..\resources\images\*.png

  SetOutPath "$INSTDIR\resources\vsa"
  File /r ..\..\resources\vsa\*.jpg
  File /r ..\..\resources\vsa\*.png

  SetOutPath "$INSTDIR\resources\meshes"
  File /nonfatal /r ..\..\resources\meshes\*.obj
