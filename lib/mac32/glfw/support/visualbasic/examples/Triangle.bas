Attribute VB_Name = "Program"
'========================================================================
' This is a small test application for GLFW.
' The program opens a window (640x480), and renders a spinning colored
' triangle (it is controlled with both the GLFW timer and the mouse). It
' also calculates the rendering speed (FPS), which is displayed in the
' window title bar.
'========================================================================

Private Sub Main()

  Dim running, frames, Ok As Long
  Dim x, y, width, height As Long
  Dim t0, t, fps As Double
  Dim titlestr As String

  ' Initialize GLFW
  Ok = glfwInit

  ' Open OpenGL window
  Ok = glfwOpenWindow(640, 480, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)
  If Ok = 0 Then
    glfwTerminate
    End
  End If

  ' Enable sticky keys
  glfwEnable (GLFW_STICKY_KEYS)

  ' Disable vertical sync (on cards that support it)
  glfwSwapInterval 0

  ' Main loop
  running = 1
  frames = 0
  t0 = glfwGetTime
  While running = 1
      ' Get time and mouse position
      t = glfwGetTime
      glfwGetMousePos x, y

      ' Calculate and display FPS (frames per second)
      If (t - t0) > 1# Or frames = 0 Then
          fps = frames / (t - t0)
          titlestr = "Spinning Triangle (" + Str(Round(fps, 1)) + " FPS)"
          glfwSetWindowTitle titlestr
          t0 = t
          frames = 0
      End If
      frames = frames + 1

      ' Get window size (may be different than the requested size)
      glfwGetWindowSize width, height
      If height <= 0 Then height = 1

      ' Set viewport
      glViewport 0, 0, width, height

      ' Clear color buffer
      glClearColor 0#, 0#, 0#, 0#
      glClear GL_COLOR_BUFFER_BIT

      ' Select and setup the projection matrix
      glMatrixMode GL_PROJECTION
      glLoadIdentity
      gluPerspective 65#, width / height, 1#, 100#

      ' Select and setup the modelview matrix
      glMatrixMode GL_MODELVIEW
      glLoadIdentity
      gluLookAt 0#, 1#, 0#, 0#, 20#, 0#, 0#, 0#, 1#

      ' Draw a rotating colorful triangle
      glTranslatef 0#, 14#, 0#
      glRotatef 0.3 * x + t * 100#, 0#, 0#, 1#
      glBegin GL_TRIANGLES
          glColor3f 1#, 0#, 0#
          glVertex3f -5#, 0#, -4#
          glColor3f 0#, 1#, 0#
          glVertex3f 5#, 0#, -4#
          glColor3f 0#, 0#, 1#
          glVertex3f 0#, 0#, 6#
      glEnd

      ' Swap buffers
      glfwSwapBuffers

      ' Check if the ESC key was pressed or the window was closed
      If glfwGetKey(GLFW_KEY_ESC) = 1 Or glfwGetWindowParam(GLFW_OPENED) = 0 Then
        running = 0
      End If
  Wend

  ' Close OpenGL window and terminate GLFW
  glfwTerminate

  ' Exit program
  End

End Sub

