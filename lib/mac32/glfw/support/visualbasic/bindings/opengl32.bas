Attribute VB_Name = "OpenGL"

'*************************************************************************
'* OpenGL 1.1 (opengl32.dll) bindings for Visual Basic
'* Copyright (c) 2002 Camilla Berglund
'*
'* This file was converted from the MinGW32 GL/gl.h include file (using a
'* custom C parser / Visual Basic writer), which in turn is based on the
'* Mesa3D 4.0 gl.h include file by Brian Paul.
'*************************************************************************


'*************************************************************************
'* This bindings file is for OpenGL 1.1
'*************************************************************************

Public Const GL_VERSION_1_1 = 1



'*************************************************************************
'* Constants
'*************************************************************************

' Boolean values
Public Const GL_FALSE = 0
Public Const GL_TRUE = 1

' Data types
Public Const GL_BYTE = &H1400
Public Const GL_UNSIGNED_BYTE = &H1401
Public Const GL_SHORT = &H1402
Public Const GL_UNSIGNED_SHORT = &H1403
Public Const GL_INT = &H1404
Public Const GL_UNSIGNED_INT = &H1405
Public Const GL_FLOAT = &H1406
Public Const GL_DOUBLE = &H140A
Public Const GL_2_BYTES = &H1407
Public Const GL_3_BYTES = &H1408
Public Const GL_4_BYTES = &H1409

' Primitives
Public Const GL_POINTS = &H0
Public Const GL_LINES = &H1
Public Const GL_LINE_LOOP = &H2
Public Const GL_LINE_STRIP = &H3
Public Const GL_TRIANGLES = &H4
Public Const GL_TRIANGLE_STRIP = &H5
Public Const GL_TRIANGLE_FAN = &H6
Public Const GL_QUADS = &H7
Public Const GL_QUAD_STRIP = &H8
Public Const GL_POLYGON = &H9

' Vertex Arrays
Public Const GL_VERTEX_ARRAY = &H8074
Public Const GL_NORMAL_ARRAY = &H8075
Public Const GL_COLOR_ARRAY = &H8076
Public Const GL_INDEX_ARRAY = &H8077
Public Const GL_TEXTURE_COORD_ARRAY = &H8078
Public Const GL_EDGE_FLAG_ARRAY = &H8079
Public Const GL_VERTEX_ARRAY_SIZE = &H807A
Public Const GL_VERTEX_ARRAY_TYPE = &H807B
Public Const GL_VERTEX_ARRAY_STRIDE = &H807C
Public Const GL_NORMAL_ARRAY_TYPE = &H807E
Public Const GL_NORMAL_ARRAY_STRIDE = &H807F
Public Const GL_COLOR_ARRAY_SIZE = &H8081
Public Const GL_COLOR_ARRAY_TYPE = &H8082
Public Const GL_COLOR_ARRAY_STRIDE = &H8083
Public Const GL_INDEX_ARRAY_TYPE = &H8085
Public Const GL_INDEX_ARRAY_STRIDE = &H8086
Public Const GL_TEXTURE_COORD_ARRAY_SIZE = &H8088
Public Const GL_TEXTURE_COORD_ARRAY_TYPE = &H8089
Public Const GL_TEXTURE_COORD_ARRAY_STRIDE = &H808A
Public Const GL_EDGE_FLAG_ARRAY_STRIDE = &H808C
Public Const GL_VERTEX_ARRAY_POINTER = &H808E
Public Const GL_NORMAL_ARRAY_POINTER = &H808F
Public Const GL_COLOR_ARRAY_POINTER = &H8090
Public Const GL_INDEX_ARRAY_POINTER = &H8091
Public Const GL_TEXTURE_COORD_ARRAY_POINTER = &H8092
Public Const GL_EDGE_FLAG_ARRAY_POINTER = &H8093
Public Const GL_V2F = &H2A20
Public Const GL_V3F = &H2A21
Public Const GL_C4UB_V2F = &H2A22
Public Const GL_C4UB_V3F = &H2A23
Public Const GL_C3F_V3F = &H2A24
Public Const GL_N3F_V3F = &H2A25
Public Const GL_C4F_N3F_V3F = &H2A26
Public Const GL_T2F_V3F = &H2A27
Public Const GL_T4F_V4F = &H2A28
Public Const GL_T2F_C4UB_V3F = &H2A29
Public Const GL_T2F_C3F_V3F = &H2A2A
Public Const GL_T2F_N3F_V3F = &H2A2B
Public Const GL_T2F_C4F_N3F_V3F = &H2A2C
Public Const GL_T4F_C4F_N3F_V4F = &H2A2D

' Matrix Mode
Public Const GL_MATRIX_MODE = &HBA0
Public Const GL_MODELVIEW = &H1700
Public Const GL_PROJECTION = &H1701
Public Const GL_TEXTURE = &H1702

' Points
Public Const GL_POINT_SMOOTH = &HB10
Public Const GL_POINT_SIZE = &HB11
Public Const GL_POINT_SIZE_GRANULARITY = &HB13
Public Const GL_POINT_SIZE_RANGE = &HB12

' Lines
Public Const GL_LINE_SMOOTH = &HB20
Public Const GL_LINE_STIPPLE = &HB24
Public Const GL_LINE_STIPPLE_PATTERN = &HB25
Public Const GL_LINE_STIPPLE_REPEAT = &HB26
Public Const GL_LINE_WIDTH = &HB21
Public Const GL_LINE_WIDTH_GRANULARITY = &HB23
Public Const GL_LINE_WIDTH_RANGE = &HB22

' Polygons
Public Const GL_POINT = &H1B00
Public Const GL_LINE = &H1B01
Public Const GL_FILL = &H1B02
Public Const GL_CW = &H900
Public Const GL_CCW = &H901
Public Const GL_FRONT = &H404
Public Const GL_BACK = &H405
Public Const GL_POLYGON_MODE = &HB40
Public Const GL_POLYGON_SMOOTH = &HB41
Public Const GL_POLYGON_STIPPLE = &HB42
Public Const GL_EDGE_FLAG = &HB43
Public Const GL_CULL_FACE = &HB44
Public Const GL_CULL_FACE_MODE = &HB45
Public Const GL_FRONT_FACE = &HB46
Public Const GL_POLYGON_OFFSET_FACTOR = &H8038
Public Const GL_POLYGON_OFFSET_UNITS = &H2A00
Public Const GL_POLYGON_OFFSET_POINT = &H2A01
Public Const GL_POLYGON_OFFSET_LINE = &H2A02
Public Const GL_POLYGON_OFFSET_FILL = &H8037

' Display Lists
Public Const GL_COMPILE = &H1300
Public Const GL_COMPILE_AND_EXECUTE = &H1301
Public Const GL_LIST_BASE = &HB32
Public Const GL_LIST_INDEX = &HB33
Public Const GL_LIST_MODE = &HB30

' Depth buffer
Public Const GL_NEVER = &H200
Public Const GL_LESS = &H201
Public Const GL_EQUAL = &H202
Public Const GL_LEQUAL = &H203
Public Const GL_GREATER = &H204
Public Const GL_NOTEQUAL = &H205
Public Const GL_GEQUAL = &H206
Public Const GL_ALWAYS = &H207
Public Const GL_DEPTH_TEST = &HB71
Public Const GL_DEPTH_BITS = &HD56
Public Const GL_DEPTH_CLEAR_VALUE = &HB73
Public Const GL_DEPTH_FUNC = &HB74
Public Const GL_DEPTH_RANGE = &HB70
Public Const GL_DEPTH_WRITEMASK = &HB72
Public Const GL_DEPTH_COMPONENT = &H1902

' Lighting
Public Const GL_LIGHTING = &HB50
Public Const GL_LIGHT0 = &H4000
Public Const GL_LIGHT1 = &H4001
Public Const GL_LIGHT2 = &H4002
Public Const GL_LIGHT3 = &H4003
Public Const GL_LIGHT4 = &H4004
Public Const GL_LIGHT5 = &H4005
Public Const GL_LIGHT6 = &H4006
Public Const GL_LIGHT7 = &H4007
Public Const GL_SPOT_EXPONENT = &H1205
Public Const GL_SPOT_CUTOFF = &H1206
Public Const GL_CONSTANT_ATTENUATION = &H1207
Public Const GL_LINEAR_ATTENUATION = &H1208
Public Const GL_QUADRATIC_ATTENUATION = &H1209
Public Const GL_AMBIENT = &H1200
Public Const GL_DIFFUSE = &H1201
Public Const GL_SPECULAR = &H1202
Public Const GL_SHININESS = &H1601
Public Const GL_EMISSION = &H1600
Public Const GL_POSITION = &H1203
Public Const GL_SPOT_DIRECTION = &H1204
Public Const GL_AMBIENT_AND_DIFFUSE = &H1602
Public Const GL_COLOR_INDEXES = &H1603
Public Const GL_LIGHT_MODEL_TWO_SIDE = &HB52
Public Const GL_LIGHT_MODEL_LOCAL_VIEWER = &HB51
Public Const GL_LIGHT_MODEL_AMBIENT = &HB53
Public Const GL_FRONT_AND_BACK = &H408
Public Const GL_SHADE_MODEL = &HB54
Public Const GL_FLAT = &H1D00
Public Const GL_SMOOTH = &H1D01
Public Const GL_COLOR_MATERIAL = &HB57
Public Const GL_COLOR_MATERIAL_FACE = &HB55
Public Const GL_COLOR_MATERIAL_PARAMETER = &HB56
Public Const GL_NORMALIZE = &HBA1

