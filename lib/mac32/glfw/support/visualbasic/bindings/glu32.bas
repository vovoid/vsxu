Attribute VB_Name = "GLU"

'*************************************************************************
'* GLU 1.3 (glu32.dll) bindings for Visual Basic
'* Copyright (c) 2002 Camilla Berglund
'*
'* This file was converted from the MinGW32 GL/glu.h include file (using a
'* custom C parser / Visual Basic writer), which in turn is based on the
'* SGI OpenGL sample implementation glu.h include file.
'*************************************************************************


'*************************************************************************
'* This bindings file is for GLU 1.3
'*************************************************************************

Public Const GLU_VERSION_1_1 = 1
Public Const GLU_VERSION_1_2 = 1
Public Const GLU_VERSION_1_3 = 1



'*************************************************************************
'* Constants
'*************************************************************************

' Extensions
Public Const GLU_EXT_object_space_tess = 1
Public Const GLU_EXT_nurbs_tessellator = 1

' Boolean
Public Const GLU_FALSE = 0
Public Const GLU_TRUE = 1

' StringName
Public Const GLU_VERSION = 100800
Public Const GLU_EXTENSIONS = 100801

' ErrorCode
Public Const GLU_INVALID_ENUM = 100900
Public Const GLU_INVALID_VALUE = 100901
Public Const GLU_OUT_OF_MEMORY = 100902
Public Const GLU_INVALID_OPERATION = 100904

' NurbsDisplay
' Public Const GLU_FILL = 100012
Public Const GLU_OUTLINE_POLYGON = 100240
Public Const GLU_OUTLINE_PATCH = 100241

' NurbsCallback
Public Const GLU_NURBS_ERROR = 100103
Public Const GLU_ERROR = 100103
Public Const GLU_NURBS_BEGIN = 100164
Public Const GLU_NURBS_BEGIN_EXT = 100164
Public Const GLU_NURBS_VERTEX = 100165
Public Const GLU_NURBS_VERTEX_EXT = 100165
Public Const GLU_NURBS_NORMAL = 100166
Public Const GLU_NURBS_NORMAL_EXT = 100166
Public Const GLU_NURBS_COLOR = 100167
Public Const GLU_NURBS_COLOR_EXT = 100167
Public Const GLU_NURBS_TEXTURE_COORD = 100168
Public Const GLU_NURBS_TEX_COORD_EXT = 100168
Public Const GLU_NURBS_END = 100169
Public Const GLU_NURBS_END_EXT = 100169
Public Const GLU_NURBS_BEGIN_DATA = 100170
Public Const GLU_NURBS_BEGIN_DATA_EXT = 100170
Public Const GLU_NURBS_VERTEX_DATA = 100171
Public Const GLU_NURBS_VERTEX_DATA_EXT = 100171
Public Const GLU_NURBS_NORMAL_DATA = 100172
Public Const GLU_NURBS_NORMAL_DATA_EXT = 100172
Public Const GLU_NURBS_COLOR_DATA = 100173
Public Const GLU_NURBS_COLOR_DATA_EXT = 100173
Public Const GLU_NURBS_TEXTURE_COORD_DATA = 100174
Public Const GLU_NURBS_TEX_COORD_DATA_EXT = 100174
Public Const GLU_NURBS_END_DATA = 100175
Public Const GLU_NURBS_END_DATA_EXT = 100175

