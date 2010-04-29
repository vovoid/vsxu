  SetOutPath "$INSTDIR\plugins"
  File /r /x lib ..\..\plugins\*.dll

  SetOutPath "$INSTDIR\plugin-config\render.glsl"
  File /r ..\..\plugins\src\render.glsl\share\plugin-config\render.glsl\*.*
	