' User clipping planes
Public Const GL_CLIP_PLANE0 = &H3000
Public Const GL_CLIP_PLANE1 = &H3001
Public Const GL_CLIP_PLANE2 = &H3002
Public Const GL_CLIP_PLANE3 = &H3003
Public Const GL_CLIP_PLANE4 = &H3004
Public Const GL_CLIP_PLANE5 = &H3005

' Accumulation buffer
Public Const GL_ACCUM_RED_BITS = &HD58
Public Const GL_ACCUM_GREEN_BITS = &HD59
Public Const GL_ACCUM_BLUE_BITS = &HD5A
Public Const GL_ACCUM_ALPHA_BITS = &HD5B
Public Const GL_ACCUM_CLEAR_VALUE = &HB80
Public Const GL_ACCUM = &H100
Public Const GL_ADD = &H104
Public Const GL_LOAD = &H101
Public Const GL_MULT = &H103
Public Const GL_RETURN = &H102

' Alpha testing
Public Const GL_ALPHA_TEST = &HBC0
Public Const GL_ALPHA_TEST_REF = &HBC2
Public Const GL_ALPHA_TEST_FUNC = &HBC1

' Blending
Public Const GL_BLEND = &HBE2
Public Const GL_BLEND_SRC = &HBE1
Public Const GL_BLEND_DST = &HBE0
Public Const GL_ZERO = &H0
Public Const GL_ONE = &H1
Public Const GL_SRC_COLOR = &H300
Public Const GL_ONE_MINUS_SRC_COLOR = &H301
Public Const GL_SRC_ALPHA = &H302
Public Const GL_ONE_MINUS_SRC_ALPHA = &H303
Public Const GL_DST_ALPHA = &H304
Public Const GL_ONE_MINUS_DST_ALPHA = &H305
Public Const GL_DST_COLOR = &H306
Public Const GL_ONE_MINUS_DST_COLOR = &H307
Public Const GL_SRC_ALPHA_SATURATE = &H308
Public Const GL_CONSTANT_COLOR = &H8001
Public Const GL_ONE_MINUS_CONSTANT_COLOR = &H8002
Public Const GL_CONSTANT_ALPHA = &H8003
Public Const GL_ONE_MINUS_CONSTANT_ALPHA = &H8004

' Render Mode
Public Const GL_FEEDBACK = &H1C01
Public Const GL_RENDER = &H1C00
Public Const GL_SELECT = &H1C02

' Feedback
Public Const GL_2D = &H600
Public Const GL_3D = &H601
Public Const GL_3D_COLOR = &H602
Public Const GL_3D_COLOR_TEXTURE = &H603
Public Const GL_4D_COLOR_TEXTURE = &H604
Public Const GL_POINT_TOKEN = &H701
Public Const GL_LINE_TOKEN = &H702
Public Const GL_LINE_RESET_TOKEN = &H707
Public Const GL_POLYGON_TOKEN = &H703
Public Const GL_BITMAP_TOKEN = &H704
Public Const GL_DRAW_PIXEL_TOKEN = &H705
Public Const GL_COPY_PIXEL_TOKEN = &H706
Public Const GL_PASS_THROUGH_TOKEN = &H700
Public Const GL_FEEDBACK_BUFFER_POINTER = &HDF0
Public Const GL_FEEDBACK_BUFFER_SIZE = &HDF1
Public Const GL_FEEDBACK_BUFFER_TYPE = &HDF2

' Selection
Public Const GL_SELECTION_BUFFER_POINTER = &HDF3
Public Const GL_SELECTION_BUFFER_SIZE = &HDF4

' Fog
Public Const GL_FOG = &HB60
Public Const GL_FOG_MODE = &HB65
Public Const GL_FOG_DENSITY = &HB62
Public Const GL_FOG_COLOR = &HB66
Public Const GL_FOG_INDEX = &HB61
Public Const GL_FOG_START = &HB63
Public Const GL_FOG_END = &HB64
Public Const GL_LINEAR = &H2601
Public Const GL_EXP = &H800
Public Const GL_EXP2 = &H801

' Logic Ops
Public Const GL_LOGIC_OP = &HBF1
Public Const GL_INDEX_LOGIC_OP = &HBF1
Public Const GL_COLOR_LOGIC_OP = &HBF2
Public Const GL_LOGIC_OP_MODE = &HBF0
Public Const GL_CLEAR = &H1500
Public Const GL_SET = &H150F
Public Const GL_COPY = &H1503
Public Const GL_COPY_INVERTED = &H150C
Public Const GL_NOOP = &H1505
Public Const GL_INVERT = &H150A
Public Const GL_AND = &H1501
Public Const GL_NAND = &H150E
Public Const GL_OR = &H1507
Public Const GL_NOR = &H1508
Public Const GL_XOR = &H1506
Public Const GL_EQUIV = &H1509
Public Const GL_AND_REVERSE = &H1502
Public Const GL_AND_INVERTED = &H1504
Public Const GL_OR_REVERSE = &H150B
Public Const GL_OR_INVERTED = &H150D

' Stencil
Public Const GL_STENCIL_TEST = &HB90
Public Const GL_STENCIL_WRITEMASK = &HB98
Public Const GL_STENCIL_BITS = &HD57
Public Const GL_STENCIL_FUNC = &HB92
Public Const GL_STENCIL_VALUE_MASK = &HB93
Public Const GL_STENCIL_REF = &HB97
Public Const GL_STENCIL_FAIL = &HB94
Public Const GL_STENCIL_PASS_DEPTH_PASS = &HB96
Public Const GL_STENCIL_PASS_DEPTH_FAIL = &HB95
Public Const GL_STENCIL_CLEAR_VALUE = &HB91
Public Const GL_STENCIL_INDEX = &H1901
Public Const GL_KEEP = &H1E00
Public Const GL_REPLACE = &H1E01
Public Const GL_INCR = &H1E02
Public Const GL_DECR = &H1E03

' Buffers, Pixel Drawing/Reading
Public Const GL_NONE = &H0
Public Const GL_LEFT = &H406
Public Const GL_RIGHT = &H407
' Public Const GL_FRONT = &H0404
' Public Const GL_BACK = &H0405
' Public Const GL_FRONT_AND_BACK = &H0408
Public Const GL_FRONT_LEFT = &H400
Public Const GL_FRONT_RIGHT = &H401
Public Const GL_BACK_LEFT = &H402
Public Const GL_BACK_RIGHT = &H403
Public Const GL_AUX0 = &H409
Public Const GL_AUX1 = &H40A
Public Const GL_AUX2 = &H40B
Public Const GL_AUX3 = &H40C
Public Const GL_COLOR_INDEX = &H1900
Public Const GL_RED = &H1903
Public Const GL_GREEN = &H1904
Public Const GL_BLUE = &H1905
Public Const GL_ALPHA = &H1906
Public Const GL_LUMINANCE = &H1909
Public Const GL_LUMINANCE_ALPHA = &H190A
Public Const GL_ALPHA_BITS = &HD55
Public Const GL_RED_BITS = &HD52
Public Const GL_GREEN_BITS = &HD53
Public Const GL_BLUE_BITS = &HD54
Public Const GL_INDEX_BITS = &HD51
Public Const GL_SUBPIXEL_BITS = &HD50
Public Const GL_AUX_BUFFERS = &HC00
Public Const GL_READ_BUFFER = &HC02
Public Const GL_DRAW_BUFFER = &HC01
Public Const GL_DOUBLEBUFFER = &HC32
Public Const GL_STEREO = &HC33
Public Const GL_BITMAP = &H1A00
Public Const GL_COLOR = &H1800
Public Const GL_DEPTH = &H1801
Public Const GL_STENCIL = &H1802
Public Const GL_DITHER = &HBD0
Public Const GL_RGB = &H1907
Public Const GL_RGBA = &H1908

' Implementation limits
Public Const GL_MAX_LIST_NESTING = &HB31
Public Const GL_MAX_ATTRIB_STACK_DEPTH = &HD35
Public Const GL_MAX_MODELVIEW_STACK_DEPTH = &HD36
Public Const GL_MAX_NAME_STACK_DEPTH = &HD37
Public Const GL_MAX_PROJECTION_STACK_DEPTH = &HD38
Public Const GL_MAX_TEXTURE_STACK_DEPTH = &HD39
Public Const GL_MAX_EVAL_ORDER = &HD30
Public Const GL_MAX_LIGHTS = &HD31
Public Const GL_MAX_CLIP_PLANES = &HD32
Public Const GL_MAX_TEXTURE_SIZE = &HD33
Public Const GL_MAX_PIXEL_MAP_TABLE = &HD34
Public Const GL_MAX_VIEWPORT_DIMS = &HD3A
Public Const GL_MAX_CLIENT_ATTRIB_STACK_DEPTH = &HD3B

