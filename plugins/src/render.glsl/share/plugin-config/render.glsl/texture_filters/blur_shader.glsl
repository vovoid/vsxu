void main()
{
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_Position = ftransform();
}
*****
uniform sampler2D GlowTexture;
uniform vec2      texOffset;
uniform float     attenuation;
void main(void)
{
  vec4  finalColor = vec4(0.0,0.0,0.0,0.0);

  vec2 offset = vec2(-4.0,-4.0) * (texOffset) + gl_TexCoord[0].xy;
  finalColor += 0.0217 * texture2D(GlowTexture, offset);
  offset += texOffset;
  finalColor += 0.0434 * texture2D(GlowTexture, offset);
  offset += texOffset;
  finalColor += 0.0869 * texture2D(GlowTexture, offset);
  offset += texOffset;
  finalColor += 0.1739 * texture2D(GlowTexture, offset);
  offset += texOffset;
  finalColor += 0.3478 * texture2D(GlowTexture, offset);
  offset += texOffset;
  finalColor += 0.1739 * texture2D(GlowTexture, offset);
  offset += texOffset;
  finalColor += 0.0869 * texture2D(GlowTexture, offset);
  offset += texOffset;
  finalColor += 0.0434 * texture2D(GlowTexture, offset);
  offset += texOffset;
  finalColor += 0.0217 * texture2D(GlowTexture, offset);
  gl_FragColor = finalColor;// * attenuation; 
}
