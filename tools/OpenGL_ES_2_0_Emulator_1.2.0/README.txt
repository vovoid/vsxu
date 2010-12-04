=======================================================================
OpenGL ES 2.0 Emulator v1.2.0 README
=======================================================================

1. REQUIREMENTS

  The OpenGL ES 2.0 Emulator has been tested on the following 
  platforms. ARM recommends these platforms, but the OpenGL ES 2.0
  Emulator may also work with other graphics cards provided that the
  card supports OpenGL 2.x with appropriate extensions. On Linux,
  the graphics drivers must also support GLX 1.4.

  1.1 Windows

    * x86 PC wih Microsoft Windows XP Professional SP3 (32 Bit)
    * NVIDIA GeForce 210 with driver version 190.45
    
  1.2 Linux

    * x86 PC with Ubuntu 10.04 LTS (32 bit)
    * NVIDIA GeForce 210 with driver version 195.36.15

2. BUILDING APPLICATIONS AGAINST THE EMULATOR

  2.1 Windows

    In order to build Windows applications using the OpenGL ES 2.0
    Emulator, ARM recommends Microsoft Visual Studio 2005.

  2.2 Linux

    In order to build Linux applications using the OpenGL ES 2.0
    Emulator, ARM recommends GCC version 4.1.3 and GNU Make version
    3.80 or above.

3. CHANGES FROM PREVIOUS RELEASES

  3.1 Changes from v1.1

    * The emulator now passes Khronos Conformance Tests when using NVIDIA GPUs 
      Note: The Mali GPU Offline Shader Compiler needs to be installed for conformance tests

    * Added support for Online MRI Instrumentation in conjunction
      with the Mali GPU Performance Analysis Tool.

    * Removed the separate instrumented and non-instrumented emulator
      libraries; the single supplied library can be used in 
      instrumented or non-instrumented mode.

  3.2 Changes from v1.0

    * Added support for Linux.

