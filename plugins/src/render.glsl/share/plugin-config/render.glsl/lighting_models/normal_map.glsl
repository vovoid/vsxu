varying vec3 lightVec; 
varying vec3 eyeVec;
varying vec2 texCoord;
attribute vec3 vTangent; 

void main(void)
{
  gl_Position = ftransform();
  texCoord = gl_MultiTexCoord0.xy;

  vec3 n = normalize(gl_NormalMatrix * gl_Normal);
  vec3 t = normalize(gl_NormalMatrix * vTangent);
  vec3 b = cross(n, t);

  vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
  vec3 tmpVec = gl_LightSource[0].position.xyz - vVertex;

  lightVec.x = dot(tmpVec, t);
  lightVec.y = dot(tmpVec, b);
  lightVec.z = dot(tmpVec, n);

  tmpVec = -vVertex;
  eyeVec.x = dot(tmpVec, t);
  eyeVec.y = dot(tmpVec, b);
  eyeVec.z = dot(tmpVec, n);
}
*****
varying vec3 lightVec;
varying vec3 eyeVec;
varying vec2 texCoord;
uniform sampler2D diff_map;
uniform sampler2D normal_map;
uniform sampler2D specular_map;
uniform sampler2D emission_map;
uniform sampler2D reflection_map;
uniform samplerCube cube_map;
uniform float invRadius;

void main (void)
{
float distSqr = dot(lightVec, lightVec);
float att = clamp(1.0 - invRadius * sqrt(distSqr), 0.0, 1.0);
vec3 lVec = lightVec * inversesqrt(distSqr);

vec3 vVec = normalize(eyeVec);

vec4 base = texture2D(diff_map, texCoord);

vec3 bump = normalize( texture2D(normal_map, texCoord).xyz * 2.0 - 1.0);

vec4 vAmbient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;

float diffuse = max( dot(lVec, bump), 0.0 );

vec4 vDiffuse = gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * diffuse;
vec4 spec_col = texture2D(specular_map, texCoord);
float specular = pow(clamp(dot(reflect(-lVec, bump), vVec), 0.0, 1.0), spec_col.x * gl_FrontMaterial.shininess );
vec4 vSpecular = spec_col * gl_LightSource[0].specular * gl_FrontMaterial.specular * specular;
vec4 cube_color = textureCube(cube_map, cross(bump, eyeVec));
vec4 refmap_color = texture2D(reflection_map, texCoord);

gl_FragColor = ( vAmbient*base + vDiffuse*base + vSpecular + refmap_color * cube_color + texture2D(emission_map, texCoord)) * att;
}