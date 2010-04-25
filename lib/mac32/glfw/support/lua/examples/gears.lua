--------------------------------------------------------------------------
-- 3-D gear wheels.  This program is in the public domain.
--
-- Command line options:
--    -info      print GL implementation information
--    -exit      automatically exit after 30 seconds
--
--
-- Brian Paul
--
--
-- Marcus Geelnard:
--   - Conversion to GLFW
--   - Time based rendering (frame rate independent)
--   - Slightly modified camera that should work better for stereo viewing
--   - Ported to Lua (thanks to David Medlock for the original Lua port)
--------------------------------------------------------------------------

local t0 = 0.0
local t = 0
local dt = 0

local Frames = 0
local autoexit = 0


--------------------------------------------------------------------------
--  Draw a gear wheel.  You'll probably want to call this function when
--  building a display list since we do a lot of trig here.
--
--  Input:  inner_radius - radius of hole at center
--          outer_radius - radius at center of teeth
--          width - width of gear
--          teeth - number of teeth
--          tooth_depth - depth of tooth
--------------------------------------------------------------------------
local function gear(inner_radius, outer_radius, width, teeth, tooth_depth)

  local PI = 3.141592654
  local r0 = inner_radius
  local r1 = outer_radius - ( tooth_depth / 2.0 )
  local r2 = outer_radius + ( tooth_depth / 2.0 )
  local da = 2.0 * PI / teeth / 4.0

  gl.ShadeModel("FLAT")

  gl.Normal(0.0, 0.0, 1.0)

  local cos = math.cos;
  local sin = math.sin;
  local sqrt = math.sqrt;

  -- draw front face
  gl.Begin("QUAD_STRIP");
  for i = 0, teeth do
    angle = i * 2.0 * PI / teeth
    gl.Vertex(r0 * cos(angle), r0 * sin(angle), width * 0.5)
    gl.Vertex(r1 * cos(angle), r1 * sin(angle), width * 0.5)
    if (i < teeth) then
      gl.Vertex(r0 * cos(angle), r0 * sin(angle), width * 0.5)
      gl.Vertex(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5)
    end
  end
  gl.End()

  -- draw front sides of teeth
  gl.Begin("QUADS")
  da = 2.0 * PI / teeth / 4.0;
  for i = 0,teeth-1 do
    angle = i * 2.0 * PI / teeth

    gl.Vertex(r1 * cos(angle), r1 * sin(angle), width * 0.5)
    gl.Vertex(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5)
    gl.Vertex(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5)
    gl.Vertex(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5)
  end
  gl.End()

  gl.Normal(0.0, 0.0, -1.0)

  -- draw back face
  gl.Begin("QUAD_STRIP")
  for i = 0,teeth do
    angle = i * 2.0 * PI / teeth
    gl.Vertex(r1 * cos(angle), r1 * sin(angle), -width * 0.5)
    gl.Vertex(r0 * cos(angle), r0 * sin(angle), -width * 0.5)
    if (i < teeth) then
      gl.Vertex(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5)
      gl.Vertex(r0 * cos(angle), r0 * sin(angle), -width * 0.5)
    end
  end
  gl.End()

  -- draw back sides of teeth
  gl.Begin("QUADS")
  da = 2.0 * PI / teeth / 4.0
  for i = 0, teeth-1 do
    angle = i * 2.0 * PI / teeth

    gl.Vertex(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5)
    gl.Vertex(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5)
    gl.Vertex(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5)
    gl.Vertex(r1 * cos(angle), r1 * sin(angle), -width * 0.5)
  end
  gl.End()


  -- draw outward faces of teeth
  gl.Begin("QUAD_STRIP")
  for i = 0, teeth-1 do
    angle = i * 2.0 * PI / teeth

    gl.Vertex(r1 * cos(angle), r1 * sin(angle), width * 0.5)
    gl.Vertex(r1 * cos(angle), r1 * sin(angle), -width * 0.5)
    u = r2 * cos(angle + da) - r1 * cos(angle)
    v = r2 * sin(angle + da) - r1 * sin(angle)
    len = sqrt(u * u + v * v)
    u = u / len
    v = v / len
    gl.Normal(v, -u, 0.0)
    gl.Vertex(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5)
    gl.Vertex(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5)
    gl.Normal(cos(angle), sin(angle), 0.0)
    gl.Vertex(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5)
    gl.Vertex(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5)
    u = r1 * cos(angle + 3 * da) - r2 * cos(angle + 2 * da)
    v = r1 * sin(angle + 3 * da) - r2 * sin(angle + 2 * da)
    gl.Normal(v, -u, 0.0)
    gl.Vertex(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5)
    gl.Vertex(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5)
    gl.Normal(cos(angle), sin(angle), 0.0)
  end

  gl.Vertex(r1 * cos(0), r1 * sin(0), width * 0.5)
  gl.Vertex(r1 * cos(0), r1 * sin(0), -width * 0.5)

  gl.End()

  gl.ShadeModel("SMOOTH");

  -- draw inside radius cylinder
  gl.Begin("QUAD_STRIP")
  for i = 0,teeth do
    angle = i * 2.0 * PI / teeth
    gl.Normal(-cos(angle), -sin(angle), 0.0)
    gl.Vertex(r0 * cos(angle), r0 * sin(angle), -width * 0.5)
    gl.Vertex(r0 * cos(angle), r0 * sin(angle), width * 0.5)
  end
  gl.End()

