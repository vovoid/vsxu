@ECHO OFF

REM **********************************************************************
REM * This is the build script for the GLFW example programs for MASM32. *
REM *                                                                    *
REM * Unlike the MASM32 recommendations, I like to keep my MASM32        *
REM * installation on one partition, and my GLFW/MASM32 projects on      *
REM * another partition, which is why the script begins by finding which *
REM * drive MASM32 is installed on. Thus, all MASM32 related paths (BIN, *
REM * INCLUDE and LIB) are absolute rather than drive relative.          *
REM *                                                                    *
REM * Feel free to use this build file for your own MASM32 projects (it  *
REM * is only necessary to make minor changes in the Compile section).   *
REM **********************************************************************

REM **********************************************************************
REM * MASM32 is drive sensitive. Figure out where it is installed.       *
REM **********************************************************************
SET masmdrv=
IF EXIST C:\masm32\bin\ml.exe SET masmdrv=C:
IF EXIST D:\masm32\bin\ml.exe SET masmdrv=D:
IF EXIST E:\masm32\bin\ml.exe SET masmdrv=E:
IF EXIST F:\masm32\bin\ml.exe SET masmdrv=F:
IF EXIST G:\masm32\bin\ml.exe SET masmdrv=G:
IF EXIST H:\masm32\bin\ml.exe SET masmdrv=H:
IF NOT "%masmdrv%" == "" GOTO Compile


REM **********************************************************************
REM * Could not find MASM32 installation                                 *
REM **********************************************************************

ECHO ERROR: Could not find MASM32 installation - Aborting.
GOTO Exit


REM **********************************************************************
REM * Compile programs                                                   *
REM **********************************************************************
:Compile

REM *** ASSEMBLER AND LINKER FLAGS (for all examples) ***
SET aflags=/nologo /c /coff /I%masmdrv%\masm32\include
SET lflags=/NOLOGO /SUBSYSTEM:WINDOWS /LIBPATH:%masmdrv%\masm32\lib

REM *** TRIANGLE ***
SET prgname=triangle
%masmdrv%\masm32\bin\ml   %aflags% %prgname%.asm
%masmdrv%\masm32\bin\link %lflags% %prgname%.obj /OUT:%prgname%.exe


REM **********************************************************************
REM * Remove internal working variables                                  *
REM **********************************************************************
:Done

SET masmdrv=
SET aflags=
SET lflags=
SET prgname=


REM **********************************************************************
REM * End of batch file                                                  *
REM **********************************************************************
:Exit
