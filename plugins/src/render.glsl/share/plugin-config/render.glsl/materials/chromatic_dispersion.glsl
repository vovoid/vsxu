// courtesy of Thomas Kerwin

varying vec3 N;
varying vec3 View;
varying vec3 ScreenPos;
varying vec3 ecPosition3;


void main() {

  ecPosition3 = vec3(gl_ModelViewMatrix * gl_Vertex);
  View = normalize(-ecPosition3);
  N = normalize( gl_NormalMatrix * gl_Normal);
  gl_TexCoord[0] = 0.6 * vec4(gl_Normal,1);
  gl_Position = ftransform();
  ScreenPos = vec3(gl_Position);
}
*****
varying vec3 N;
varying vec3 View;

uniform sampler2D Texture;
uniform samplerCube Environment;
uniform float refraction_index;

void myRefract(in vec3 incom, in vec3 normal, in float index_external, in float index_internal,
        out vec3 reflection, out vec3 refraction,
        out float reflectance, out float transmittance) 
{
        
  float eta = index_external/index_internal;
  float cos_theta1 = dot(incom, normal);
  float cos_theta2 = sqrt(1.0 - ((eta * eta) * ( 1.0 - (cos_theta1 * cos_theta1))));
  reflection = incom - 2.0 * cos_theta1 * normal;
  refraction = (eta * incom) + (cos_theta2 - eta * cos_theta1) * normal;

  float fresnel_rs = (index_external * cos_theta1 - index_internal * cos_theta2 ) /
    (index_external * cos_theta1 + index_internal * cos_theta2);


  float fresnel_rp = (index_internal * cos_theta1 - index_external * cos_theta2 ) /
      (index_internal * cos_theta1 + index_external * cos_theta2);

  reflectance =  (fresnel_rs * fresnel_rs + fresnel_rp * fresnel_rp) / 2.0;
  transmittance =  ((1.0-fresnel_rs) * (1.0-fresnel_rs) + (1.0-fresnel_rp) * (1.0-fresnel_rp)) / 2.0;
}

void main() {


  
  vec3 nN = 0.5 * (N + 2.0*(texture2D(Texture, gl_TexCoord[0].xy).rgb - 0.5));
  
  vec3 refraction_ray_r, reflection_ray_r;
  vec3 refraction_ray_g, reflection_ray_g;
  vec3 refraction_ray_b, reflection_ray_b;
  float fresnel_R, fresnel_T;
  
  myRefract(View, nN, 1.0, refraction_index,
    reflection_ray_r, refraction_ray_r, fresnel_R, fresnel_T);

  myRefract(View, nN, 1.0, refraction_index + 0.1,
    reflection_ray_g, refraction_ray_g, fresnel_R, fresnel_T);

  myRefract(View, nN, 1.0, refraction_index + 0.2,
    reflection_ray_b, refraction_ray_b, fresnel_R, fresnel_T);


  refraction_ray_r = (gl_ModelViewMatrix * vec4(refraction_ray_r,0.0)).xyz;
  reflection_ray_r = (gl_ModelViewMatrix * vec4(reflection_ray_r,0.0)).xyz;
  refraction_ray_g = (gl_ModelViewMatrix * vec4(refraction_ray_g,0.0)).xyz;
  reflection_ray_g = (gl_ModelViewMatrix * vec4(reflection_ray_g,0.0)).xyz;
  refraction_ray_b = (gl_ModelViewMatrix * vec4(refraction_ray_b,0.0)).xyz;
  reflection_ray_b = (gl_ModelViewMatrix * vec4(reflection_ray_b,0.0)).xyz;


  vec4 reflect_color = vec4(textureCube(Environment, reflection_ray_r).r, textureCube(Environment, reflection_ray_g).g, textureCube(Environment, reflection_ray_b).b, 1);
  vec4 refract_color = vec4(textureCube(Environment, refraction_ray_r).r, textureCube(Environment, refraction_ray_g).g, textureCube(Environment, refraction_ray_b).b, 1);
  
  fresnel_T = fresnel_T * 0.5;
  fresnel_R = fresnel_R * 0.5;

  gl_FragColor = (reflect_color * fresnel_R + refract_color * fresnel_T) ;

}

