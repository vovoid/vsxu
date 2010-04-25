--------------------------------------------------------------------------
-- Simple test program testing GLFW and OpenGL from Lua
--------------------------------------------------------------------------


local running = true


-- Key callback function
function keyfun( key, action )
  if action == glfw.RELEASE then
    return
  end

  if key == glfw.KEY_ESC then
    running = false
  end
end


-- Window close callback function
function closefun()
  running = false
  return glfw.TRUE
end


-- Main program and event loop
function main()
    -- Init GLFW
    if glfw.Init() ~= glfw.TRUE then
        return
    end

    -- Open window
    if glfw.OpenWindow( 640,480,0,0,0,0,24,0,glfw.WINDOW ) ~= glfw.TRUE then
        glfw.Terminate()
        return
    end

    -- Set window properties
    glfw.SetWindowTitle( "Hello world!" )
    glfw.Enable( glfw.KEY_REPEAT )
    glfw.SwapInterval( 0 )

    -- Set callback functions
    glfw.SetKeyCallback( "keyfun" );
    glfw.SetWindowCloseCallback( "closefun" );

    while running do
        -- Draw
        gl.ClearColor(0,1,0,0)
        gl.Clear("COLOR_BUFFER_BIT,DEPTH_BUFFER_BIT")

        -- Swap buffers
        glfw.SwapBuffers()
    end

    -- Terminate GLFW
    glfw.Terminate()

    -- Exit program
    return
end


-- Execute!
main()
