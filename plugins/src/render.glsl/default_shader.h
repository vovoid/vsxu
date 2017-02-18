/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#pragma once

/*
const char* default_vert2 = "\
void main()\n\
{\n\
	gl_TexCoord[0] = gl_MultiTexCoord0;\n\
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n\
}\n\
";

const char* default_frag2 = "\
uniform sampler2D base_t,blend_t;\n\
uniform float alpha;\n\
const vec4 white = vec4(1.0,1.0,1.0,1.0);\n\
void main()\n\
{\n\
	vec4 base = texture2D(base_t,gl_TexCoord[0].st);\n\
	vec4 blend = texture2D(blend_t,gl_TexCoord[0].st);\n\
	vec4 res = base / (white - blend);\n\
	gl_FragColor = vec4(res.rgb,alpha);\n\
}\
";
*/
const char* default_vert = "\
vec4 Ambient;\n\
vec4 Diffuse;\n\
vec4 Specular;\n\
\n\
    void pointLight(in int i, in vec3 normal, in vec3 eye, in vec3 ecPosition3)\n\
    {\n\
        float nDotVP;         // normal . light direction\n\
        float nDotHV;         // normal . light half vector\n\
        float pf;                   // power factor\n\
        float attenuation;    // computed attenuation factor\n\
        float d;                    // distance from surface to light source\n\
        vec3  VP;               // direction from surface to light position\n\
        vec3  halfVector;    // direction of maximum highlights\n\
\n\
        // Compute vector from surface to light position\n\
        VP = vec3 (gl_LightSource[i].position) - ecPosition3;\n\
\n\
        // Compute distance between surface and light position\n\
        d = length(VP);\n\
\n\
        // Normalize the vector from surface to light position\n\
        VP = normalize(VP);\n\
\n\
        // Compute attenuation\n\
        attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +\n\
                      gl_LightSource[i].linearAttenuation * d +\n\
                      gl_LightSource[i].quadraticAttenuation * d * d);\n\
        halfVector = normalize(VP + eye);\n\
        nDotVP = max(0.0, dot(normal, VP));\n\
        nDotHV = max(0.0, dot(normal, halfVector));\n\
\n\
        if (nDotVP == 0.0)\n\
            pf = 0.0;\n\
        else\n\
            pf = pow(nDotHV, gl_FrontMaterial.shininess);\n\
\n\
        Ambient  += gl_LightSource[i].ambient * attenuation;\n\
        Diffuse  += gl_LightSource[i].diffuse * nDotVP * attenuation;\n\
        Specular += gl_LightSource[i].specular * pf * attenuation;\n\
    }\n\
\n\
    vec3 fnormal(void)\n\
    {\n\
        //Compute the normal \n\
        vec3 normal = gl_NormalMatrix * gl_Normal;\n\
        normal = normalize(normal);\n\
        return normal;\n\
    }\n\
\n\
    void flight(in vec3 normal, in vec4 ecPosition, float alphaFade)\n\
    {\n\
        vec4 color;\n\
        vec3 ecPosition3;\n\
        vec3 eye;\n\
\n\
        ecPosition3 = (vec3 (ecPosition)) / ecPosition.w;\n\
        eye = vec3 (0.0, 0.0, 1.0);\n\
\n\
        // Clear the light intensity accumulators\n\
        Ambient  = vec4 (0.0);\n\
        Diffuse  = vec4 (0.0);\n\
        Specular = vec4 (0.0);\n\
\n\
       pointLight(0, normal, eye, ecPosition3);\n\
\n\
        color = gl_FrontLightModelProduct.sceneColor +\n\
                    Ambient  * gl_FrontMaterial.ambient +\n\
                    Diffuse  * gl_FrontMaterial.diffuse;\n\
        color += Specular * gl_FrontMaterial.specular;\n\
        color = clamp( color, 0.0, 1.0 );\n\
        gl_FrontColor = color;\n\
        gl_FrontColor.a *= alphaFade;\n\
    }  \n\
\n\
\n\
    void main(void)\n\
    {\n\
        vec3  transformedNormal;\n\
        float alphaFade = 1.0;\n\
\n\
        // Eye-coordinate position of vertex, needed in various calculations\n\
        vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;\n\
\n\
        // Do fixed functionality vertex transform\n\
        gl_Position = ftransform();\n\
        transformedNormal = fnormal();\n\
        flight(transformedNormal, ecPosition, alphaFade);\n\
    }\n\
";

/*shader.vertex_program = "\
void main()\n\
{\n\
  gl_TexCoord[0] = gl_MultiTexCoord0;\n\
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n\
}\n\
";*/

const char* default_frag = "\
    void main (void)\n\
    {\n\
        vec4 color;\n\
\n\
        color = gl_Color;\n\
\n\
        gl_FragColor = color;\n\
    }\n\
";


	
/*
shader.vertex_program = "\
//\n\
// Vertex shader for procedural bricks\n\
//\n\
// Authors: Dave Baldwin, Steve Koren, Randi Rost\n\
//          based on a shader by Darwyn Peachey\n\
//\n\
// Copyright (c) 2002-2004 3Dlabs Inc. Ltd. \n\
//\n\
// See 3Dlabs-License.txt for license information\n\
//\n\
\n\
uniform vec3 LightPosition;\n\
\n\
const float SpecularContribution = 0.3;\n\
const float DiffuseContribution  = 1.0 - SpecularContribution;\n\
\n\
varying float LightIntensity;\n\
varying vec2  MCposition;\n\
\n\
void main(void)\n\
{\n\
    vec3 ecPosition = vec3 (gl_ModelViewMatrix * gl_Vertex);\n\
    vec3 tnorm      = normalize(gl_NormalMatrix * gl_Normal);\n\
    vec3 lightVec   = normalize(LightPosition - ecPosition);\n\
    vec3 reflectVec = reflect(-lightVec, tnorm);\n\
    vec3 viewVec    = normalize(-ecPosition);\n\
    float diffuse   = max(dot(lightVec, tnorm), 0.0);\n\
    float spec      = 0.0;\n\
\n\
    if (diffuse > 0.0)\n\
    {\n\
        spec = max(dot(reflectVec, viewVec), 0.0);\n\
        spec = pow(spec, 16.0);\n\
    }\n\
\n\
    LightIntensity  = DiffuseContribution * diffuse +\n\
                      SpecularContribution * spec;\n\
\n\
    MCposition      = gl_Vertex.xy;\n\
    gl_Position     = ftransform();\n\
}\n\
";

shader.fragment_program = "\
// fooble fuuble\n\
\n\
uniform vec3  BrickColor, MortarColor;\n\
uniform vec2  BrickSize;\n\
uniform vec2  BrickPct;\n\
varying vec2  MCposition;\n\
varying float LightIntensity;\n\
\n\
void main(void)\n\
{\n\
    vec3  color;\n\
    vec2  position, useBrick;\n\
    \n\
    position = MCposition / BrickSize;\n\
\n\
    if (fract(position.y * 0.5) > 0.5)\n\
        position.x += 0.5;\n\
\n\
    position = fract(position);\n\
\n\
    useBrick = step(position, BrickPct);\n\
\n\
    color  = mix(MortarColor, BrickColor, useBrick.x * useBrick.y);\n\
    color *= LightIntensity;\n\
    gl_FragColor = vec4 (color, 1.0);\n\
    \n\
}\n";
*/
