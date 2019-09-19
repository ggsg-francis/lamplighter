#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in float Height;

uniform uint id; // identity
uniform bool idn; // id null
uniform vec3 csun;
uniform vec3 camb;
uniform vec3 cfog;
uniform float ffog;
uniform vec3 pcam;
uniform float ft; // Time

uniform sampler2D texture_diffuse1;
uniform sampler2D tlm; // texture lightmap
//uniform sampler2D thm; // texture heightmap

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
	
const int indexMat4x4PSX[16] = int[]
(
	-4,	0,	-3,	1,
	2,	-2,	3,	-1,
	-3,	1,	-4,	0,
	3,	-1,	2,	-2
);

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
	if (gl_FragCoord.x > 319 && gl_FragCoord.x < 321 && gl_FragCoord.y > 239 && gl_FragCoord.y < 241)
	{
		if (idn)
		{
			FragColor.r = 0;
			FragColor.g = 0;
			FragColor.b = 1;
		}
		else
		{
			FragColor.r = float(id) / float(256);
			FragColor.g = float(id >> 8) / float(256);
			FragColor.b = 0;
		}
	}
	else
	{
		float ndotl = dot(Normal, lightdir);

		//FragColor = texture(texture_diffuse1, TexCoords);
		FragColor = texture(texture_diffuse1, vec2(TexCoords.x + (ft), TexCoords.y));
	}
}