' NurbsError
Public Const GLU_NURBS_ERROR1 = 100251
Public Const GLU_NURBS_ERROR2 = 100252
Public Const GLU_NURBS_ERROR3 = 100253
Public Const GLU_NURBS_ERROR4 = 100254
Public Const GLU_NURBS_ERROR5 = 100255
Public Const GLU_NURBS_ERROR6 = 100256
Public Const GLU_NURBS_ERROR7 = 100257
Public Const GLU_NURBS_ERROR8 = 100258
Public Const GLU_NURBS_ERROR9 = 100259
Public Const GLU_NURBS_ERROR10 = 100260
Public Const GLU_NURBS_ERROR11 = 100261
Public Const GLU_NURBS_ERROR12 = 100262
Public Const GLU_NURBS_ERROR13 = 100263
Public Const GLU_NURBS_ERROR14 = 100264
Public Const GLU_NURBS_ERROR15 = 100265
Public Const GLU_NURBS_ERROR16 = 100266
Public Const GLU_NURBS_ERROR17 = 100267
Public Const GLU_NURBS_ERROR18 = 100268
Public Const GLU_NURBS_ERROR19 = 100269
Public Const GLU_NURBS_ERROR20 = 100270
Public Const GLU_NURBS_ERROR21 = 100271
Public Const GLU_NURBS_ERROR22 = 100272
Public Const GLU_NURBS_ERROR23 = 100273
Public Const GLU_NURBS_ERROR24 = 100274
Public Const GLU_NURBS_ERROR25 = 100275
Public Const GLU_NURBS_ERROR26 = 100276
Public Const GLU_NURBS_ERROR27 = 100277
Public Const GLU_NURBS_ERROR28 = 100278
Public Const GLU_NURBS_ERROR29 = 100279
Public Const GLU_NURBS_ERROR30 = 100280
Public Const GLU_NURBS_ERROR31 = 100281
Public Const GLU_NURBS_ERROR32 = 100282
Public Const GLU_NURBS_ERROR33 = 100283
Public Const GLU_NURBS_ERROR34 = 100284
Public Const GLU_NURBS_ERROR35 = 100285
Public Const GLU_NURBS_ERROR36 = 100286
Public Const GLU_NURBS_ERROR37 = 100287

' NurbsProperty
Public Const GLU_AUTO_LOAD_MATRIX = 100200
Public Const GLU_CULLING = 100201
Public Const GLU_SAMPLING_TOLERANCE = 100203
Public Const GLU_DISPLAY_MODE = 100204
Public Const GLU_PARAMETRIC_TOLERANCE = 100202
Public Const GLU_SAMPLING_METHOD = 100205
Public Const GLU_U_STEP = 100206
Public Const GLU_V_STEP = 100207
Public Const GLU_NURBS_MODE = 100160
Public Const GLU_NURBS_MODE_EXT = 100160
Public Const GLU_NURBS_TESSELLATOR = 100161
Public Const GLU_NURBS_TESSELLATOR_EXT = 100161
Public Const GLU_NURBS_RENDERER = 100162
Public Const GLU_NURBS_RENDERER_EXT = 100162

' NurbsSampling
Public Const GLU_OBJECT_PARAMETRIC_ERROR = 100208
Public Const GLU_OBJECT_PARAMETRIC_ERROR_EXT = 100208
Public Const GLU_OBJECT_PATH_LENGTH = 100209
Public Const GLU_OBJECT_PATH_LENGTH_EXT = 100209
Public Const GLU_PATH_LENGTH = 100215
Public Const GLU_PARAMETRIC_ERROR = 100216
Public Const GLU_DOMAIN_DISTANCE = 100217

' NurbsTrim
Public Const GLU_MAP1_TRIM_2 = 100210
Public Const GLU_MAP1_TRIM_3 = 100211

' QuadricDrawStyle
Public Const GLU_POINT = 100010
Public Const GLU_LINE = 100011
Public Const GLU_FILL = 100012
Public Const GLU_SILHOUETTE = 100013

' QuadricCallback
' Public Const GLU_ERROR = 100103

' QuadricNormal
Public Const GLU_SMOOTH = 100000
Public Const GLU_FLAT = 100001
Public Const GLU_NONE = 100002

' QuadricOrientation
Public Const GLU_OUTSIDE = 100020
Public Const GLU_INSIDE = 100021