' Gets
Public Const GL_ATTRIB_STACK_DEPTH = &HBB0
Public Const GL_CLIENT_ATTRIB_STACK_DEPTH = &HBB1
Public Const GL_COLOR_CLEAR_VALUE = &HC22
Public Const GL_COLOR_WRITEMASK = &HC23
Public Const GL_CURRENT_INDEX = &HB01
Public Const GL_CURRENT_COLOR = &HB00
Public Const GL_CURRENT_NORMAL = &HB02
Public Const GL_CURRENT_RASTER_COLOR = &HB04
Public Const GL_CURRENT_RASTER_DISTANCE = &HB09
Public Const GL_CURRENT_RASTER_INDEX = &HB05
Public Const GL_CURRENT_RASTER_POSITION = &HB07
Public Const GL_CURRENT_RASTER_TEXTURE_COORDS = &HB06
Public Const GL_CURRENT_RASTER_POSITION_VALID = &HB08
Public Const GL_CURRENT_TEXTURE_COORDS = &HB03
Public Const GL_INDEX_CLEAR_VALUE = &HC20
Public Const GL_INDEX_MODE = &HC30
Public Const GL_INDEX_WRITEMASK = &HC21
Public Const GL_MODELVIEW_MATRIX = &HBA6
Public Const GL_MODELVIEW_STACK_DEPTH = &HBA3
Public Const GL_NAME_STACK_DEPTH = &HD70
Public Const GL_PROJECTION_MATRIX = &HBA7
Public Const GL_PROJECTION_STACK_DEPTH = &HBA4
Public Const GL_RENDER_MODE = &HC40
Public Const GL_RGBA_MODE = &HC31
Public Const GL_TEXTURE_MATRIX = &HBA8
Public Const GL_TEXTURE_STACK_DEPTH = &HBA5
Public Const GL_VIEWPORT = &HBA2

' Evaluators
Public Const GL_AUTO_NORMAL = &HD80
Public Const GL_MAP1_COLOR_4 = &HD90
Public Const GL_MAP1_GRID_DOMAIN = &HDD0
Public Const GL_MAP1_GRID_SEGMENTS = &HDD1
Public Const GL_MAP1_INDEX = &HD91
Public Const GL_MAP1_NORMAL = &HD92
Public Const GL_MAP1_TEXTURE_COORD_1 = &HD93
Public Const GL_MAP1_TEXTURE_COORD_2 = &HD94
Public Const GL_MAP1_TEXTURE_COORD_3 = &HD95
Public Const GL_MAP1_TEXTURE_COORD_4 = &HD96
Public Const GL_MAP1_VERTEX_3 = &HD97
Public Const GL_MAP1_VERTEX_4 = &HD98
Public Const GL_MAP2_COLOR_4 = &HDB0
Public Const GL_MAP2_GRID_DOMAIN = &HDD2
Public Const GL_MAP2_GRID_SEGMENTS = &HDD3
Public Const GL_MAP2_INDEX = &HDB1
Public Const GL_MAP2_NORMAL = &HDB2
Public Const GL_MAP2_TEXTURE_COORD_1 = &HDB3
Public Const GL_MAP2_TEXTURE_COORD_2 = &HDB4
Public Const GL_MAP2_TEXTURE_COORD_3 = &HDB5
Public Const GL_MAP2_TEXTURE_COORD_4 = &HDB6
Public Const GL_MAP2_VERTEX_3 = &HDB7
Public Const GL_MAP2_VERTEX_4 = &HDB8
Public Const GL_COEFF = &HA00
Public Const GL_DOMAIN = &HA02
Public Const GL_ORDER = &HA01

' Hints
Public Const GL_FOG_HINT = &HC54
Public Const GL_LINE_SMOOTH_HINT = &HC52
Public Const GL_PERSPECTIVE_CORRECTION_HINT = &HC50
Public Const GL_POINT_SMOOTH_HINT = &HC51
Public Const GL_POLYGON_SMOOTH_HINT = &HC53
Public Const GL_DONT_CARE = &H1100
Public Const GL_FASTEST = &H1101
Public Const GL_NICEST = &H1102

' Scissor box
Public Const GL_SCISSOR_TEST = &HC11
Public Const GL_SCISSOR_BOX = &HC10

' Pixel Mode / Transfer
Public Const GL_MAP_COLOR = &HD10
Public Const GL_MAP_STENCIL = &HD11
Public Const GL_INDEX_SHIFT = &HD12
Public Const GL_INDEX_OFFSET = &HD13
Public Const GL_RED_SCALE = &HD14
Public Const GL_RED_BIAS = &HD15
Public Const GL_GREEN_SCALE = &HD18
Public Const GL_GREEN_BIAS = &HD19
Public Const GL_BLUE_SCALE = &HD1A
Public Const GL_BLUE_BIAS = &HD1B
Public Const GL_ALPHA_SCALE = &HD1C
Public Const GL_ALPHA_BIAS = &HD1D
Public Const GL_DEPTH_SCALE = &HD1E
Public Const GL_DEPTH_BIAS = &HD1F
Public Const GL_PIXEL_MAP_S_TO_S_SIZE = &HCB1
Public Const GL_PIXEL_MAP_I_TO_I_SIZE = &HCB0
Public Const GL_PIXEL_MAP_I_TO_R_SIZE = &HCB2
Public Const GL_PIXEL_MAP_I_TO_G_SIZE = &HCB3
Public Const GL_PIXEL_MAP_I_TO_B_SIZE = &HCB4
Public Const GL_PIXEL_MAP_I_TO_A_SIZE = &HCB5
Public Const GL_PIXEL_MAP_R_TO_R_SIZE = &HCB6
Public Const GL_PIXEL_MAP_G_TO_G_SIZE = &HCB7
Public Const GL_PIXEL_MAP_B_TO_B_SIZE = &HCB8
Public Const GL_PIXEL_MAP_A_TO_A_SIZE = &HCB9
Public Const GL_PIXEL_MAP_S_TO_S = &HC71
Public Const GL_PIXEL_MAP_I_TO_I = &HC70
Public Const GL_PIXEL_MAP_I_TO_R = &HC72
Public Const GL_PIXEL_MAP_I_TO_G = &HC73
Public Const GL_PIXEL_MAP_I_TO_B = &HC74
Public Const GL_PIXEL_MAP_I_TO_A = &HC75
Public Const GL_PIXEL_MAP_R_TO_R = &HC76
Public Const GL_PIXEL_MAP_G_TO_G = &HC77
Public Const GL_PIXEL_MAP_B_TO_B = &HC78
Public Const GL_PIXEL_MAP_A_TO_A = &HC79
Public Const GL_PACK_ALIGNMENT = &HD05
Public Const GL_PACK_LSB_FIRST = &HD01
Public Const GL_PACK_ROW_LENGTH = &HD02
Public Const GL_PACK_SKIP_PIXELS = &HD04
Public Const GL_PACK_SKIP_ROWS = &HD03
Public Const GL_PACK_SWAP_BYTES = &HD00
Public Const GL_UNPACK_ALIGNMENT = &HCF5
Public Const GL_UNPACK_LSB_FIRST = &HCF1
Public Const GL_UNPACK_ROW_LENGTH = &HCF2
Public Const GL_UNPACK_SKIP_PIXELS = &HCF4
Public Const GL_UNPACK_SKIP_ROWS = &HCF3
Public Const GL_UNPACK_SWAP_BYTES = &HCF0
Public Const GL_ZOOM_X = &HD16
Public Const GL_ZOOM_Y = &HD17

' Texture mapping
Public Const GL_TEXTURE_ENV = &H2300
Public Const GL_TEXTURE_ENV_MODE = &H2200
Public Const GL_TEXTURE_1D = &HDE0
Public Const GL_TEXTURE_2D = &HDE1
Public Const GL_TEXTURE_WRAP_S = &H2802
Public Const GL_TEXTURE_WRAP_T = &H2803
Public Const GL_TEXTURE_MAG_FILTER = &H2800
Public Const GL_TEXTURE_MIN_FILTER = &H2801
Public Const GL_TEXTURE_ENV_COLOR = &H2201
Public Const GL_TEXTURE_GEN_S = &HC60
Public Const GL_TEXTURE_GEN_T = &HC61
Public Const GL_TEXTURE_GEN_MODE = &H2500
Public Const GL_TEXTURE_BORDER_COLOR = &H1004
Public Const GL_TEXTURE_WIDTH = &H1000
Public Const GL_TEXTURE_HEIGHT = &H1001
Public Const GL_TEXTURE_BORDER = &H1005
Public Const GL_TEXTURE_COMPONENTS = &H1003
Public Const GL_TEXTURE_RED_SIZE = &H805C
Public Const GL_TEXTURE_GREEN_SIZE = &H805D
Public Const GL_TEXTURE_BLUE_SIZE = &H805E
Public Const GL_TEXTURE_ALPHA_SIZE = &H805F
Public Const GL_TEXTURE_LUMINANCE_SIZE = &H8060
Public Const GL_TEXTURE_INTENSITY_SIZE = &H8061
Public Const GL_NEAREST_MIPMAP_NEAREST = &H2700
Public Const GL_NEAREST_MIPMAP_LINEAR = &H2702
Public Const GL_LINEAR_MIPMAP_NEAREST = &H2701
Public Const GL_LINEAR_MIPMAP_LINEAR = &H2703
Public Const GL_OBJECT_LINEAR = &H2401
Public Const GL_OBJECT_PLANE = &H2501
Public Const GL_EYE_LINEAR = &H2400
Public Const GL_EYE_PLANE = &H2502
Public Const GL_SPHERE_MAP = &H2402
Public Const GL_DECAL = &H2101
Public Const GL_MODULATE = &H2100
Public Const GL_NEAREST = &H2600
Public Const GL_REPEAT = &H2901
Public Const GL_CLAMP = &H2900
Public Const GL_S = &H2000
Public Const GL_T = &H2001
Public Const GL_R = &H2002
Public Const GL_Q = &H2003
Public Const GL_TEXTURE_GEN_R = &HC62
Public Const GL_TEXTURE_GEN_Q = &HC63

