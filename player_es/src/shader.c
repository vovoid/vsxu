/*
 * This proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2009 - 2010 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

/*
 * shader.c
 * Functions for loading and process shaders.
 */

#include "shader.h"
#include "cube.h"

/* 
 * Loads the shader source into memory.
 *
 * sFilename: String holding filename to load 
 */
char* load_shader(char *sFilename) {
    char *pResult = NULL;
    FILE *pFile = NULL;
    long iLen = 0;

    pFile = fopen(sFilename, "r");

    if(pFile == NULL) {
        fprintf(stderr, "Error: Cannot read file '%s'\n", sFilename);
  		exit(-1);
    }

    fseek(pFile, 0, SEEK_END); /* Seek end of file */
    iLen = ftell(pFile);
    fseek(pFile, 0, SEEK_SET); /* Seek start of file again */
    pResult = calloc(iLen+1, sizeof(char));
    fread(pResult, sizeof(char), iLen, pFile);
    pResult[iLen] = '\0';
    fclose(pFile);

    return pResult;
}

/* 
 * Create shader, load in source, compile, dump debug as necessary.
 *
 * pShader: Pointer to return created shader ID.
 * sFilename: Passed-in filename from which to load shader source.
 * iShaderType: Passed to GL, e.g. GL_VERTEX_SHADER.
 */
void process_shader(GLuint *pShader, char *sFilename, GLint iShaderType) {
	GLint iStatus;
	const char *aStrings[1] = { NULL };

	/* Create shader and load into GL. */
	*pShader = GL_CHECK(glCreateShader(iShaderType));
	
	aStrings[0] = load_shader(sFilename);
	
	GL_CHECK(glShaderSource(*pShader, 1, aStrings, NULL));

	/* Clean up shader source. */
	free((void *)aStrings[0]);
	aStrings[0] = NULL;

	/* Try compiling the shader. */
	GL_CHECK(glCompileShader(*pShader));
	GL_CHECK(glGetShaderiv(*pShader, GL_COMPILE_STATUS, &iStatus));

	// Dump debug info (source and log) if compilation failed.
	if(iStatus != GL_TRUE) {
#ifdef DEBUG
		GLint iLen;
		char *sDebugSource = NULL;
		char *sErrorLog = NULL;

		/* Get shader source. */
		GL_CHECK(glGetShaderiv(*pShader, GL_SHADER_SOURCE_LENGTH, &iLen));

		sDebugSource = malloc(iLen);

		GL_CHECK(glGetShaderSource(*pShader, iLen, NULL, sDebugSource));
		
		printf("Debug source START:\n%s\nDebug source END\n\n", sDebugSource);
		free(sDebugSource);

		/* Now get the info log. */
		GL_CHECK(glGetShaderiv(*pShader, GL_INFO_LOG_LENGTH, &iLen));
		
		sErrorLog = malloc(iLen);
		
		GL_CHECK(glGetShaderInfoLog(*pShader, iLen, NULL, sErrorLog));
		
		printf("Log START:\n%s\nLog END\n\n", sErrorLog);
		free(sErrorLog);
#endif

		exit(-1);
	}
}