' TessCallback
Public Const GLU_TESS_BEGIN = 100100
Public Const GLU_BEGIN = 100100
Public Const GLU_TESS_VERTEX = 100101
Public Const GLU_VERTEX = 100101
Public Const GLU_TESS_END = 100102
Public Const GLU_END = 100102
Public Const GLU_TESS_ERROR = 100103
Public Const GLU_TESS_EDGE_FLAG = 100104
Public Const GLU_EDGE_FLAG = 100104
Public Const GLU_TESS_COMBINE = 100105
Public Const GLU_TESS_BEGIN_DATA = 100106
Public Const GLU_TESS_VERTEX_DATA = 100107
Public Const GLU_TESS_END_DATA = 100108
Public Const GLU_TESS_ERROR_DATA = 100109
Public Const GLU_TESS_EDGE_FLAG_DATA = 100110
Public Const GLU_TESS_COMBINE_DATA = 100111

' TessContour
Public Const GLU_CW = 100120
Public Const GLU_CCW = 100121
Public Const GLU_INTERIOR = 100122
Public Const GLU_EXTERIOR = 100123
Public Const GLU_UNKNOWN = 100124

' TessProperty
Public Const GLU_TESS_WINDING_RULE = 100140
Public Const GLU_TESS_BOUNDARY_ONLY = 100141
Public Const GLU_TESS_TOLERANCE = 100142

' TessError
Public Const GLU_TESS_ERROR1 = 100151
Public Const GLU_TESS_ERROR2 = 100152
Public Const GLU_TESS_ERROR3 = 100153
Public Const GLU_TESS_ERROR4 = 100154
Public Const GLU_TESS_ERROR5 = 100155
Public Const GLU_TESS_ERROR6 = 100156
Public Const GLU_TESS_ERROR7 = 100157
Public Const GLU_TESS_ERROR8 = 100158
Public Const GLU_TESS_MISSING_BEGIN_POLYGON = 100151
Public Const GLU_TESS_MISSING_BEGIN_CONTOUR = 100152
Public Const GLU_TESS_MISSING_END_POLYGON = 100153
Public Const GLU_TESS_MISSING_END_CONTOUR = 100154
Public Const GLU_TESS_COORD_TOO_LARGE = 100155
Public Const GLU_TESS_NEED_COMBINE_CALLBACK = 100156

' TessWinding
Public Const GLU_TESS_WINDING_ODD = 100130
Public Const GLU_TESS_WINDING_NONZERO = 100131
Public Const GLU_TESS_WINDING_POSITIVE = 100132
Public Const GLU_TESS_WINDING_NEGATIVE = 100133
Public Const GLU_TESS_WINDING_ABS_GEQ_TWO = 100134

' Tesselation (misc)
Public Const GLU_TESS_MAX_COORD = 1E+150



'*************************************************************************
'* Function Definitions
'*************************************************************************

