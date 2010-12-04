/*
 * This proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2009 - 2010 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

#ifndef SHADER_H
#define SHADER_H

#include "GLES2/gl2.h"

char* load_shader(char *sFilename);
void process_shader(GLuint *pShader, char *sFilename, GLint iShaderType);

#endif

