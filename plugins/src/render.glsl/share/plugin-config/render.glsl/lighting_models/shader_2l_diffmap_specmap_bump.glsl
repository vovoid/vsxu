varying vec3 normal;
varying vec3 position;
varying vec3 tangent;
//varying vec3 lightVector0;
//varying vec3 lightVector1;
//varying float nxDir0;
//varying float nxDir1;
//attribute vec3 tangent_attrib;
void main()
{
  gl_Position = ftransform();
  gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
  normal = normalize(gl_NormalMatrix * gl_Normal.xyz);
  position = gl_Position.xyz;
  //lightVector0 = normalize(gl_LightSource[0].position.xyz);
  //lightVector1 = normalize(gl_LightSource[1].position.xyz);
//  nxDir0 = max(0.0, dot(normal, lightVector0));
//  nxDir1 = max(0.0, dot(normal, lightVector1));
 //tangent = tangent_attrib;
}
*****
uniform sampler2D emit_map;
uniform sampler2D diff_map;
uniform sampler2D spec_map;
uniform samplerCube cube_map;
uniform sampler2D reflection_map;
uniform sampler2D normal_map;
uniform vec3 CAMERA_POSITION;
varying vec3 normal;
varying vec3 position;
//varying vec3 tangent;
uniform vec3 lightVector0;
uniform vec3 lightVector1;
//varying float nxDir0;
//varying float nxDir1;

void main()
{
  vec4 specular;// = vec4(0.0);
  
  //float dist;
  float nxDir;
  // = normal; //Important: after interpolation normal modulus != 1.
  //vec3 lightVector0 = normalize(gl_LightSource[0].position.xyz);// - position;
  //vec3 lightVector1 = normalize(gl_LightSource[1].position.xyz);// - position;
  vec3 norm = normalize(reflect(normal, (vec3(-0.5,-0.5,-0.5) + vec3(texture2D(normal_map, gl_TexCoord[0].st)))));
  //lightVector1 = reflect(lightVector1, normalize(texture2D(normal_map, gl_TexCoord[0].st)));
  //dist = length(lightVector0);
  //float attenuation0 = 1.0 / (gl_LightSource[0].constantAttenuation +
                              //gl_LightSource[0].linearAttenuation    +
                              //gl_LightSource[0].quadraticAttenuation);
  //dist = length(lightVector1);
  //float attenuation1 = 1.0 / (gl_LightSource[1].constantAttenuation +
                              //gl_LightSource[1].linearAttenuation   +
                              //gl_LightSource[1].quadraticAttenuation );
  nxDir = max(0.0, dot(norm, lightVector0));
//  diffuse = vec4(vec3(dot(norm, lightVector0)),1.0);
  vec4 diffuse = gl_LightSource[0].diffuse * nxDir;
  vec3 cameraVector = normalize(CAMERA_POSITION - position.xyz);
  if(nxDir != 0.0)
  {
    vec3 halfVector = normalize(lightVector0 + cameraVector);
    float nxHalf = max(0.0,dot(norm, halfVector));
    float specularPower = pow(nxHalf, gl_FrontMaterial.shininess);
    specular = gl_LightSource[0].specular * specularPower;
  }
  nxDir = max(0.0, dot(norm, lightVector1));
//  diffuse = vec4(vec3(dot(norm, lightVector0)),1.0);
  diffuse += gl_LightSource[1].diffuse * nxDir;
  if(nxDir != 0.0)
  {
    vec3 halfVector = normalize(lightVector1 + cameraVector);
    float nxHalf = max(0.0,dot(norm, halfVector));
    float specularPower = pow(nxHalf, gl_FrontMaterial.shininess);
    specular += gl_LightSource[1].specular * specularPower;
  }
  vec4 diff_color = texture2D(diff_map, gl_TexCoord[0].st);
  vec4 spec_color = texture2D(spec_map, gl_TexCoord[0].st);
  vec4 cube_color = textureCube(cube_map, cross(norm, CAMERA_POSITION - position)));
  vec4 refmap_color = texture2D(reflection_map, gl_TexCoord[0].st);
  gl_FragColor = gl_LightSource[0].ambient * diff_color +
                  texture2D(emit_map, gl_TexCoord[0].st) +
                           (diffuse * diff_color) +
                           (specular * spec_color)+
                           (refmap_color * cube_color);
}