' Utility
Public Const GL_VENDOR = &H1F00
Public Const GL_RENDERER = &H1F01
Public Const GL_VERSION = &H1F02
Public Const GL_EXTENSIONS = &H1F03

' Errors
Public Const GL_NO_ERROR = &H0
Public Const GL_INVALID_VALUE = &H501
Public Const GL_INVALID_ENUM = &H500
Public Const GL_INVALID_OPERATION = &H502
Public Const GL_STACK_OVERFLOW = &H503
Public Const GL_STACK_UNDERFLOW = &H504
Public Const GL_OUT_OF_MEMORY = &H505

' glPush/PopAttrib bits
Public Const GL_CURRENT_BIT = &H1
Public Const GL_POINT_BIT = &H2
Public Const GL_LINE_BIT = &H4
Public Const GL_POLYGON_BIT = &H8
Public Const GL_POLYGON_STIPPLE_BIT = &H10
Public Const GL_PIXEL_MODE_BIT = &H20
Public Const GL_LIGHTING_BIT = &H40
Public Const GL_FOG_BIT = &H80
Public Const GL_DEPTH_BUFFER_BIT = &H100
Public Const GL_ACCUM_BUFFER_BIT = &H200
Public Const GL_STENCIL_BUFFER_BIT = &H400
Public Const GL_VIEWPORT_BIT = &H800
Public Const GL_TRANSFORM_BIT = &H1000
Public Const GL_ENABLE_BIT = &H2000
Public Const GL_COLOR_BUFFER_BIT = &H4000
Public Const GL_HINT_BIT = &H8000
Public Const GL_EVAL_BIT = &H10000
Public Const GL_LIST_BIT = &H20000
Public Const GL_TEXTURE_BIT = &H40000
Public Const GL_SCISSOR_BIT = &H80000
Public Const GL_ALL_ATTRIB_BITS = &HFFFFF


' OpenGL 1.1
Public Const GL_PROXY_TEXTURE_1D = &H8063
Public Const GL_PROXY_TEXTURE_2D = &H8064
Public Const GL_TEXTURE_PRIORITY = &H8066
Public Const GL_TEXTURE_RESIDENT = &H8067
Public Const GL_TEXTURE_BINDING_1D = &H8068
Public Const GL_TEXTURE_BINDING_2D = &H8069
Public Const GL_TEXTURE_INTERNAL_FORMAT = &H1003
Public Const GL_ALPHA4 = &H803B
Public Const GL_ALPHA8 = &H803C
Public Const GL_ALPHA12 = &H803D
Public Const GL_ALPHA16 = &H803E
Public Const GL_LUMINANCE4 = &H803F
Public Const GL_LUMINANCE8 = &H8040
Public Const GL_LUMINANCE12 = &H8041
Public Const GL_LUMINANCE16 = &H8042
Public Const GL_LUMINANCE4_ALPHA4 = &H8043
Public Const GL_LUMINANCE6_ALPHA2 = &H8044
Public Const GL_LUMINANCE8_ALPHA8 = &H8045
Public Const GL_LUMINANCE12_ALPHA4 = &H8046
Public Const GL_LUMINANCE12_ALPHA12 = &H8047
Public Const GL_LUMINANCE16_ALPHA16 = &H8048
Public Const GL_INTENSITY = &H8049
Public Const GL_INTENSITY4 = &H804A
Public Const GL_INTENSITY8 = &H804B
Public Const GL_INTENSITY12 = &H804C
Public Const GL_INTENSITY16 = &H804D
Public Const GL_R3_G3_B2 = &H2A10
Public Const GL_RGB4 = &H804F
Public Const GL_RGB5 = &H8050
Public Const GL_RGB8 = &H8051
Public Const GL_RGB10 = &H8052
Public Const GL_RGB12 = &H8053
Public Const GL_RGB16 = &H8054
Public Const GL_RGBA2 = &H8055
Public Const GL_RGBA4 = &H8056
Public Const GL_RGB5_A1 = &H8057
Public Const GL_RGBA8 = &H8058
Public Const GL_RGB10_A2 = &H8059
Public Const GL_RGBA12 = &H805A
Public Const GL_RGBA16 = &H805B
Public Const GL_CLIENT_PIXEL_STORE_BIT = &H1
Public Const GL_CLIENT_VERTEX_ARRAY_BIT = &H2
Public Const GL_ALL_CLIENT_ATTRIB_BITS = &HFFFFFFFF
Public Const GL_CLIENT_ALL_ATTRIB_BITS = &HFFFFFFFF




'*************************************************************************
'* Function Definitions
'*************************************************************************

' Miscellaneous
Public Declare Sub glClearIndex Lib "opengl32.dll" (ByVal c As Single)
Public Declare Sub glClearColor Lib "opengl32.dll" (ByVal red As Single, ByVal green As Single, ByVal blue As Single, ByVal alpha As Single)
Public Declare Sub glClear Lib "opengl32.dll" (ByVal mask As Long)
Public Declare Sub glIndexMask Lib "opengl32.dll" (ByVal mask As Long)
Public Declare Sub glColorMask Lib "opengl32.dll" (ByVal red As Byte, ByVal green As Byte, ByVal blue As Byte, ByVal alpha As Byte)
Public Declare Sub glAlphaFunc Lib "opengl32.dll" (ByVal func As Long, ByVal ref As Single)
Public Declare Sub glBlendFunc Lib "opengl32.dll" (ByVal sfactor As Long, ByVal dfactor As Long)
Public Declare Sub glLogicOp Lib "opengl32.dll" (ByVal opcode As Long)
Public Declare Sub glCullFace Lib "opengl32.dll" (ByVal mode As Long)
Public Declare Sub glFrontFace Lib "opengl32.dll" (ByVal mode As Long)
Public Declare Sub glPointSize Lib "opengl32.dll" (ByVal size As Single)
Public Declare Sub glLineWidth Lib "opengl32.dll" (ByVal width As Single)
Public Declare Sub glLineStipple Lib "opengl32.dll" (ByVal factor As Long, ByVal pattern As Integer)
Public Declare Sub glPolygonMode Lib "opengl32.dll" (ByVal face As Long, ByVal mode As Long)
Public Declare Sub glPolygonOffset Lib "opengl32.dll" (ByVal factor As Single, ByVal units As Single)
Public Declare Sub glPolygonStipple Lib "opengl32.dll" (mask As Byte)
Public Declare Sub glGetPolygonStipple Lib "opengl32.dll" (mask As Byte)
Public Declare Sub glEdgeFlag Lib "opengl32.dll" (ByVal flag As Byte)
Public Declare Sub glEdgeFlagv Lib "opengl32.dll" (flag As Byte)
Public Declare Sub glScissor Lib "opengl32.dll" (ByVal x As Long, ByVal y As Long, ByVal width As Long, ByVal height As Long)
Public Declare Sub glClipPlane Lib "opengl32.dll" (ByVal plane As Long, equation As Double)
Public Declare Sub glGetClipPlane Lib "opengl32.dll" (ByVal plane As Long, equation As Double)
Public Declare Sub glDrawBuffer Lib "opengl32.dll" (ByVal mode As Long)
Public Declare Sub glReadBuffer Lib "opengl32.dll" (ByVal mode As Long)
Public Declare Sub glEnable Lib "opengl32.dll" (ByVal cap As Long)
Public Declare Sub glDisable Lib "opengl32.dll" (ByVal cap As Long)
Public Declare Function glIsEnabled Lib "opengl32.dll" (ByVal cap As Long) As Byte

Public Declare Sub glGetBooleanv Lib "opengl32.dll" (ByVal pname As Long, params As Byte)
Public Declare Sub glGetDoublev Lib "opengl32.dll" (ByVal pname As Long, params As Double)
Public Declare Sub glGetFloatv Lib "opengl32.dll" (ByVal pname As Long, params As Single)
Public Declare Sub glGetIntegerv Lib "opengl32.dll" (ByVal pname As Long, params As Long)
Public Declare Sub glPushAttrib Lib "opengl32.dll" (ByVal mask As Long)
Public Declare Sub glPopAttrib Lib "opengl32.dll" ()

Public Declare Function glRenderMode Lib "opengl32.dll" (ByVal mode As Long) As Long
Public Declare Function glGetError Lib "opengl32.dll" () As Long
Public Declare Function glGetString Lib "opengl32.dll" (ByVal name As Long) As String
Public Declare Sub glFinish Lib "opengl32.dll" ()
Public Declare Sub glFlush Lib "opengl32.dll" ()
Public Declare Sub glHint Lib "opengl32.dll" (ByVal target As Long, ByVal mode As Long)