end

local running = true
local view = { rotx = 20.0 ; roty = 30.0 ; rotz = 0.0 }
local gear1, gear2, gear3
local angle = 0.0


--------------------------------------------------------------------------
-- OpenGL draw function & timing
--------------------------------------------------------------------------
local function draw()
  gl.Clear("DEPTH_BUFFER_BIT,COLOR_BUFFER_BIT")

    gl.MatrixMode( "MODELVIEW" );

  gl.PushMatrix()
    gl.Rotate(view.rotx, 1.0, 0.0, 0.0)
    gl.Rotate(view.roty, 0.0, 1.0, 0.0)
    gl.Rotate(view.rotz, 0.0, 0.0, 1.0)

    gl.PushMatrix()
      gl.Translate(-3.0, -2.0, 0.0)
      gl.Rotate(angle, 0.0, 0.0, 1.0)
      gl.CallList(gear1)
    gl.PopMatrix()

    gl.PushMatrix()
      gl.Translate(3.1, -2.0, 0.0)
      gl.Rotate(-2.0 * angle - 9.0, 0.0, 0.0, 1.0)
      gl.CallList(gear2)
    gl.PopMatrix()

    gl.PushMatrix()
      gl.Translate(-3.1, 4.2, 0.0)
      gl.Rotate(-2.0 * angle - 25.0, 0.0, 0.0, 1.0)
      gl.CallList(gear3)
    gl.PopMatrix()

  gl.PopMatrix()

  Frames = Frames + 1;

  do
     t_new = glfw.GetTime()
     dt = t_new - t;
     t = t_new;

     if (t - t0 >= 5.0) then
        seconds = t - t0;
        fps = Frames / seconds;
        print(Frames, " frames in ", seconds, " seconds = ", fps, " FPS")
        t0 = t;
        Frames = 0;
        if (t >= 0.999 * autoexit) and autoexit then running = 0 end
     end
  end
end


