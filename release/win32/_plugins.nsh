  SetOutPath "$INSTDIR\_plugins_win32"
  File /x *ocean* ..\..\_plugins_win32\*.dll

  SetOutPath "$INSTDIR\_plugins_win32\render.glsl\blend_modes"
  File ..\..\_plugins_win32\render.glsl\blend_modes\*.glsl

  SetOutPath "$INSTDIR\_plugins_win32\render.glsl\lighting_models"
  File ..\..\_plugins_win32\render.glsl\lighting_models\*.glsl
  
  SetOutPath "$INSTDIR\_plugins_win32\render.glsl\texture_filters"
  File ..\..\_plugins_win32\render.glsl\texture_filters\*.glsl
	