Public Declare Sub gluBeginCurve Lib "glu32.dll" (nurb As Any)
Public Declare Sub gluBeginPolygon Lib "glu32.dll" (tess As Any)
Public Declare Sub gluBeginSurface Lib "glu32.dll" (nurb As Any)
Public Declare Sub gluBeginTrim Lib "glu32.dll" (nurb As Any)
Public Declare Function gluBuild1DMipmapLevels Lib "glu32.dll" (ByVal target As Long, ByVal internalFormat As Long, ByVal width As Long, ByVal format As Long, ByVal typ As Long, ByVal level As Long, ByVal base As Long, ByVal max As Long, data As Any) As Long
Public Declare Function gluBuild1DMipmaps Lib "glu32.dll" (ByVal target As Long, ByVal internalFormat As Long, ByVal width As Long, ByVal format As Long, ByVal typ As Long, data As Any) As Long
Public Declare Function gluBuild2DMipmapLevels Lib "glu32.dll" (ByVal target As Long, ByVal internalFormat As Long, ByVal width As Long, ByVal height As Long, ByVal format As Long, ByVal typ As Long, ByVal level As Long, ByVal base As Long, ByVal max As Long, data As Any) As Long
Public Declare Function gluBuild2DMipmaps Lib "glu32.dll" (ByVal target As Long, ByVal internalFormat As Long, ByVal width As Long, ByVal height As Long, ByVal format As Long, ByVal typ As Long, data As Any) As Long
Public Declare Function gluBuild3DMipmapLevels Lib "glu32.dll" (ByVal target As Long, ByVal internalFormat As Long, ByVal width As Long, ByVal height As Long, ByVal depth As Long, ByVal format As Long, ByVal typ As Long, ByVal level As Long, ByVal base As Long, ByVal max As Long, data As Any) As Long
Public Declare Function gluBuild3DMipmaps Lib "glu32.dll" (ByVal target As Long, ByVal internalFormat As Long, ByVal width As Long, ByVal height As Long, ByVal depth As Long, ByVal format As Long, ByVal typ As Long, data As Any) As Long
Public Declare Function gluCheckExtension Lib "glu32.dll" (extName As Byte, extString As Byte) As Byte
Public Declare Sub gluCylinder Lib "glu32.dll" (quad As Any, ByVal base As Double, ByVal top As Double, ByVal height As Double, ByVal slices As Long, ByVal stacks As Long)
Public Declare Sub gluDeleteNurbsRenderer Lib "glu32.dll" (nurb As Any)
Public Declare Sub gluDeleteQuadric Lib "glu32.dll" (quad As Any)
Public Declare Sub gluDeleteTess Lib "glu32.dll" (tess As Any)
Public Declare Sub gluDisk Lib "glu32.dll" (quad As Any, ByVal inner As Double, ByVal outer As Double, ByVal slices As Long, ByVal loops As Long)
Public Declare Sub gluEndCurve Lib "glu32.dll" (nurb As Any)
Public Declare Sub gluEndPolygon Lib "glu32.dll" (tess As Any)
Public Declare Sub gluEndSurface Lib "glu32.dll" (nurb As Any)
Public Declare Sub gluEndTrim Lib "glu32.dll" (nurb As Any)
Public Declare Function gluErrorString Lib "glu32.dll" (ByVal error As Long) As String
Public Declare Sub gluGetNurbsProperty Lib "glu32.dll" (nurb As Any, ByVal property As Long, data As Single)
Public Declare Function gluGetString Lib "glu32.dll" (ByVal name As Long) As String
Public Declare Sub gluGetTessProperty Lib "glu32.dll" (tess As Any, ByVal which As Long, data As Double)
Public Declare Sub gluLoadSamplingMatrices Lib "glu32.dll" (nurb As Any, model As Single, perspective As Single, view As Long)
Public Declare Sub gluLookAt Lib "glu32.dll" (ByVal eyeX As Double, ByVal eyeY As Double, ByVal eyeZ As Double, ByVal centerX As Double, ByVal centerY As Double, ByVal centerZ As Double, ByVal upX As Double, ByVal upY As Double, ByVal upZ As Double)
Public Declare Function gluNewNurbsRenderer Lib "glu32.dll" () As Long
Public Declare Function gluNewQuadric Lib "glu32.dll" () As Long
Public Declare Function gluNewTess Lib "glu32.dll" () As Long
Public Declare Sub gluNextContour Lib "glu32.dll" (tess As Any, ByVal typ As Long)
Public Declare Sub gluNurbsCallback Lib "glu32.dll" (nurb As Any, ByVal which As Long, ByVal CallBackFunc As Any)
Public Declare Sub gluNurbsCallbackData Lib "glu32.dll" (nurb As Any, userData As Any)
Public Declare Sub gluNurbsCallbackDataEXT Lib "glu32.dll" (nurb As Any, userData As Any)
Public Declare Sub gluNurbsCurve Lib "glu32.dll" (nurb As Any, ByVal knotCount As Long, knots As Single, ByVal stride As Long, control As Single, ByVal order As Long, ByVal typ As Long)
Public Declare Sub gluNurbsProperty Lib "glu32.dll" (nurb As Any, ByVal property As Long, ByVal value As Single)
Public Declare Sub gluNurbsSurface Lib "glu32.dll" (nurb As Any, ByVal sKnotCount As Long, sKnots As Single, ByVal tKnotCount As Long, tKnots As Single, ByVal sStride As Long, ByVal tStride As Long, control As Single, ByVal sOrder As Long, ByVal tOrder As Long, ByVal typ As Long)
Public Declare Sub gluOrtho2D Lib "glu32.dll" (ByVal left As Double, ByVal right As Double, ByVal bottom As Double, ByVal top As Double)
Public Declare Sub gluPartialDisk Lib "glu32.dll" (quad As Any, ByVal inner As Double, ByVal outer As Double, ByVal slices As Long, ByVal loops As Long, ByVal start As Double, ByVal sweep As Double)
Public Declare Sub gluPerspective Lib "glu32.dll" (ByVal fovy As Double, ByVal aspect As Double, ByVal zNear As Double, ByVal zFar As Double)
Public Declare Sub gluPickMatrix Lib "glu32.dll" (ByVal x As Double, ByVal y As Double, ByVal delX As Double, ByVal delY As Double, viewport As Long)
Public Declare Function gluProject Lib "glu32.dll" (ByVal objX As Double, ByVal objY As Double, ByVal objZ As Double, model As Double, proj As Double, view As Long, winX As Double, winY As Double, winZ As Double) As Long
Public Declare Sub gluPwlCurve Lib "glu32.dll" (nurb As Any, ByVal count As Long, data As Single, ByVal stride As Long, ByVal typ As Long)
Public Declare Sub gluQuadricCallback Lib "glu32.dll" (quad As Any, ByVal which As Long, ByVal CallBackFunc As Any)
Public Declare Sub gluQuadricDrawStyle Lib "glu32.dll" (quad As Any, ByVal draw As Long)
Public Declare Sub gluQuadricNormals Lib "glu32.dll" (quad As Any, ByVal normal As Long)
Public Declare Sub gluQuadricOrientation Lib "glu32.dll" (quad As Any, ByVal orientation As Long)
Public Declare Sub gluQuadricTexture Lib "glu32.dll" (quad As Any, ByVal texture As Byte)
Public Declare Function gluScaleImage Lib "glu32.dll" (ByVal format As Long, ByVal wIn As Long, ByVal hIn As Long, ByVal typeIn As Long, dataIn As Any, ByVal wOut As Long, ByVal hOut As Long, ByVal typeOut As Long, dataOut As Any) As Long
Public Declare Sub gluSphere Lib "glu32.dll" (quad As Any, ByVal radius As Double, ByVal slices As Long, ByVal stacks As Long)
Public Declare Sub gluTessBeginContour Lib "glu32.dll" (tess As Any)
Public Declare Sub gluTessBeginPolygon Lib "glu32.dll" (tess As Any, data As Any)
Public Declare Sub gluTessCallback Lib "glu32.dll" (tess As Any, ByVal which As Long, ByVal CallBackFunc As Any)
Public Declare Sub gluTessEndContour Lib "glu32.dll" (tess As Any)
Public Declare Sub gluTessEndPolygon Lib "glu32.dll" (tess As Any)
Public Declare Sub gluTessNormal Lib "glu32.dll" (tess As Any, ByVal valueX As Double, ByVal valueY As Double, ByVal valueZ As Double)
Public Declare Sub gluTessProperty Lib "glu32.dll" (tess As Any, ByVal which As Long, ByVal data As Double)
Public Declare Sub gluTessVertex Lib "glu32.dll" (tess As Any, location As Double, data As Any)
Public Declare Function gluUnProject Lib "glu32.dll" (ByVal winX As Double, ByVal winY As Double, ByVal winZ As Double, model As Double, proj As Double, view As Long, objX As Double, objY As Double, objZ As Double) As Long
Public Declare Function gluUnProject4 Lib "glu32.dll" (ByVal winX As Double, ByVal winY As Double, ByVal winZ As Double, ByVal clipW As Double, model As Double, proj As Double, view As Long, ByVal nearVal As Double, ByVal farVal As Double, objX As Double, objY As Double, objZ As Double, objW As Double) As Long