' Depth Buffer
Public Declare Sub glClearDepth Lib "opengl32.dll" (ByVal depth As Double)
Public Declare Sub glDepthFunc Lib "opengl32.dll" (ByVal func As Long)
Public Declare Sub glDepthMask Lib "opengl32.dll" (ByVal flag As Byte)
Public Declare Sub glDepthRange Lib "opengl32.dll" (ByVal near_val As Double, ByVal far_val As Double)

' Accumulation Buffer
Public Declare Sub glClearAccum Lib "opengl32.dll" (ByVal red As Single, ByVal green As Single, ByVal blue As Single, ByVal alpha As Single)
Public Declare Sub glAccum Lib "opengl32.dll" (ByVal op As Long, ByVal value As Single)

' Transformation
Public Declare Sub glMatrixMode Lib "opengl32.dll" (ByVal mode As Long)
Public Declare Sub glOrtho Lib "opengl32.dll" (ByVal left As Double, ByVal right As Double, ByVal bottom As Double, ByVal top As Double, ByVal near_val As Double, ByVal far_val As Double)
Public Declare Sub glFrustum Lib "opengl32.dll" (ByVal left As Double, ByVal right As Double, ByVal bottom As Double, ByVal top As Double, ByVal near_val As Double, ByVal far_val As Double)
Public Declare Sub glViewport Lib "opengl32.dll" (ByVal x As Long, ByVal y As Long, ByVal width As Long, ByVal height As Long)
Public Declare Sub glPushMatrix Lib "opengl32.dll" ()
Public Declare Sub glPopMatrix Lib "opengl32.dll" ()
Public Declare Sub glLoadIdentity Lib "opengl32.dll" ()
Public Declare Sub glLoadMatrixd Lib "opengl32.dll" (m As Double)
Public Declare Sub glLoadMatrixf Lib "opengl32.dll" (m As Single)
Public Declare Sub glMultMatrixd Lib "opengl32.dll" (m As Double)
Public Declare Sub glMultMatrixf Lib "opengl32.dll" (m As Single)
Public Declare Sub glRotated Lib "opengl32.dll" (ByVal angle As Double, ByVal x As Double, ByVal y As Double, ByVal z As Double)
Public Declare Sub glRotatef Lib "opengl32.dll" (ByVal angle As Single, ByVal x As Single, ByVal y As Single, ByVal z As Single)
Public Declare Sub glScaled Lib "opengl32.dll" (ByVal x As Double, ByVal y As Double, ByVal z As Double)
Public Declare Sub glScalef Lib "opengl32.dll" (ByVal x As Single, ByVal y As Single, ByVal z As Single)
Public Declare Sub glTranslated Lib "opengl32.dll" (ByVal x As Double, ByVal y As Double, ByVal z As Double)
Public Declare Sub glTranslatef Lib "opengl32.dll" (ByVal x As Single, ByVal y As Single, ByVal z As Single)

' Display Lists
Public Declare Function glIsList Lib "opengl32.dll" (ByVal list As Long) As Byte
Public Declare Sub glDeleteLists Lib "opengl32.dll" (ByVal list As Long, ByVal range As Long)
Public Declare Function glGenLists Lib "opengl32.dll" (ByVal range As Long) As Long
Public Declare Sub glNewList Lib "opengl32.dll" (ByVal list As Long, ByVal mode As Long)
Public Declare Sub glEndList Lib "opengl32.dll" ()
Public Declare Sub glCallList Lib "opengl32.dll" (ByVal list As Long)
Public Declare Sub glCallLists Lib "opengl32.dll" (ByVal n As Long, ByVal t As Long, lists As Any)
Public Declare Sub glListBase Lib "opengl32.dll" (ByVal base As Long)

' Drawing Functions
Public Declare Sub glBegin Lib "opengl32.dll" (ByVal mode As Long)
Public Declare Sub glEnd Lib "opengl32.dll" ()
Public Declare Sub glVertex2d Lib "opengl32.dll" (ByVal x As Double, ByVal y As Double)
Public Declare Sub glVertex2f Lib "opengl32.dll" (ByVal x As Single, ByVal y As Single)
Public Declare Sub glVertex2i Lib "opengl32.dll" (ByVal x As Long, ByVal y As Long)
Public Declare Sub glVertex2s Lib "opengl32.dll" (ByVal x As Integer, ByVal y As Integer)
Public Declare Sub glVertex3d Lib "opengl32.dll" (ByVal x As Double, ByVal y As Double, ByVal z As Double)
Public Declare Sub glVertex3f Lib "opengl32.dll" (ByVal x As Single, ByVal y As Single, ByVal z As Single)
Public Declare Sub glVertex3i Lib "opengl32.dll" (ByVal x As Long, ByVal y As Long, ByVal z As Long)
Public Declare Sub glVertex3s Lib "opengl32.dll" (ByVal x As Integer, ByVal y As Integer, ByVal z As Integer)
Public Declare Sub glVertex4d Lib "opengl32.dll" (ByVal x As Double, ByVal y As Double, ByVal z As Double, ByVal w As Double)
Public Declare Sub glVertex4f Lib "opengl32.dll" (ByVal x As Single, ByVal y As Single, ByVal z As Single, ByVal w As Single)
Public Declare Sub glVertex4i Lib "opengl32.dll" (ByVal x As Long, ByVal y As Long, ByVal z As Long, ByVal w As Long)
Public Declare Sub glVertex4s Lib "opengl32.dll" (ByVal x As Integer, ByVal y As Integer, ByVal z As Integer, ByVal w As Integer)
Public Declare Sub glVertex2dv Lib "opengl32.dll" (v As Double)
Public Declare Sub glVertex2fv Lib "opengl32.dll" (v As Single)
Public Declare Sub glVertex2iv Lib "opengl32.dll" (v As Long)
Public Declare Sub glVertex2sv Lib "opengl32.dll" (v As Integer)
Public Declare Sub glVertex3dv Lib "opengl32.dll" (v As Double)
Public Declare Sub glVertex3fv Lib "opengl32.dll" (v As Single)
Public Declare Sub glVertex3iv Lib "opengl32.dll" (v As Long)
Public Declare Sub glVertex3sv Lib "opengl32.dll" (v As Integer)
Public Declare Sub glVertex4dv Lib "opengl32.dll" (v As Double)
Public Declare Sub glVertex4fv Lib "opengl32.dll" (v As Single)
Public Declare Sub glVertex4iv Lib "opengl32.dll" (v As Long)
Public Declare Sub glVertex4sv Lib "opengl32.dll" (v As Integer)
Public Declare Sub glNormal3b Lib "opengl32.dll" (ByVal nx As Byte, ByVal ny As Byte, ByVal nz As Byte)
Public Declare Sub glNormal3d Lib "opengl32.dll" (ByVal nx As Double, ByVal ny As Double, ByVal nz As Double)
Public Declare Sub glNormal3f Lib "opengl32.dll" (ByVal nx As Single, ByVal ny As Single, ByVal nz As Single)
Public Declare Sub glNormal3i Lib "opengl32.dll" (ByVal nx As Long, ByVal ny As Long, ByVal nz As Long)
Public Declare Sub glNormal3s Lib "opengl32.dll" (ByVal nx As Integer, ByVal ny As Integer, ByVal nz As Integer)
Public Declare Sub glNormal3bv Lib "opengl32.dll" (v As Byte)
Public Declare Sub glNormal3dv Lib "opengl32.dll" (v As Double)
Public Declare Sub glNormal3fv Lib "opengl32.dll" (v As Single)
Public Declare Sub glNormal3iv Lib "opengl32.dll" (v As Long)
Public Declare Sub glNormal3sv Lib "opengl32.dll" (v As Integer)
Public Declare Sub glIndexd Lib "opengl32.dll" (ByVal c As Double)
Public Declare Sub glIndexf Lib "opengl32.dll" (ByVal c As Single)
Public Declare Sub glIndexi Lib "opengl32.dll" (ByVal c As Long)
Public Declare Sub glIndexs Lib "opengl32.dll" (ByVal c As Integer)

Public Declare Sub glIndexdv Lib "opengl32.dll" (c As Double)
Public Declare Sub glIndexfv Lib "opengl32.dll" (c As Single)
Public Declare Sub glIndexiv Lib "opengl32.dll" (c As Long)
Public Declare Sub glIndexsv Lib "opengl32.dll" (c As Integer)

