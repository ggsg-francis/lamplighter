#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in float Height;

uniform vec3 csun;
uniform vec3 camb;
uniform vec3 cfog;
uniform float ffog;

uniform sampler2D texture_diffuse1;

uniform vec3 lightdir = vec3(.75,.6,0.5);

const int indexMatrix4x4old[16] = int[](
	0,8,2,10,
	12,4,14,6,
	3,11,1,9,
	15,7,13,5);

const int indexMatrix4x4[16] = int[](
	1,8,2,10,
	12,4,14,6,
	3,11,1,9,
	14,7,13,5);

// normally the first one is zero
const int indexMatrix8x8[64] = int[](
	0,32,8,40,2,34,10,42,
	48,16,56,24,50,18,58,26,
	12,44,4,36,14,46,6,38,
	60,28,52,20,62,30,54,22,
	3,35,11,43,1,33,9,41,
	51,19,59,27,49,17,57,25,
	15,47,7,39,13,45,5,37,
	63,31,55,23,61,29,53,21);

float dither(float color, float index_value) {
	float closestColor = (color < 0.5) ? 0 : 1;
	float secondClosestColor = 1 - closestColor;
	float distance = abs(closestColor - color);
	return (distance < index_value) ? closestColor : secondClosestColor;
}
	
void main()
{    
	float ndotl = dot(Normal, lightdir);

	vec4 color = texture(texture_diffuse1, TexCoords);
	if (color.a < 0.5)
	{
		discard;
	};
    FragColor = texture(texture_diffuse1, TexCoords);
	
		//FragColor.rgb *= round(Height * 16) / 16;
	//FragColor.rgb *= Height;
	//FragColor.rgb *= clamp(ndotl * 2, 0, 1) * 0.3 + 0.7;
		
	int dx = int(mod(gl_FragCoord.x, 4));
	int dy = int(mod(gl_FragCoord.y, 4));
	float index_value = indexMatrix4x4[(dx + dy * 4)] / 16.0;
	/*	
	int dx = int(mod(gl_FragCoord.x, 8));
	int dy = int(mod(gl_FragCoord.y, 8));
	float index_value = indexMatrix8x8[(dx + dy * 8)] / 16.0;*/
	
	float lightval = dither(clamp(ndotl * 2,0,1), index_value);
	float lightval2 = dither(clamp(ndotl * 2 - 1,0,1), index_value);
		
	//FragColor.a = clamp(FragColor.a * 3 - 1.5, 0, 1);
	
	//make alpha more sharp
	//FragColor.a = clamp(FragColor.a * 2 - 0.5, 0, 1);
	
	//gl_FragDepth = (FragColor.r + FragColor.g + FragColor.b) / 3;
	//gl_FragDepth = -Height + 1;
	
	//FragColor.rgb *= mix(camb, csun, clamp(round(ndotl + 0.5f), 0, 1));
	//FragColor.rgb *= mix(camb, csun, clamp(round(ndotl * 6) / 2, 0, 1));
	//FragColor.rgb *= mix(camb * 0.5, csun, clamp(ndotl + 0.1, 0, 1));
	FragColor.rgb *= mix(camb, csun, (lightval + lightval2) / 2);
	//FragColor.rgb = round(FragColor.rgb * 16) / 16;
	
	//debug
	//FragColor.rgb = vec3(Height * 0.16f);
}