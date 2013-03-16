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
uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform float invRadius;

void main (void)
{
  float distSqr = dot(lightVec, lightVec);
  float att = clamp(1.0 - invRadius * sqrt(distSqr), 0.0, 1.0);
  vec3 lVec = lightVec * inversesqrt(distSqr);

  vec3 vVec = normalize(eyeVec);

  vec4 base = texture2D(colorMap, texCoord);

  vec3 bump = normalize( texture2D(normalMap, texCoord).xyz * 2.0 - 1.0);

  vec4 vAmbient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;

  float diffuse = max( dot(lVec, bump), 0.0 );

  vec4 vDiffuse = gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * 
                diffuse;

  float specular = pow(clamp(dot(reflect(-lVec, bump), vVec), 0.0, 1.0), 
                   gl_FrontMaterial.shininess );

  vec4 vSpecular = gl_LightSource[0].specular * gl_FrontMaterial.specular * 
                  specular;

  gl_FragColor = ( vAmbient*base + 
                   vDiffuse*base + 
                   vSpecular) * att;
}