Public Declare Sub glColor3b Lib "opengl32.dll" (ByVal red As Byte, ByVal green As Byte, ByVal blue As Byte)
Public Declare Sub glColor3d Lib "opengl32.dll" (ByVal red As Double, ByVal green As Double, ByVal blue As Double)
Public Declare Sub glColor3f Lib "opengl32.dll" (ByVal red As Single, ByVal green As Single, ByVal blue As Single)
Public Declare Sub glColor3i Lib "opengl32.dll" (ByVal red As Long, ByVal green As Long, ByVal blue As Long)
Public Declare Sub glColor3s Lib "opengl32.dll" (ByVal red As Integer, ByVal green As Integer, ByVal blue As Integer)
Public Declare Sub glColor3ub Lib "opengl32.dll" (ByVal red As Byte, ByVal green As Byte, ByVal blue As Byte)
Public Declare Sub glColor3ui Lib "opengl32.dll" (ByVal red As Long, ByVal green As Long, ByVal blue As Long)
Public Declare Sub glColor3us Lib "opengl32.dll" (ByVal red As Integer, ByVal green As Integer, ByVal blue As Integer)
Public Declare Sub glColor4b Lib "opengl32.dll" (ByVal red As Byte, ByVal green As Byte, ByVal blue As Byte, ByVal alpha As Byte)
Public Declare Sub glColor4d Lib "opengl32.dll" (ByVal red As Double, ByVal green As Double, ByVal blue As Double, ByVal alpha As Double)
Public Declare Sub glColor4f Lib "opengl32.dll" (ByVal red As Single, ByVal green As Single, ByVal blue As Single, ByVal alpha As Single)
Public Declare Sub glColor4i Lib "opengl32.dll" (ByVal red As Long, ByVal green As Long, ByVal blue As Long, ByVal alpha As Long)
Public Declare Sub glColor4s Lib "opengl32.dll" (ByVal red As Integer, ByVal green As Integer, ByVal blue As Integer, ByVal alpha As Integer)
Public Declare Sub glColor4ub Lib "opengl32.dll" (ByVal red As Byte, ByVal green As Byte, ByVal blue As Byte, ByVal alpha As Byte)
Public Declare Sub glColor4ui Lib "opengl32.dll" (ByVal red As Long, ByVal green As Long, ByVal blue As Long, ByVal alpha As Long)
Public Declare Sub glColor4us Lib "opengl32.dll" (ByVal red As Integer, ByVal green As Integer, ByVal blue As Integer, ByVal alpha As Integer)
Public Declare Sub glColor3bv Lib "opengl32.dll" (v As Byte)
Public Declare Sub glColor3dv Lib "opengl32.dll" (v As Double)
Public Declare Sub glColor3fv Lib "opengl32.dll" (v As Single)
Public Declare Sub glColor3iv Lib "opengl32.dll" (v As Long)
Public Declare Sub glColor3sv Lib "opengl32.dll" (v As Integer)
Public Declare Sub glColor3ubv Lib "opengl32.dll" (v As Byte)
Public Declare Sub glColor3uiv Lib "opengl32.dll" (v As Long)
Public Declare Sub glColor3usv Lib "opengl32.dll" (v As Integer)
Public Declare Sub glColor4bv Lib "opengl32.dll" (v As Byte)
Public Declare Sub glColor4dv Lib "opengl32.dll" (v As Double)
Public Declare Sub glColor4fv Lib "opengl32.dll" (v As Single)
Public Declare Sub glColor4iv Lib "opengl32.dll" (v As Long)
Public Declare Sub glColor4sv Lib "opengl32.dll" (v As Integer)
Public Declare Sub glColor4ubv Lib "opengl32.dll" (v As Byte)
Public Declare Sub glColor4uiv Lib "opengl32.dll" (v As Long)
Public Declare Sub glColor4usv Lib "opengl32.dll" (v As Integer)
Public Declare Sub glTexCoord1d Lib "opengl32.dll" (ByVal s As Double)
Public Declare Sub glTexCoord1f Lib "opengl32.dll" (ByVal s As Single)
Public Declare Sub glTexCoord1i Lib "opengl32.dll" (ByVal s As Long)
Public Declare Sub glTexCoord1s Lib "opengl32.dll" (ByVal s As Integer)
Public Declare Sub glTexCoord2d Lib "opengl32.dll" (ByVal s As Double, ByVal t As Double)
Public Declare Sub glTexCoord2f Lib "opengl32.dll" (ByVal s As Single, ByVal t As Single)
Public Declare Sub glTexCoord2i Lib "opengl32.dll" (ByVal s As Long, ByVal t As Long)
Public Declare Sub glTexCoord2s Lib "opengl32.dll" (ByVal s As Integer, ByVal t As Integer)
Public Declare Sub glTexCoord3d Lib "opengl32.dll" (ByVal s As Double, ByVal t As Double, ByVal r As Double)
Public Declare Sub glTexCoord3f Lib "opengl32.dll" (ByVal s As Single, ByVal t As Single, ByVal r As Single)
Public Declare Sub glTexCoord3i Lib "opengl32.dll" (ByVal s As Long, ByVal t As Long, ByVal r As Long)
Public Declare Sub glTexCoord3s Lib "opengl32.dll" (ByVal s As Integer, ByVal t As Integer, ByVal r As Integer)
Public Declare Sub glTexCoord4d Lib "opengl32.dll" (ByVal s As Double, ByVal t As Double, ByVal r As Double, ByVal q As Double)
Public Declare Sub glTexCoord4f Lib "opengl32.dll" (ByVal s As Single, ByVal t As Single, ByVal r As Single, ByVal q As Single)
Public Declare Sub glTexCoord4i Lib "opengl32.dll" (ByVal s As Long, ByVal t As Long, ByVal r As Long, ByVal q As Long)
Public Declare Sub glTexCoord4s Lib "opengl32.dll" (ByVal s As Integer, ByVal t As Integer, ByVal r As Integer, ByVal q As Integer)
Public Declare Sub glTexCoord1dv Lib "opengl32.dll" (v As Double)
Public Declare Sub glTexCoord1fv Lib "opengl32.dll" (v As Single)
Public Declare Sub glTexCoord1iv Lib "opengl32.dll" (v As Long)
Public Declare Sub glTexCoord1sv Lib "opengl32.dll" (v As Integer)
Public Declare Sub glTexCoord2dv Lib "opengl32.dll" (v As Double)
Public Declare Sub glTexCoord2fv Lib "opengl32.dll" (v As Single)
Public Declare Sub glTexCoord2iv Lib "opengl32.dll" (v As Long)
Public Declare Sub glTexCoord2sv Lib "opengl32.dll" (v As Integer)
Public Declare Sub glTexCoord3dv Lib "opengl32.dll" (v As Double)
Public Declare Sub glTexCoord3fv Lib "opengl32.dll" (v As Single)
Public Declare Sub glTexCoord3iv Lib "opengl32.dll" (v As Long)
Public Declare Sub glTexCoord3sv Lib "opengl32.dll" (v As Integer)
Public Declare Sub glTexCoord4dv Lib "opengl32.dll" (v As Double)
Public Declare Sub glTexCoord4fv Lib "opengl32.dll" (v As Single)
Public Declare Sub glTexCoord4iv Lib "opengl32.dll" (v As Long)
Public Declare Sub glTexCoord4sv Lib "opengl32.dll" (v As Integer)
Public Declare Sub glRasterPos2d Lib "opengl32.dll" (ByVal x As Double, ByVal y As Double)
Public Declare Sub glRasterPos2f Lib "opengl32.dll" (ByVal x As Single, ByVal y As Single)
Public Declare Sub glRasterPos2i Lib "opengl32.dll" (ByVal x As Long, ByVal y As Long)
Public Declare Sub glRasterPos2s Lib "opengl32.dll" (ByVal x As Integer, ByVal y As Integer)
Public Declare Sub glRasterPos3d Lib "opengl32.dll" (ByVal x As Double, ByVal y As Double, ByVal z As Double)
Public Declare Sub glRasterPos3f Lib "opengl32.dll" (ByVal x As Single, ByVal y As Single, ByVal z As Single)
Public Declare Sub glRasterPos3i Lib "opengl32.dll" (ByVal x As Long, ByVal y As Long, ByVal z As Long)
Public Declare Sub glRasterPos3s Lib "opengl32.dll" (ByVal x As Integer, ByVal y As Integer, ByVal z As Integer)
Public Declare Sub glRasterPos4d Lib "opengl32.dll" (ByVal x As Double, ByVal y As Double, ByVal z As Double, ByVal w As Double)
Public Declare Sub glRasterPos4f Lib "opengl32.dll" (ByVal x As Single, ByVal y As Single, ByVal z As Single, ByVal w As Single)
Public Declare Sub glRasterPos4i Lib "opengl32.dll" (ByVal x As Long, ByVal y As Long, ByVal z As Long, ByVal w As Long)
Public Declare Sub glRasterPos4s Lib "opengl32.dll" (ByVal x As Integer, ByVal y As Integer, ByVal z As Integer, ByVal w As Integer)
Public Declare Sub glRasterPos2dv Lib "opengl32.dll" (v As Double)
Public Declare Sub glRasterPos2fv Lib "opengl32.dll" (v As Single)
Public Declare Sub glRasterPos2iv Lib "opengl32.dll" (v As Long)
Public Declare Sub glRasterPos2sv Lib "opengl32.dll" (v As Integer)
Public Declare Sub glRasterPos3dv Lib "opengl32.dll" (v As Double)
Public Declare Sub glRasterPos3fv Lib "opengl32.dll" (v As Single)
Public Declare Sub glRasterPos3iv Lib "opengl32.dll" (v As Long)
Public Declare Sub glRasterPos3sv Lib "opengl32.dll" (v As Integer)
Public Declare Sub glRasterPos4dv Lib "opengl32.dll" (v As Double)
Public Declare Sub glRasterPos4fv Lib "opengl32.dll" (v As Single)
Public Declare Sub glRasterPos4iv Lib "opengl32.dll" (v As Long)
Public Declare Sub glRasterPos4sv Lib "opengl32.dll" (v As Integer)
Public Declare Sub glRectd Lib "opengl32.dll" (ByVal x1 As Double, ByVal y1 As Double, ByVal x2 As Double, ByVal y2 As Double)
Public Declare Sub glRectf Lib "opengl32.dll" (ByVal x1 As Single, ByVal y1 As Single, ByVal x2 As Single, ByVal y2 As Single)
Public Declare Sub glRecti Lib "opengl32.dll" (ByVal x1 As Long, ByVal y1 As Long, ByVal x2 As Long, ByVal y2 As Long)
Public Declare Sub glRects Lib "opengl32.dll" (ByVal x1 As Integer, ByVal y1 As Integer, ByVal x2 As Integer, ByVal y2 As Integer)
Public Declare Sub glRectdv Lib "opengl32.dll" (v1 As Double, v2 As Double)
Public Declare Sub glRectfv Lib "opengl32.dll" (v1 As Single, v2 As Single)
Public Declare Sub glRectiv Lib "opengl32.dll" (v1 As Long, v2 As Long)
Public Declare Sub glRectsv Lib "opengl32.dll" (v1 As Integer, v2 As Integer)

