varying vec3 normal;
varying vec3 position;
varying vec3 lightVector0;
varying vec3 lightVector1;
void main()
{
  gl_Position = ftransform();
  gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
  normal = normalize(gl_NormalMatrix * gl_Normal.xyz);
  lightVector0 = normalize(gl_LightSource[0].position.xyz);
  lightVector1 = normalize(gl_LightSource[1].position.xyz);
  position = gl_Position.xyz;
}
*****
uniform sampler2D emit_map;
uniform sampler2D diff_map;
uniform sampler2D spec_map;
uniform samplerCube cube_map;
uniform sampler2D reflection_map;
uniform vec3 CAMERA_POSITION; 
varying vec3 normal;
varying vec3 position;

void main()
{
  vec4 specular = vec4(0.0);
  vec4 diffuse;
  float dist;
  float nxDir;
  vec3 norm = normal; //Important: after interpolation normal modulus != 1.
  //vec3 lightVector0 = gl_LightSource[0].position.xyz;// - position;
  //vec3 lightVector1 = gl_LightSource[1].position.xyz;// - position;

  //dist = length(lightVector0);
  //float attenuation0 = 1.0 / (gl_LightSource[0].constantAttenuation +
                              //gl_LightSource[0].linearAttenuation    * dist +
                              //gl_LightSource[0].quadraticAttenuation * dist * dist);
  //dist = length(lightVector1);
  //float attenuation1 = 1.0 / (gl_LightSource[1].constantAttenuation +
                              //gl_LightSource[1].linearAttenuation    * dist +
                              //gl_LightSource[1].quadraticAttenuation * dist * dist);
  //lightVector0 = normalize(lightVector0);
  //lightVector1 = normalize(lightVector1);
  nxDir = max(0.0, dot(norm, lightVector0));
//  diffuse = vec4(vec3(dot(norm, lightVector0)),1.0);
  diffuse = gl_LightSource[0].diffuse * nxDir * attenuation0;
  vec3 cameraVector = normalize(CAMERA_POSITION - position.xyz);
  if(nxDir != 0.0)
  {
    vec3 halfVector = normalize(lightVector0 + cameraVector);
    float nxHalf = max(0.0,dot(norm, halfVector));
    float specularPower = pow(nxHalf, gl_FrontMaterial.shininess);
    specular = gl_LightSource[0].specular * specularPower * attenuation0;
  }
  nxDir = max(0.0, dot(norm, lightVector1));
//  diffuse = vec4(vec3(dot(norm, lightVector0)),1.0);
  diffuse += gl_LightSource[1].diffuse * nxDir * attenuation1;
  if(nxDir != 0.0)
  {
//    vec3 cameraVector = normalize(CAMERA_POSITION - position.xyz);
    vec3 halfVector = normalize(lightVector1 + cameraVector);
    float nxHalf = max(0.0,dot(norm, halfVector));
    float specularPower = pow(nxHalf, gl_FrontMaterial.shininess);
    specular += gl_LightSource[1].specular * specularPower * attenuation1;
  }
  vec4 diff_color = texture2D(diff_map, gl_TexCoord[0].st);
  vec4 spec_color = texture2D(spec_map, gl_TexCoord[0].st);
  vec4 cube_color = textureCube(cube_map, cross(norm, position));
  vec4 refmap_color = texture2D(reflection_map, gl_TexCoord[0].st);
  gl_FragColor = gl_LightSource[0].ambient * diff_color +
                  texture2D(emit_map, gl_TexCoord[0].st) +
                           (diffuse * diff_color) +
                           (specular * spec_color)+
                           (refmap_color * cube_color);
}
