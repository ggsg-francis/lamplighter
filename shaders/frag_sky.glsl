#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec4 Col;

uniform uint id; // identity
uniform bool idn; // id null
uniform vec3 pcam;

uniform float ft; // Time

uniform sampler2D texture_diffuse1;
uniform sampler2D tlm; // texture lightmap

uniform vec3 vsun = normalize(vec3(-1,1,-1));

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
		float ndotl = dot(Normal, vsun);

		//FragColor = texture(texture_diffuse1, TexCoords);
		//FragColor = texture(texture_diffuse1, vec2(TexCoords.x + (ft), TexCoords.y));
		
		vec3 vd = normalize(Pos - pcam);
		
		/*
		FragColor = texture(texture_diffuse1, vec2(
				ft + dot(vd, vsun) * 0.01, // X Coord
				dot(vd, vec3(0,-1,0)) * 0.5 + 0.5
			)); // Y Coord
		*/
		/*
		FragColor = texture(texture_diffuse1, vec2(ft,
				(dot(vd, vec3(0,-1,0)) * 0.5 + (0.5 - 0.06)) + (dot(vd, vsun) * 0.06)
			));
		*/
		
		FragColor = Col;
				
		// Draw Sun
		FragColor += vec4(clamp(round(dot(vd, vsun) * 512 - 511), 0, 1));
		// Draw Sun Halo
		//vec3 suncol = texture(texture_diffuse1, vec2(ft, 30.5f / 32.f)).rgb * 2.f;
		//FragColor.rgb += suncol * clamp(dot(vd, vsun) * 0.5 - 0.25, 0, 2);
		
		// Dither
		/*
		int dx = int(mod(gl_FragCoord.x, 4));
		int dy = int(mod(gl_FragCoord.y, 4));
		float rndBy = 12.f;
		FragColor.rgb += indexMat4x4PSX[(dx + dy * 4)] / (rndBy * 4.f);
		// Posterize
		FragColor.rgb = round(FragColor.rgb * rndBy) / rndBy;
		*/
	}
}