' Lighting
Public Declare Sub glShadeModel Lib "opengl32.dll" (ByVal mode As Long)
Public Declare Sub glLightf Lib "opengl32.dll" (ByVal light As Long, ByVal pname As Long, ByVal param As Single)
Public Declare Sub glLighti Lib "opengl32.dll" (ByVal light As Long, ByVal pname As Long, ByVal param As Long)
Public Declare Sub glLightfv Lib "opengl32.dll" (ByVal light As Long, ByVal pname As Long, params As Single)
Public Declare Sub glLightiv Lib "opengl32.dll" (ByVal light As Long, ByVal pname As Long, params As Long)
Public Declare Sub glGetLightfv Lib "opengl32.dll" (ByVal light As Long, ByVal pname As Long, params As Single)
Public Declare Sub glGetLightiv Lib "opengl32.dll" (ByVal light As Long, ByVal pname As Long, params As Long)
Public Declare Sub glLightModelf Lib "opengl32.dll" (ByVal pname As Long, ByVal param As Single)
Public Declare Sub glLightModeli Lib "opengl32.dll" (ByVal pname As Long, ByVal param As Long)
Public Declare Sub glLightModelfv Lib "opengl32.dll" (ByVal pname As Long, params As Single)
Public Declare Sub glLightModeliv Lib "opengl32.dll" (ByVal pname As Long, params As Long)
Public Declare Sub glMaterialf Lib "opengl32.dll" (ByVal face As Long, ByVal pname As Long, ByVal param As Single)
Public Declare Sub glMateriali Lib "opengl32.dll" (ByVal face As Long, ByVal pname As Long, ByVal param As Long)
Public Declare Sub glMaterialfv Lib "opengl32.dll" (ByVal face As Long, ByVal pname As Long, params As Single)
Public Declare Sub glMaterialiv Lib "opengl32.dll" (ByVal face As Long, ByVal pname As Long, params As Long)
Public Declare Sub glGetMaterialfv Lib "opengl32.dll" (ByVal face As Long, ByVal pname As Long, params As Single)
Public Declare Sub glGetMaterialiv Lib "opengl32.dll" (ByVal face As Long, ByVal pname As Long, params As Long)
Public Declare Sub glColorMaterial Lib "opengl32.dll" (ByVal face As Long, ByVal mode As Long)

' Raster functions
Public Declare Sub glPixelZoom Lib "opengl32.dll" (ByVal xfactor As Single, ByVal yfactor As Single)
Public Declare Sub glPixelStoref Lib "opengl32.dll" (ByVal pname As Long, ByVal param As Single)
Public Declare Sub glPixelStorei Lib "opengl32.dll" (ByVal pname As Long, ByVal param As Long)
Public Declare Sub glPixelTransferf Lib "opengl32.dll" (ByVal pname As Long, ByVal param As Single)
Public Declare Sub glPixelTransferi Lib "opengl32.dll" (ByVal pname As Long, ByVal param As Long)
Public Declare Sub glPixelMapfv Lib "opengl32.dll" (ByVal map As Long, ByVal mapsize As Long, values As Single)
Public Declare Sub glPixelMapuiv Lib "opengl32.dll" (ByVal map As Long, ByVal mapsize As Long, values As Long)
Public Declare Sub glPixelMapusv Lib "opengl32.dll" (ByVal map As Long, ByVal mapsize As Long, values As Integer)
Public Declare Sub glGetPixelMapfv Lib "opengl32.dll" (ByVal map As Long, values As Single)
Public Declare Sub glGetPixelMapuiv Lib "opengl32.dll" (ByVal map As Long, values As Long)
Public Declare Sub glGetPixelMapusv Lib "opengl32.dll" (ByVal map As Long, values As Integer)
Public Declare Sub glBitmap Lib "opengl32.dll" (ByVal width As Long, ByVal height As Long, ByVal xorig As Single, ByVal yorig As Single, ByVal xmove As Single, ByVal ymove As Single, bitmap As Byte)
Public Declare Sub glReadPixels Lib "opengl32.dll" (ByVal x As Long, ByVal y As Long, ByVal width As Long, ByVal height As Long, ByVal format As Long, ByVal t As Long, pixels As Any)
Public Declare Sub glDrawPixels Lib "opengl32.dll" (ByVal width As Long, ByVal height As Long, ByVal format As Long, ByVal t As Long, pixels As Any)
Public Declare Sub glCopyPixels Lib "opengl32.dll" (ByVal x As Long, ByVal y As Long, ByVal width As Long, ByVal height As Long, ByVal t As Long)

' Stenciling
Public Declare Sub glStencilFunc Lib "opengl32.dll" (ByVal func As Long, ByVal ref As Long, ByVal mask As Long)
Public Declare Sub glStencilMask Lib "opengl32.dll" (ByVal mask As Long)
Public Declare Sub glStencilOp Lib "opengl32.dll" (ByVal fail As Long, ByVal zfail As Long, ByVal zpass As Long)
Public Declare Sub glClearStencil Lib "opengl32.dll" (ByVal s As Long)

' Texture mapping
Public Declare Sub glTexGend Lib "opengl32.dll" (ByVal coord As Long, ByVal pname As Long, ByVal param As Double)
Public Declare Sub glTexGenf Lib "opengl32.dll" (ByVal coord As Long, ByVal pname As Long, ByVal param As Single)
Public Declare Sub glTexGeni Lib "opengl32.dll" (ByVal coord As Long, ByVal pname As Long, ByVal param As Long)
Public Declare Sub glTexGendv Lib "opengl32.dll" (ByVal coord As Long, ByVal pname As Long, params As Double)
Public Declare Sub glTexGenfv Lib "opengl32.dll" (ByVal coord As Long, ByVal pname As Long, params As Single)
Public Declare Sub glTexGeniv Lib "opengl32.dll" (ByVal coord As Long, ByVal pname As Long, params As Long)
Public Declare Sub glGetTexGendv Lib "opengl32.dll" (ByVal coord As Long, ByVal pname As Long, params As Double)
Public Declare Sub glGetTexGenfv Lib "opengl32.dll" (ByVal coord As Long, ByVal pname As Long, params As Single)
Public Declare Sub glGetTexGeniv Lib "opengl32.dll" (ByVal coord As Long, ByVal pname As Long, params As Long)
Public Declare Sub glTexEnvf Lib "opengl32.dll" (ByVal target As Long, ByVal pname As Long, ByVal param As Single)
Public Declare Sub glTexEnvi Lib "opengl32.dll" (ByVal target As Long, ByVal pname As Long, ByVal param As Long)
Public Declare Sub glTexEnvfv Lib "opengl32.dll" (ByVal target As Long, ByVal pname As Long, params As Single)
Public Declare Sub glTexEnviv Lib "opengl32.dll" (ByVal target As Long, ByVal pname As Long, params As Long)
Public Declare Sub glGetTexEnvfv Lib "opengl32.dll" (ByVal target As Long, ByVal pname As Long, params As Single)
Public Declare Sub glGetTexEnviv Lib "opengl32.dll" (ByVal target As Long, ByVal pname As Long, params As Long)
Public Declare Sub glTexParameterf Lib "opengl32.dll" (ByVal target As Long, ByVal pname As Long, ByVal param As Single)
Public Declare Sub glTexParameteri Lib "opengl32.dll" (ByVal target As Long, ByVal pname As Long, ByVal param As Long)
Public Declare Sub glTexParameterfv Lib "opengl32.dll" (ByVal target As Long, ByVal pname As Long, params As Single)
Public Declare Sub glTexParameteriv Lib "opengl32.dll" (ByVal target As Long, ByVal pname As Long, params As Long)
Public Declare Sub glGetTexParameterfv Lib "opengl32.dll" (ByVal target As Long, ByVal pname As Long, params As Single)
Public Declare Sub glGetTexParameteriv Lib "opengl32.dll" (ByVal target As Long, ByVal pname As Long, params As Long)
Public Declare Sub glGetTexLevelParameterfv Lib "opengl32.dll" (ByVal target As Long, ByVal level As Long, ByVal pname As Long, params As Single)
Public Declare Sub glGetTexLevelParameteriv Lib "opengl32.dll" (ByVal target As Long, ByVal level As Long, ByVal pname As Long, params As Long)
Public Declare Sub glTexImage1D Lib "opengl32.dll" (ByVal target As Long, ByVal level As Long, ByVal internalFormat As Long, ByVal width As Long, ByVal border As Long, ByVal format As Long, ByVal t As Long, pixels As Any)
Public Declare Sub glTexImage2D Lib "opengl32.dll" (ByVal target As Long, ByVal level As Long, ByVal internalFormat As Long, ByVal width As Long, ByVal height As Long, ByVal border As Long, ByVal format As Long, ByVal t As Long, pixels As Any)
Public Declare Sub glGetTexImage Lib "opengl32.dll" (ByVal target As Long, ByVal level As Long, ByVal format As Long, ByVal t As Long, pixels As Any)

