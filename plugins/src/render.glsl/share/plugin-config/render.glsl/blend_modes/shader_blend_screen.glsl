void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
*****
uniform sampler2D base_t,blend_t;
uniform float base_mult;
uniform float blend_mult;
uniform float alpha;
uniform vec3 base_position;
uniform vec3 blend_position;
uniform vec3 base_scale;
uniform vec3 blend_scale;
const vec4 white = vec4(1.0,1.0,1.0,1.0);
void main()
{
	vec2 base_tc = clamp(gl_TexCoord[0].st*base_scale.xy+base_position.xy,vec2(0.0,0.0),vec2(1.0,1.0));

	vec4 base = vec4(1.0,1.0,1.0,1.0) - texture2D(base_t,base_tc);
	
	vec2 ble_tc = clamp(gl_TexCoord[0].st*blend_scale.xy+blend_position.xy,vec2(0.0,0.0),vec2(1.0,1.0));
	
	vec4 blend = vec4(1.0,1.0,1.0,1.0) - texture2D(blend_t, ble_tc);
	vec4 res = vec4(1.0,1.0,1.0,1.0) - base*blend*blend_mult;
	gl_FragColor = vec4(res.rgb,alpha);
}