--------------------------------------------------------------------------
-- Initialize OpenGL
--------------------------------------------------------------------------
local function init(args)
  pos = {5.0, 5.0, 10.0, 0.0}
  red = {0.8, 0.1, 0.0, 1.0}
  green = {0.0, 0.8, 0.2, 1.0}
  blue = {0.2, 0.2, 1.0, 1.0}

  gl.Light("LIGHT0", "POSITION", pos)
  gl.Enable("CULL_FACE")
  gl.Enable("LIGHTING")
  gl.Enable("LIGHT0")
  gl.Enable("DEPTH_TEST")
  gl.DepthFunc("LEQUAL")

  -- make the gears
  gear1 = gl.GenLists(1)
  gl.NewList(gear1, "COMPILE");
  gl.Material("FRONT", "AMBIENT_AND_DIFFUSE", red);
  gear(1.0, 4.0, 1.0, 20, 0.7);
  gl.EndList()

  gear2 = gl.GenLists(1)
  gl.NewList(gear2, "COMPILE")
  gl.Material("FRONT", "AMBIENT_AND_DIFFUSE", green)
  gear(0.5, 2.0, 2.0, 10, 0.7)
  gl.EndList()

  gear3 = gl.GenLists(1);
  gl.NewList(gear3, "COMPILE")
  gl.Material("FRONT", "AMBIENT_AND_DIFFUSE", blue)
  gear(1.3, 2.0, 0.5, 10, 0.7)
  gl.EndList()

  gl.Enable("NORMALIZE")

  if args then
    for parm in args do
      if parm == "-exit" then
        autoexit = 30
        print("Auto Exit after ", autoexit ," seconds."  );
      end
    end
  end
end


--------------------------------------------------------------------------
-- update animation parameters
--------------------------------------------------------------------------
local function animate()
  angle = angle + 100.0*dt
end


--------------------------------------------------------------------------
-- new window size
--------------------------------------------------------------------------
function reshape( width, height )
  local h = height / width

  local znear = 5.0
  local zfar  = 30.0
  local xmax  = znear * 0.5

  gl.Viewport( 0, 0, width, height )
  gl.MatrixMode( "PROJECTION" );
  gl.LoadIdentity()
  gl.Frustum( -xmax, xmax, -xmax*h, xmax*h, znear, zfar )
  gl.MatrixMode( "MODELVIEW" );
  gl.LoadIdentity()
  gl.Translate( 0.0, 0.0, -20.0 )
  gl.Enable( "DEPTH_TEST" );
end


--------------------------------------------------------------------------
-- change view angle, exit upon ESC
--------------------------------------------------------------------------
function key( k, action )
  if action ~= glfw.PRESS then return end

  -- 'Z'?
  if k == 90 then
    if glfw.GetKey( glfw.KEY_LSHIFT ) == glfw.PRESS then
      view.rotz = view.rotz - 5.0
    else
      view.rotz = view.rotz + 5.0
    end
    return
  end

  if k == glfw.KEY_ESC then
    running = false
    return
  end

  if k == glfw.KEY_UP then
    view.rotx = view.rotx + 5.0
    return
  end

  if k == glfw.KEY_DOWN then
    view.rotx = view.rotx - 5.0
    return
  end

  if k == glfw.KEY_LEFT then
    view.roty = view.roty + 5.0
    return
  end

  if k == glfw.KEY_RIGHT then
    view.roty = view.roty - 5.0
    return
  end
end


--------------------------------------------------------------------------
-- main()
--------------------------------------------------------------------------
local function main()
    -- Init GLFW
    if glfw.Init() ~= glfw.TRUE then
        return
    end

    -- Open window
    if glfw.OpenWindow( 640,480,0,0,0,0,24,0,glfw.WINDOW ) ~= glfw.TRUE then
        glfw.Terminate()
        return
    end

    glfw.SetWindowTitle( "Gears" );
    glfw.Enable( glfw.KEY_REPEAT );
    glfw.SwapInterval( 0 );

    -- Special args?
    init(args);

    -- Set callback functions
    glfw.SetWindowSizeCallback( "reshape" );
    glfw.SetKeyCallback( "key" );

    while running do
        -- Draw gears
        draw()

        -- Update animation
        animate()

        -- Swap buffers
        glfw.SwapBuffers()

        -- Was the window closed?
        if glfw.GetWindowParam( glfw.OPENED ) ~= glfw.TRUE then
          running = false
        end

    end

    -- Terminate GLFW
    glfw.Terminate()

    -- Exit program
    return
end


-- execute!
main()