' Evaluators
Public Declare Sub glMap1d Lib "opengl32.dll" (ByVal target As Long, ByVal u1 As Double, ByVal u2 As Double, ByVal stride As Long, ByVal order As Long, points As Double)
Public Declare Sub glMap1f Lib "opengl32.dll" (ByVal target As Long, ByVal u1 As Single, ByVal u2 As Single, ByVal stride As Long, ByVal order As Long, points As Single)
Public Declare Sub glMap2d Lib "opengl32.dll" (ByVal target As Long, ByVal u1 As Double, ByVal u2 As Double, ByVal ustride As Long, ByVal uorder As Long, ByVal v1 As Double, ByVal v2 As Double, ByVal vstride As Long, ByVal vorder As Long, points As Double)
Public Declare Sub glMap2f Lib "opengl32.dll" (ByVal target As Long, ByVal u1 As Single, ByVal u2 As Single, ByVal ustride As Long, ByVal uorder As Long, ByVal v1 As Single, ByVal v2 As Single, ByVal vstride As Long, ByVal vorder As Long, points As Single)
Public Declare Sub glGetMapdv Lib "opengl32.dll" (ByVal target As Long, ByVal query As Long, v As Double)
Public Declare Sub glGetMapfv Lib "opengl32.dll" (ByVal target As Long, ByVal query As Long, v As Single)
Public Declare Sub glGetMapiv Lib "opengl32.dll" (ByVal target As Long, ByVal query As Long, v As Long)
Public Declare Sub glEvalCoord1d Lib "opengl32.dll" (ByVal u As Double)
Public Declare Sub glEvalCoord1f Lib "opengl32.dll" (ByVal u As Single)
Public Declare Sub glEvalCoord1dv Lib "opengl32.dll" (u As Double)
Public Declare Sub glEvalCoord1fv Lib "opengl32.dll" (u As Single)
Public Declare Sub glEvalCoord2d Lib "opengl32.dll" (ByVal u As Double, ByVal v As Double)
Public Declare Sub glEvalCoord2f Lib "opengl32.dll" (ByVal u As Single, ByVal v As Single)
Public Declare Sub glEvalCoord2dv Lib "opengl32.dll" (u As Double)
Public Declare Sub glEvalCoord2fv Lib "opengl32.dll" (u As Single)
Public Declare Sub glMapGrid1d Lib "opengl32.dll" (ByVal un As Long, ByVal u1 As Double, ByVal u2 As Double)
Public Declare Sub glMapGrid1f Lib "opengl32.dll" (ByVal un As Long, ByVal u1 As Single, ByVal u2 As Single)
Public Declare Sub glMapGrid2d Lib "opengl32.dll" (ByVal un As Long, ByVal u1 As Double, ByVal u2 As Double, ByVal vn As Long, ByVal v1 As Double, ByVal v2 As Double)
Public Declare Sub glMapGrid2f Lib "opengl32.dll" (ByVal un As Long, ByVal u1 As Single, ByVal u2 As Single, ByVal vn As Long, ByVal v1 As Single, ByVal v2 As Single)
Public Declare Sub glEvalPoint1 Lib "opengl32.dll" (ByVal i As Long)
Public Declare Sub glEvalPoint2 Lib "opengl32.dll" (ByVal i As Long, ByVal j As Long)
Public Declare Sub glEvalMesh1 Lib "opengl32.dll" (ByVal mode As Long, ByVal i1 As Long, ByVal i2 As Long)
Public Declare Sub glEvalMesh2 Lib "opengl32.dll" (ByVal mode As Long, ByVal i1 As Long, ByVal i2 As Long, ByVal j1 As Long, ByVal j2 As Long)

' Fog
Public Declare Sub glFogf Lib "opengl32.dll" (ByVal pname As Long, ByVal param As Single)
Public Declare Sub glFogi Lib "opengl32.dll" (ByVal pname As Long, ByVal param As Long)
Public Declare Sub glFogfv Lib "opengl32.dll" (ByVal pname As Long, params As Single)
Public Declare Sub glFogiv Lib "opengl32.dll" (ByVal pname As Long, params As Long)

' Selection and Feedback
Public Declare Sub glFeedbackBuffer Lib "opengl32.dll" (ByVal size As Long, ByVal t As Long, buffer As Single)
Public Declare Sub glPassThrough Lib "opengl32.dll" (ByVal token As Single)
Public Declare Sub glSelectBuffer Lib "opengl32.dll" (ByVal size As Long, buffer As Long)
Public Declare Sub glInitNames Lib "opengl32.dll" ()
Public Declare Sub glLoadName Lib "opengl32.dll" (ByVal name As Long)
Public Declare Sub glPushName Lib "opengl32.dll" (ByVal name As Long)
Public Declare Sub glPopName Lib "opengl32.dll" ()


' 1.1 functions
' texture objects
Public Declare Sub glGenTextures Lib "opengl32.dll" (ByVal n As Long, textures As Long)
Public Declare Sub glDeleteTextures Lib "opengl32.dll" (ByVal n As Long, textures As Long)
Public Declare Sub glBindTexture Lib "opengl32.dll" (ByVal target As Long, ByVal texture As Long)
Public Declare Sub glPrioritizeTextures Lib "opengl32.dll" (ByVal n As Long, textures As Long, priorities As Single)
Public Declare Function glAreTexturesResident Lib "opengl32.dll" (ByVal n As Long, textures As Long, residences As Byte) As Byte
Public Declare Function glIsTexture Lib "opengl32.dll" (ByVal texture As Long) As Byte
' texture mapping
Public Declare Sub glTexSubImage1D Lib "opengl32.dll" (ByVal target As Long, ByVal level As Long, ByVal xoffset As Long, ByVal width As Long, ByVal format As Long, ByVal t As Long, pixels As Any)
Public Declare Sub glTexSubImage2D Lib "opengl32.dll" (ByVal target As Long, ByVal level As Long, ByVal xoffset As Long, ByVal yoffset As Long, ByVal width As Long, ByVal height As Long, ByVal format As Long, ByVal t As Long, pixels As Any)
Public Declare Sub glCopyTexImage1D Lib "opengl32.dll" (ByVal target As Long, ByVal level As Long, ByVal internalFormat As Long, ByVal x As Long, ByVal y As Long, ByVal width As Long, ByVal border As Long)
Public Declare Sub glCopyTexImage2D Lib "opengl32.dll" (ByVal target As Long, ByVal level As Long, ByVal internalFormat As Long, ByVal x As Long, ByVal y As Long, ByVal width As Long, ByVal height As Long, ByVal border As Long)
Public Declare Sub glCopyTexSubImage1D Lib "opengl32.dll" (ByVal target As Long, ByVal level As Long, ByVal xoffset As Long, ByVal x As Long, ByVal y As Long, ByVal width As Long)
Public Declare Sub glCopyTexSubImage2D Lib "opengl32.dll" (ByVal target As Long, ByVal level As Long, ByVal xoffset As Long, ByVal yoffset As Long, ByVal x As Long, ByVal y As Long, ByVal width As Long, ByVal height As Long)
' vertex arrays
Public Declare Sub glVertexPointer Lib "opengl32.dll" (ByVal size As Long, ByVal t As Long, ByVal stride As Long, ptr As Any)
Public Declare Sub glNormalPointer Lib "opengl32.dll" (ByVal t As Long, ByVal stride As Long, ptr As Any)
Public Declare Sub glColorPointer Lib "opengl32.dll" (ByVal size As Long, ByVal t As Long, ByVal stride As Long, ptr As Any)
Public Declare Sub glIndexPointer Lib "opengl32.dll" (ByVal t As Long, ByVal stride As Long, ptr As Any)
Public Declare Sub glTexCoordPointer Lib "opengl32.dll" (ByVal size As Long, ByVal t As Long, ByVal stride As Long, ptr As Any)
Public Declare Sub glEdgeFlagPointer Lib "opengl32.dll" (ByVal stride As Long, ptr As Any)
Public Declare Sub glGetPointerv Lib "opengl32.dll" (ByVal pname As Long, params As Any)
Public Declare Sub glArrayElement Lib "opengl32.dll" (ByVal i As Long)
Public Declare Sub glDrawArrays Lib "opengl32.dll" (ByVal mode As Long, ByVal first As Long, ByVal count As Long)
Public Declare Sub glDrawElements Lib "opengl32.dll" (ByVal mode As Long, ByVal count As Long, ByVal t As Long, indices As Any)
Public Declare Sub glInterleavedArrays Lib "opengl32.dll" (ByVal format As Long, ByVal stride As Long, pointer As Any)
