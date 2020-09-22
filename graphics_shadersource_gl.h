#define SSRC_VERSION R"(
#version 330 core
)"

//________________________________________________________________________________________________________________________________
// SHADER VARIABLES --------------------------------------------------------------------------------------------------------------

#define SSRC_VAR_FRAG_3D R"(
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec4 Col;
in vec4 LightSpacePos;
in vec3 LC; // Light Colour

uniform uint id; // identity
uniform bool idn; // id null
uniform vec3 pcam;

uniform float ft; // Time

uniform sampler2D texture_diffuse1;
uniform sampler2D tlm; // texture lightmap
uniform sampler2D thm; // texture heightmap
uniform sampler2D ts; // texture sky
uniform sampler2D tshadow; // texture shadow

uniform vec3 vsun = normalize(vec3(-1, 1, -1));
uniform vec3 cFog = vec3(0.1, 0.1, 0.1);
uniform float fFogDens = 0.f;

uniform bool lit = true;

// lower value = more blending
// Standard
//const float shadowDepthBlend = 2048.f;
// smooth
const float shadowDepthBlend = 512.f;
)"

#define SHADER_VAR_FRAG_3D_CHARA R"(
uniform vec3 c_a = vec3(112. / 256., 103. / 256., 85.  / 256.);
uniform vec3 c_b = vec3(178. / 256., 107. / 256., 22.  / 256.);
uniform vec3 c_c = vec3(152. / 256., 144. / 256., 127. / 256.);
)"

#define SSRC_VAR_FRAG_3D_MEAT R"(
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec4 Col;
in vec4 LightSpacePos;
in vec4 ViewSpacePos;

uniform uint id; // identity
uniform bool idn; // id null
uniform vec3 pcam;

uniform float ft; // Time

uniform sampler2D texture_diffuse1;
uniform sampler2D tlm; // texture lightmap
uniform sampler2D thm; // texture heightmap
uniform sampler2D ts; // texture sky
uniform sampler2D tshadow; // texture shadow

uniform vec3 vsun = normalize(vec3(-1, 1, -1));

uniform bool lit = true;

const float scalar = 100.f;

// lower value = more blending
// Standard
//const float shadowDepthBlend = 2048.f;
// smooth
const float shadowDepthBlend = 512.f;

const int indexMat4x4PSX[16] = int[]
(
	-4,	0,	-3,	1,
	2,	-2,	3,	-1,
	-3,	1,	-4,	0,
	3,	-1,	2,	-2
);
)"

#define SSRC_VAR_FRAG_3D_TERRAIN R"(
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec4 Col;
in float TBlend[8];
in vec4 LightSpacePos;
in vec3 LC; // Light Colour

uniform uint id; // identity
uniform bool idn; // id null
uniform vec3 pcam;

uniform float ft; // Time

uniform sampler2D tt1;
uniform sampler2D tt2;
uniform sampler2D tt3;
uniform sampler2D tt4;
uniform sampler2D tt5;
uniform sampler2D tt6;
uniform sampler2D tt7;
uniform sampler2D tt8;
uniform sampler2D tlm; // texture lightmap
uniform sampler2D thm; // texture heightmap
uniform sampler2D ts; // texture sky
uniform sampler2D tshadow; // texture shadow

uniform vec3 vsun = normalize(vec3(-1, 1, -1));
uniform vec3 cFog = vec3(0.1, 0.1, 0.1);
uniform float fFogDens = 0.f;

uniform bool lit = true;

// lower value = more blending
// Standard
//const float shadowDepthBlend = 2048.f;
// smooth
const float shadowDepthBlend = 512.f;
)"


//________________________________________________________________________________________________________________________________
// SHADER FUNCTIONS --------------------------------------------------------------------------------------------------------------

#define SHADER_FNC_DITHER R"(
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
)"

#define SSRC_FNC_SHADOW R"(
float GetShadowBilinear(vec3 projCoords, ivec2 tsize, vec2 texelSize) {
	vec2 nearestpoint = (round(projCoords.xy * tsize.x) - 0.5) / tsize.x; // also seems to work, for some reason
	vec2 offset = (projCoords.xy - nearestpoint) / texelSize.x; // known to work
	return mix( // X1xX2 x X3xX4 (Y lerp)
		mix( // X1, X2 Lerp
			1 - clamp((projCoords.z - texture(tshadow, nearestpoint).r) * shadowDepthBlend, 0.0, 1.0),                          // Shadow A
			1 - clamp((projCoords.z - texture(tshadow, nearestpoint + vec2(1, 0) * texelSize).r) * shadowDepthBlend, 0.0, 1.0), // Shadow B
			abs(offset.x)),
		mix( // X3, X4 Lerp
			1 - clamp((projCoords.z - texture(tshadow, nearestpoint + vec2(0, 1) * texelSize).r) * shadowDepthBlend, 0.0, 1.0), // Shadow C
			1 - clamp((projCoords.z - texture(tshadow, nearestpoint + vec2(1, 1) * texelSize).r) * shadowDepthBlend, 0.0, 1.0), // Shadow D
			abs(offset.x)),
		abs(offset.y));
}
float ShadowCalculation(vec4 fragPosLightSpace) {
	float shadow = 0.0;
	if (length(Pos - pcam) < 12.f)
	{
		// perform perspective divide
		vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
		// transform to [0,1] range (coord Z represents current fragment from light's perspective)
		projCoords = projCoords * 0.5 + 0.5;

		float currentDepth = projCoords.z;

		// texture parameters
		ivec2 tsize = textureSize(tshadow, 0);
		vec2 texelSize = 1.0 / textureSize(tshadow, 0);

		// circle pattern
		float offsetamt = 1.f * texelSize.x;
		float offsetamtdiag = 0.707107f * texelSize.x;
		shadow += GetShadowBilinear(projCoords + vec3(offsetamtdiag, offsetamtdiag, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(offsetamtdiag, -offsetamtdiag, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(-offsetamtdiag, offsetamtdiag, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(-offsetamtdiag, -offsetamtdiag, 0.f), tsize, texelSize);
		if (shadow < 3.5f && shadow > 0.5f)
		{
			shadow += GetShadowBilinear(projCoords + vec3(offsetamt, 0.f, 0.f), tsize, texelSize);
			shadow += GetShadowBilinear(projCoords + vec3(0.f, offsetamt, 0.f), tsize, texelSize);
			shadow += GetShadowBilinear(projCoords + vec3(-offsetamt, 0.f, 0.f), tsize, texelSize);
			shadow += GetShadowBilinear(projCoords + vec3(0.f, -offsetamt, 0.f), tsize, texelSize);
			shadow += GetShadowBilinear(projCoords, tsize, texelSize);
			shadow /= 9;
		}
		else
		{
			shadow += GetShadowBilinear(projCoords, tsize, texelSize);
			shadow /= 5;
		}

		// Blend into clip distance
		shadow += 1 - clamp((12.f - length(Pos - pcam)) / 6.f, 0, 1);
	}
	else shadow = 1.f;
	//else shadow = 0.f; // For debugging

	return shadow;
}
)"

//________________________________________________________________________________________________________________________________
// FRAMEBUFFER -------------------------------------------------------------------------------------------------------------------

#define SHADER_VERT_FRAMEBUFFER R"(
#version 330 core

layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_uv;

out vec2 TexCoords;

uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(in_pos.x, in_pos.y, 0.0, 1.0);
    TexCoords = in_uv;
}
)"

#if DEF_PROJECT == DEF_PROJECT_EXPLORE
#define SHADER_FRAG_FRAMEBUFFER R"(
#version 330 core

in vec2 TexCoords;
  
out vec4 FragColor;

uniform sampler2D uf_txtr;
uniform float wx = 640.f;
uniform float wy = 480.f;

vec3 BloomAdd(float rndsz, float fmodx, float fmody, float offsetamt, vec2 pxpos) {
	return mix(
		mix(
			texture(uf_txtr, floor(pxpos * rndsz) / rndsz).rgb,
			texture(uf_txtr, floor((pxpos + vec2(offsetamt, 0)) * rndsz) / rndsz).rgb,
			fmodx),
		mix(
			texture(uf_txtr, floor((pxpos + vec2(0, offsetamt)) * rndsz) / rndsz).rgb,
			texture(uf_txtr, floor((pxpos + vec2(offsetamt, offsetamt)) * rndsz) / rndsz).rgb,
			fmodx),
		fmody);
}
vec3 Bloom() {
	int itercount = 6;
	float rndsz = 24.;
	float fmodx = (TexCoords.x - (floor(TexCoords.x * rndsz) / rndsz)) * rndsz;
	float fmody = (TexCoords.y - (floor(TexCoords.y * rndsz) / rndsz)) * rndsz;
	float offsetP = 1.f / rndsz;
	vec3 bloom = vec3(0.f, 0.f, 0.f);
	for (int x = -itercount; x < itercount; x++) {
		for (int y = -itercount; y < itercount; y++) {
			bloom += BloomAdd(rndsz, fmodx, fmody, offsetP, TexCoords + vec2(x * offsetP, y * offsetP));
		}
	}
	return bloom / ((itercount * 2) * (itercount * 2)) * 0.5f;
}

void main() { 
	// blur part
	// sample diffuse colour
	// wtf was i even doing here??
	vec3 sampleTex[9];
	//vec3 sampleTex[18];

	// 0.9 is like photoshop 0.5px blur
	float ofsx = 0.5f / wx;
	float ofsy = 0.5f / wy;
	
	sampleTex[0] = vec3(texture(uf_txtr, TexCoords.st + vec2(-ofsx, ofsy)));
	sampleTex[1] = vec3(texture(uf_txtr, TexCoords.st + vec2(0.0f,  ofsy)));
	sampleTex[2] = vec3(texture(uf_txtr, TexCoords.st + vec2(ofsx,  ofsy)));
	sampleTex[3] = vec3(texture(uf_txtr, TexCoords.st + vec2(-ofsx, 0.0f)));
	sampleTex[4] = vec3(texture(uf_txtr, TexCoords.st + vec2(0.0f,  0.0f)));
	sampleTex[5] = vec3(texture(uf_txtr, TexCoords.st + vec2(ofsx,  0.0f)));
	sampleTex[6] = vec3(texture(uf_txtr, TexCoords.st + vec2(-ofsx,-ofsy)));
	sampleTex[7] = vec3(texture(uf_txtr, TexCoords.st + vec2(0.0f, -ofsy)));
	sampleTex[8] = vec3(texture(uf_txtr, TexCoords.st + vec2(ofsx, -ofsy)));
	

	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++)
		col += sampleTex[i] / 9.f;
	
	//col = clamp((col), 0, 1);	
	
	//col += clamp((Bloom()) * 0.75f, 0.f, 128.f);

	FragColor.rgb = col;
	FragColor.a = 1.f;
	


	// Gamma correction
	//*
	const float exposure = 2.2;
	const float gamma = 0.6;
	// Exposure tone mapping
	vec3 mapped = vec3(1.0) - exp(-col * exposure);
	// Gamma correction 
	mapped = pow(mapped, vec3(1.0 / gamma));
	col = mapped;
	//*/
	
	//bleach overflow
	float highestOverflow = max(max(FragColor.r, FragColor.g), FragColor.b);
	FragColor.rgb = mix(FragColor.rgb, vec3((FragColor.r + FragColor.g + FragColor.b) / 3.f), clamp((highestOverflow - 0.9f) * 0.5f, 0.f, 1.f));


	//darken
	//FragColor = FragColor - (0.045 * (1 - FragColor));
	//FragColor *= 1.2f;
}
)"
#elif DEF_PROJECT == PROJECT_BC
// Colour distorted framebuffer
#define SHADER_FRAG_FRAMEBUFFER R"(
#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D uf_txtr;

uniform float wx = 640.f;
uniform float wy = 480.f;

const float PI = 3.1415926535; // for fisheye

// normally the first one is zero
const int indexMatrix4x4[16] = int[](
	0,	8,	2,	10,
	12,	4,	14,	6,
	3,	11,	1,	9,
	15,	7,	13,	5);

float kernel[9] = float[](
	1.0f/16,2.0f/16,1.0f/16,
	2.0f/16,4.0f/16,2.0f/16,
	1.0f/16,2.0f/16,1.0f/16
);

/*
float dither(float color, float index_value) {
	float closestColor = (color < 0.5) ? 0 : 1;
	float secondClosestColor = 1 - closestColor;
	float distance = abs(closestColor - color);
	return (distance < index_value) ? closestColor : secondClosestColor;
}*/

const int indexMat4x4PSX[16] = int[]
(
	-4,	0,	-3,	1,
	2,	-2,	3,	-1,
	-3,	1,	-4,	0,
	3,	-1,	2,	-2
);

//*
float indexValue(void) {
	int x = int(mod(gl_FragCoord.x, 4));
	int y = int(mod(gl_FragCoord.y, 4));
	return indexMatrix4x4[(x + y * 4)] / 16.0;
}

float dither(float color) {
	float closestColor = (color < 0.5) ? 0 : 1;
	float secondClosestColor = 1 - closestColor;
	float d = indexValue();
	float distance = abs(closestColor - color);
	return (distance < d) ? closestColor : secondClosestColor;
}
//*/

vec3 BloomAdd(float rndsz, float fmodx, float fmody, float offsetamt, vec2 pxpos) {
	return mix(
		mix(
			texture(uf_txtr, floor(pxpos * rndsz) / rndsz).rgb,
			texture(uf_txtr, floor((pxpos + vec2(offsetamt, 0)) * rndsz) / rndsz).rgb,
			fmodx),
		mix(
			texture(uf_txtr, floor((pxpos + vec2(0, offsetamt)) * rndsz) / rndsz).rgb,
			texture(uf_txtr, floor((pxpos + vec2(offsetamt, offsetamt)) * rndsz) / rndsz).rgb,
			fmodx),
		fmody);
}
vec3 Bloom() {
	int itercount = 8;
	float rndsz = 32.;
	float fmodx = (TexCoords.x - (floor(TexCoords.x * rndsz) / rndsz)) * rndsz;
	float fmody = (TexCoords.y - (floor(TexCoords.y * rndsz) / rndsz)) * rndsz;
	float offsetP = 1.f / rndsz;
	vec3 bloom = vec3(0.f, 0.f, 0.f);
	for (int x = -itercount; x < itercount; x++) {
		for (int y = -itercount; y < itercount; y++) {
			bloom += BloomAdd(rndsz, fmodx, fmody, offsetP, TexCoords + vec2(x * offsetP, y * offsetP));
		}
	}
	return bloom / ((itercount * 2) * (itercount * 2)) * 0.5f;
}

void main() { 
	//FragColor = texture(uf_txtr, TexCoords);
	
	// blur part
	//sample diffuse colour
	//vec3 sampleTex[9];
	vec3 sampleTex[18];

	// 0.9 is like PS 0.5px blur
	float ofsx = 1.f / wx;
	float ofsy = 1.f / wy;
	
	sampleTex[0] = vec3(texture(uf_txtr, TexCoords.st + vec2(-ofsx, ofsy)));
	sampleTex[1] = vec3(texture(uf_txtr, TexCoords.st + vec2(0.0f,  ofsy)));
	sampleTex[2] = vec3(texture(uf_txtr, TexCoords.st + vec2(ofsx,  ofsy)));
	sampleTex[3] = vec3(texture(uf_txtr, TexCoords.st + vec2(-ofsx, 0.0f)));
	sampleTex[4] = vec3(texture(uf_txtr, TexCoords.st + vec2(0.0f,  0.0f)));
	sampleTex[5] = vec3(texture(uf_txtr, TexCoords.st + vec2(ofsx,  0.0f)));
	sampleTex[6] = vec3(texture(uf_txtr, TexCoords.st + vec2(-ofsx,-ofsy)));
	sampleTex[7] = vec3(texture(uf_txtr, TexCoords.st + vec2(0.0f, -ofsy)));
	sampleTex[8] = vec3(texture(uf_txtr, TexCoords.st + vec2(ofsx, -ofsy)));
	
	// extra samples
	// Since I am going to come back to this some day and wonder wtf is going on here
	// Basically: I don't know why modx and mody make the blur work so well
	// They were meant to be used the other way around to create a dither effect!
	int modx = int(mod(gl_FragCoord.x, 2));
	int mody = int(mod(gl_FragCoord.y, 2));
	// Right hand side
	sampleTex[9] = vec3(texture(uf_txtr, TexCoords.st + vec2(ofsx * (-2 - modx), ofsy)));
	sampleTex[10] = vec3(texture(uf_txtr, TexCoords.st + vec2(ofsx * (-2 - modx), 0.0f)));
	sampleTex[11] = vec3(texture(uf_txtr, TexCoords.st + vec2(ofsx * (-2 - modx), -ofsy)));
	sampleTex[12] = vec3(texture(uf_txtr, TexCoords.st + vec2(ofsx * (-4 - modx), ofsy)));
	sampleTex[13] = vec3(texture(uf_txtr, TexCoords.st + vec2(ofsx * (-4 - modx), 0.f)));
	sampleTex[14] = vec3(texture(uf_txtr, TexCoords.st + vec2(ofsx * (-4 - modx), -ofsy)));
	// Top side
	sampleTex[15] = vec3(texture(uf_txtr, TexCoords.st + vec2(-ofsx, ofsy * (-2 - mody))));
	sampleTex[16] = vec3(texture(uf_txtr, TexCoords.st + vec2(0.0f,  ofsy * (-2 - mody))));
	sampleTex[17] = vec3(texture(uf_txtr, TexCoords.st + vec2(ofsx,  ofsy * (-2 - mody))));
	
	vec3 col = vec3(0.0);
	for(int i = 0; i < 18; i++)
		col += sampleTex[i] / 18.f;
	
	col = clamp(normalize(col), 0, 1);
	
	float difR = (col.r - ((col.g + col.b) * 0.5f)) + 0.5f;
	float difG = (col.g - ((col.r + col.b) * 0.5f)) + 0.5f;
	float difB = (col.b - ((col.r + col.g) * 0.5f)) + 0.5f;
	col = vec3(0.0);
	for(int i = 0; i < 9; i++)
		//col += sampleTex[i] / 9.f;
		col += sampleTex[i] * kernel[i];
	float brightness = (col.r + col.g + col.b) / 1.5f;
	col.r = brightness * difR;
	col.g = brightness * difG;
	col.b = brightness * difB;
	//col.r = min(brightness, difR);
	//col.g = min(brightness, difG);
	//col.b = min(brightness, difB);
	
	// BOX BLUR BLOOM
	//col += Bloom();
	
	FragColor.rgb = col;
	FragColor.a = 1.f;
	
	// Gamma correction
	//*
	const float exposure = 2.2;
	const float gamma = 0.6;
	// Exposure tone mapping
	vec3 mapped = vec3(1.0) - exp(-col * exposure);
	// Gamma correction 
	mapped = pow(mapped, vec3(1.0 / gamma));
	col = mapped;
	
	//bleach overflow
	float highestOverflow = max(max(col.r, col.g), col.b);
	col.rgb = mix(col.rgb, vec3(1,1,1), clamp((highestOverflow - 0.9f) * 0.75f, 0.f, 1.f));
	
	//darken
	//FragColor = FragColor - (0.045 * (1 - FragColor));
	
	//FragColor.rgb = col - 0.5f;
	
	//FragColor.rgb = vec3(1.f,1.f,0.f);
	
	//normal
	//FragColor = vec4(col, 1.0);
	//FragColor = vec4(red, grn, blu, 1.0);
	
	//FragColor = texture(uf_txtr, TexCoords);
	//FragColor -= texture(noise, TexCoords);
	
	// Dither
	/*
	int dx = int(mod(gl_FragCoord.x, 4));
	int dy = int(mod(gl_FragCoord.y, 4));
	float rndBy = 16.f;
	FragColor.rgb += indexMat4x4PSX[(dx + dy * 4)] / (rndBy * 4.f);
	//FragColor.rgb += (indexMatrix4x4[(dx + dy * 4)] - 8.f) / (rndBy * 4.f);
	// Posterize
	FragColor.rgb = round(FragColor.rgb * rndBy) / rndBy;
	//*/
	
	//FragColor = vec4(0.5f);
}
)"
#endif

#define SHADER_FRAG_FRAMEBUFFER_DEPTH_BUFFER R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uf_txtr;
uniform sampler2D depthTexture;

uniform float wx = 640.f;
uniform float wy = 480.f;

const float PI = 3.1415926535; // for fisheye

							   // normally the first one is zero
const int indexMatrix4x4[16] = int[](
	1, 8, 2, 10,
	12, 4, 14, 6,
	3, 11, 1, 9,
	15, 7, 13, 5);

float kernel[9] = float[](
	1.0f / 16, 2.0f / 16, 1.0f / 16,
	2.0f / 16, 4.0f / 16, 2.0f / 16,
	1.0f / 16, 2.0f / 16, 1.0f / 16
	);

/*
float dither(float color, float index_value) {
float closestColor = (color < 0.5) ? 0 : 1;
float secondClosestColor = 1 - closestColor;
float distance = abs(closestColor - color);
return (distance < index_value) ? closestColor : secondClosestColor;
}*/

const int indexMat4x4PSX[16] = int[]
(
	-4, 0, -3, 1,
	2, -2, 3, -1,
	-3, 1, -4, 0,
	3, -1, 2, -2
	);

//*
float indexValue(void) {
	int x = int(mod(gl_FragCoord.x, 4));
	int y = int(mod(gl_FragCoord.y, 4));
	return indexMatrix4x4[(x + y * 4)] / 16.0;
}

float dither(float color) {
	float closestColor = (color < 0.5) ? 0 : 1;
	float secondClosestColor = 1 - closestColor;
	float d = indexValue();
	float distance = abs(closestColor - color);
	return (distance < d) ? closestColor : secondClosestColor;
}
//*/

void main()
{
	//FragColor = texture(uf_txtr, TexCoords);

	// blur part
	//sample diffuse colour
	//vec3 sampleTex[9];
	vec3 sampleTex[18];

	// 0.9 is like PS 0.5px blur
	float ofsx = 0.5f / wx;
	float ofsy = 0.5f / wy;

	sampleTex[0] = vec3(texture(uf_txtr, TexCoords.st + vec2(-ofsx, ofsy)));
	sampleTex[1] = vec3(texture(uf_txtr, TexCoords.st + vec2(0.0f, ofsy)));
	sampleTex[2] = vec3(texture(uf_txtr, TexCoords.st + vec2(ofsx, ofsy)));
	sampleTex[3] = vec3(texture(uf_txtr, TexCoords.st + vec2(-ofsx, 0.0f)));
	sampleTex[4] = vec3(texture(uf_txtr, TexCoords.st + vec2(0.0f, 0.0f)));
	sampleTex[5] = vec3(texture(uf_txtr, TexCoords.st + vec2(ofsx, 0.0f)));
	sampleTex[6] = vec3(texture(uf_txtr, TexCoords.st + vec2(-ofsx, -ofsy)));
	sampleTex[7] = vec3(texture(uf_txtr, TexCoords.st + vec2(0.0f, -ofsy)));
	sampleTex[8] = vec3(texture(uf_txtr, TexCoords.st + vec2(ofsx, -ofsy)));

	vec3 col = vec3(0.0);
	for (int i = 0; i < 18; i++)
		col += sampleTex[i] / 9.f;

	col = clamp((col), 0, 1);

	//*
	float rndsz = 8.;
	float fmodx = (TexCoords.x - (floor(TexCoords.x * rndsz) / rndsz)) * rndsz;
	float fmody = (TexCoords.y - (floor(TexCoords.y * rndsz) / rndsz)) * rndsz;
	float offsetamt = 1.f / rndsz;

	//vec3 bloom = texture(uf_txtr, floor(TexCoords * rndsz) / rndsz).rgb;
	vec3 bloom;// = texture(uf_txtr, TexCoords).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f *  0.004f, 0.75f *    0.01f)).rgb;//
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f * -0.004f, 0.75f *    0.01f)).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f *  0.004f, 0.75f *   -0.01f)).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f * -0.004f, 0.75f *   -0.01f)).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f *  0.01f, 0.75f *   0.004f)).rgb;//
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f * -0.01f, 0.75f *   0.004f)).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f *  0.01f, 0.75f *  -0.004f)).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f * -0.01f, 0.75f *  -0.004f)).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f *  0.002f, 0.75f *   0.005f)).rgb;//
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f * -0.002f, 0.75f *   0.005f)).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f *  0.002f, 0.75f *  -0.005f)).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f * -0.002f, 0.75f *  -0.005f)).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f *  0.005f, 0.75f *   0.002f)).rgb;//
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f * -0.005f, 0.75f *   0.002f)).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f *  0.005f, 0.75f *  -0.002f)).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f * -0.005f, 0.75f *  -0.002f)).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f *  0.001f, 0.75f *  0.0025f)).rgb;//
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f * -0.001f, 0.75f *  0.0025f)).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f *  0.001f, 0.75f * -0.0025f)).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f * -0.001f, 0.75f * -0.0025f)).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f *  0.0025f, 0.75f *   0.001f)).rgb;//
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f * -0.0025f, 0.75f *   0.001f)).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f *  0.0025f, 0.75f *  -0.001f)).rgb;
	bloom += texture(uf_txtr, TexCoords + vec2(0.75f * -0.0025f, 0.75f *  -0.001f)).rgb;
	bloom /= 24.0;

	//FragColor.rgb += clamp(bloom * 0.5f - 0.15f, 0, 100);
	//FragColor.rgb += clamp(bloom * 0.5f - 0.15f, 0, 100);
	col += bloom * 0.5f;
	//col = min(col, bloom);
	//*/


	col = texture(uf_txtr, TexCoords).rgb;


	float offsetxdep = 1.0 / wx;
	float offsetydep = 1.0 / wy;

	//faded inside
	/*
	float depth = texture(depthTexture, TexCoords).r;
	depth = pow(depth, 10); // cheap depth curve correct (doesn't work well)

	float edgelevel_r = texture(depthTexture, TexCoords + vec2(offsetxdep, 0)).r;
	edgelevel_r = pow(edgelevel_r, 10); // cheap depth curve correct (doesn't work well)
	float edgelevel_l = texture(depthTexture, TexCoords + vec2(-offsetxdep, 0)).r;
	edgelevel_l = pow(edgelevel_l, 10); // cheap depth curve correct (doesn't work well)
	float edgelevel_u = texture(depthTexture, TexCoords + vec2(0, offsetydep)).r;
	edgelevel_u = pow(edgelevel_u, 10); // cheap depth curve correct (doesn't work well)
	float edgelevel_d = texture(depthTexture, TexCoords + vec2(0, -offsetydep)).r;
	edgelevel_d = pow(edgelevel_d, 10); // cheap depth curve correct (doesn't work well)

	//float edgelevel = (clamp(((((edgelevel_r + edgelevel_l + edgelevel_u + edgelevel_d) / 4) - depth)) * 128 - 0.125, 0, 1));
	float edgelevel = (clamp(((((edgelevel_r + edgelevel_l + edgelevel_u + edgelevel_d) / 4) - depth)) * 32768 - 0.125, 0, 1));

	//float edgelevel = (clamp(((((edgelevel_r + edgelevel_l + edgelevel_u + edgelevel_d) / 4) - depth)) * 256 - 0.25, 0, 1));

	col.rgb -= vec3(round(edgelevel)) * 0.25; // Transparent Line
	//col.rgb -= vec3(round(edgelevel)); // Line
	//col.rgb -= vec3(dither(edgelevel)); // Dithered Line
	//*/


	// binary inside
	//*
	float depth = texture(depthTexture, TexCoords).r;
	//float edgelevel_r = ((((texture(depthTexture, TexCoords + vec2(offsetxdep, 0)).r - depth))  * 128 - 0.125));
	//float edgelevel_l = ((((texture(depthTexture, TexCoords + vec2(-offsetxdep, 0)).r - depth)) * 128 - 0.125));
	//float edgelevel_u = ((((texture(depthTexture, TexCoords + vec2(0, offsetydep)).r - depth))  * 128 - 0.125));
	//float edgelevel_d = ((((texture(depthTexture, TexCoords + vec2(0, -offsetydep)).r - depth)) * 128 - 0.125));
	float edgelevel_r = ((((texture(depthTexture, TexCoords + vec2(offsetxdep, 0)).r - depth)) * 1024 - 0.125));
	float edgelevel_l = ((((texture(depthTexture, TexCoords + vec2(-offsetxdep, 0)).r - depth)) * 1024 - 0.125));
	float edgelevel_u = ((((texture(depthTexture, TexCoords + vec2(0, offsetydep)).r - depth)) * 1024 - 0.125));
	float edgelevel_d = ((((texture(depthTexture, TexCoords + vec2(0, -offsetydep)).r - depth)) * 1024 - 0.125));

	float edgelevel = 0;

	if (edgelevel_r > -edgelevel_l)
		//if (texture(depthTexture, TexCoords + vec2(-offsetxdep, 0)).r < depth + 0.0001)
		edgelevel = 1;
	if (edgelevel_l > -edgelevel_r)
		//if (texture(depthTexture, TexCoords + vec2(offsetxdep, 0)).r < depth + 0.0001)
		edgelevel = 1;
	if (edgelevel_d > -edgelevel_u)
		//if (texture(depthTexture, TexCoords + vec2(0, -offsetydep)).r < depth + 0.0001)
		edgelevel = 1;
	if (edgelevel_u > -edgelevel_d)
		//if (texture(depthTexture, TexCoords + vec2(0, offsetydep)).r < depth + 0.0001)
		edgelevel = 1;

	if (edgelevel > 0.5)
	{
		//col.rgb *= 4;
		col.rgb -= 0.5;
	}
	//*/




	FragColor.rgb = col;
	FragColor.a = 1.f;

	// Gamma correction
	//*
	const float exposure = 2.2;
	const float gamma = 0.6;
	// Exposure tone mapping
	vec3 mapped = vec3(1.0) - exp(-col * exposure);
	// Gamma correction 
	mapped = pow(mapped, vec3(1.0 / gamma));
	col = mapped;

	//bleach overflow
	float highestOverflow = max(max(col.r, col.g), col.b);
	FragColor.rgb = mix(col.rgb, vec3(1.2, 1.2, 1.2), clamp((highestOverflow - 0.9f) * 1.75f, 0.f, 1.f));

	//darken
	FragColor = FragColor - (0.045 * (1 - FragColor));

	//FragColor.rgb = col - 0.5f;





	//FragColor = texture(uf_txtr, TexCoords);
	//FragColor.rgb = vec3(texture(depthTexture, TexCoords).r);

	//normal
	//FragColor = vec4(col, 1.0);
	//FragColor = vec4(red, grn, blu, 1.0);

	//FragColor = texture(uf_txtr, TexCoords);
	//FragColor -= texture(noise, TexCoords);

	// Dither
	/*
	int dx = int(mod(gl_FragCoord.x, 4));
	int dy = int(mod(gl_FragCoord.y, 4));
	float rndBy = 16.f;
	FragColor.rgb += indexMat4x4PSX[(dx + dy * 4)] / (rndBy * 4.f);
	//FragColor.rgb += (indexMatrix4x4[(dx + dy * 4)] - 8.f) / (rndBy * 4.f);
	// Posterize
	FragColor.rgb = round(FragColor.rgb * rndBy) / rndBy;
	//*/

	//FragColor = vec4(0.5f);

	// fisheye
	/*
	float aperture = 178.0;
	float apertureHalf = 0.5 * aperture * (PI / 180.0);
	float maxFactor = sin(apertureHalf);
	vec2 uv;
	vec2 xy = 2.0 * TexCoords.xy - 1.0;
	float d = length(xy);
	if (d < (2.0-maxFactor)) {
	d = length(xy * maxFactor);
	float z = sqrt(1.0 - d * d);
	float r = atan(d, z) / PI;
	float phi = atan(xy.y, xy.x);
	uv.x = r * cos(phi) + 0.5;
	uv.y = r * sin(phi) + 0.5;
	}
	else { uv = TexCoords.xy; }
	vec4 c = texture2D(uf_txtr, uv);
	FragColor = c;
	//*/
}
)"

//________________________________________________________________________________________________________________________________
// GUI ---------------------------------------------------------------------------------------------------------------------------

#define SHADER_VERT_GUI R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;
uniform mat4 transform;
uniform mat4 offset;

void main()
{
	gl_Position = transform * offset * vec4(aPos, 1.0);
	//gl_Position = vec4(aPos, 1.0);
	ourColor = aColor;
	TexCoord = aTexCoord;
}
)"

#define SHADER_FRAG_GUI R"(
#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform float opa; // Opacity

void main()
{
    FragColor = texture(ourTexture, TexCoord);
	FragColor.a *= opa;
	FragColor.a = clamp(FragColor.a, 0.0, 1.0);
	//if (FragColor.a < 0.25)
	//	discard;
	//else FragColor.a = 1.0;
}
)"

//________________________________________________________________________________________________________________________________
// VERTEX 3D ---------------------------------------------------------------------------------------------------------------------

#define SSRC_VERT_3D R"(
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aCol;

out vec2 TexCoords;
out vec3 Normal;
out vec3 Pos;
out vec4 Col;
out vec4 LightSpacePos;
out vec3 LC; // Light Colour

uniform mat4 matm;
uniform mat4 matv;
uniform mat4 matp;
uniform mat4 lightProj;

uniform vec3 pcam;

// Shading variables

uniform float ft; // Time

uniform sampler2D tshadow; // texture shadow

uniform vec3 vsun;
uniform vec3 cSun;
uniform vec3 cAmb;
uniform vec3 cFog;
uniform vec3 cLit;
uniform vec3 edgcol;

uniform bool lit = true;

void main()
{
	TexCoords = aTexCoords;
	
	Normal = normalize(vec3(matm * vec4(aNormal, 0.0)));
	
    //gl_Position = matp * matv * matm * vec4(aPos, 1.0);

	vec4 pos2 = matm * vec4(aPos, 1.0);
	
	Pos = pos2.xyz;
	
    gl_Position = matp * matv * pos2;
	
	Col = aCol;
	
	LightSpacePos = lightProj * vec4(Pos, 1.0);
	
	// Shading
	
	float ndotl = clamp(dot(Normal, vsun) * Col.r, 0, 1);
	float ndotl_amb = clamp(dot(Normal, vec3(0,1,0)) * 0.5f + 0.75f, 0, 1);
	
	if (lit)
	{
		// WIP
		// edge light
		vec3 vd = Pos - pcam;
		vec3 halfwayDir = normalize(vsun - normalize(-vd));
		float edge_light = pow(max(dot(normalize(Normal), halfwayDir), 0.0), 2.0);
	
		LC =  (cAmb * ndotl_amb) // Ambient light
			+ (cSun * (ndotl + edge_light)) // Sunlight (and rim light)
			+ vec3(Col.g); // Emissive vertices
		
		edge_light = clamp(dot(Normal, normalize(vd)) * 0.5f + 0.5f, 0, 1);
		halfwayDir = normalize(vsun - normalize(vd));
		float spec_light = pow(max(dot(normalize(Normal), halfwayDir), 0.0), 6.0) * 4.0;
		
		// Mix between diffuse and metallic
		LC = mix(LC,
			(cAmb * edge_light) // Ambient light
			+ (cSun * max(spec_light, edge_light)) // Sunlight (and rim light)
			+ vec3(Col.g), Col.b); // Emissive vertices
			
		//LC = vec3(spec_light);
	}
	else
	{
		//LC *= ndotl * 2.f; Moon shading
		LC = vec3(3.f, 0.f, 0.f);
	}
}
)"

#define SSRC_VERT_3D_BLEND R"(
layout (location = 0) in vec3 aPos1;
layout (location = 1) in vec3 aPos2;
layout (location = 2) in vec3 aNor1;
layout (location = 3) in vec3 aNor2;
layout (location = 4) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 Pos;
out vec4 Col;
out vec4 LightSpacePos;
out vec3 LC; // Light Colour

uniform float blendState = 0.5f;

uniform mat4 matm;
uniform mat4 matv;
uniform mat4 matp;
uniform mat4 lightProj;

uniform vec3 pcam;

// Shading variables

uniform float ft; // Time

uniform sampler2D tshadow; // texture shadow

uniform vec3 vsun;
uniform vec3 cSun;
uniform vec3 cAmb;
uniform vec3 cFog;
uniform vec3 cLit;
uniform vec3 edgcol;

uniform bool lit = true;

void main()
{
	TexCoords = aTexCoords;
	
	//Normal = normalize(vec3(matm * vec4(aNor1, 0.0)));
	Normal = normalize(vec3(matm * vec4(mix(aNor1, aNor2, blendState), 0.0)));
	//Normal = normalize(vec3(matm * vec4(mix(aNor1, aNor1, blendState), 1.0)));
	
	//Height = clamp((model * vec4(aPos, 1.0)).y + 0.25, 0, 1);
	
	//precise projection
	//apos2 halved only temporarily
	vec4 pos2 = matm * vec4(mix(aPos1, aPos2, blendState), 1.0);
	Pos = pos2.xyz;
	
    gl_Position = matp * matv * pos2;
	
	Col = vec4(1.f, 0.f, 0.f, 0.f);

	LightSpacePos = lightProj * vec4(Pos, 1.0);

	// Shading
	
	float ndotl = clamp(dot(Normal, vsun) * Col.r, 0, 1);
	float ndotl_amb = clamp(dot(Normal, vec3(0,1,0)) * 0.5f + 0.75f, 0, 1);
	
	if (lit)
	{
		// WIP
		// edge light
		vec3 vd = Pos - pcam;
		vec3 halfwayDir = normalize(vsun - normalize(-vd));
		float edge_light = pow(max(dot(normalize(Normal), halfwayDir), 0.0), 2.0);
	
		LC =  (cAmb * ndotl_amb) // Ambient light
			+ (cSun * (ndotl + edge_light)) // Sunlight (and rim light)
			+ vec3(Col.g); // Emissive vertices
			
		edge_light = clamp(dot(Normal, normalize(vd)) * 0.5f + 0.5f, 0, 1);
		vec3 spec_light = vec3(pow(max(dot(normalize(Normal), normalize(vd)), 0.0), 6.0));
		
		// Mix between diffuse and metallic
		LC = mix(LC,
			(cAmb * ndotl_amb) // Ambient light
			+ (cSun * max(spec_light * 2, edge_light)) // Sunlight (and rim light)
			+ vec3(Col.g), Col.b); // Emissive vertices
	}
	else
	{
		//LC *= ndotl * 2.f; Moon shading
		LC = vec3(3.f, 0.f, 0.f);
	}
}
)"

#define SSRC_VERT_3D_DEFORM R"(
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aCol;
layout (location = 4) in float aDeform[4];

out vec2 TexCoords;
out vec3 Normal;
out vec3 Pos;
out vec4 Col;
out vec4 LightSpacePos;
out vec3 LC; // Light Colour

uniform uint mc = 1u; // Matrix count
uniform mat4 matma;
uniform mat4 matmb;
uniform mat4 matmc;
uniform mat4 matmd;
uniform mat4 matv;
uniform mat4 matp;
uniform mat4 lightProj;

// Bone forward vectors
uniform vec3 bfwa,bfwb,bfwc,bfwd;

uniform vec3 pcam;

// Shading variables

uniform float ft; // Time

uniform sampler2D tshadow; // texture shadow

uniform vec3 vsun;
uniform vec3 cSun;
uniform vec3 cAmb;
uniform vec3 cFog;
uniform vec3 cLit;
uniform vec3 edgcol;

uniform bool lit = true;

mat4 mixm(mat4 a, mat4 b, float f)
{
	mat4 m;
	m[0] = mix(a[0], b[0], f);
	m[1] = mix(a[1], b[1], f);
	m[2] = mix(a[2], b[2], f);
	m[3] = mix(a[3], b[3], f);
	return m;
}

void main()
{
	TexCoords = aTexCoords;
	
	//Normal = normalize(vec3(matma * vec4(aNormal, 0.0)));
	vec3 nora = vec3(matma * vec4(aNormal, 0.0));
	vec3 norb = vec3(matmb * vec4(aNormal, 0.0));
	vec3 norc = vec3(matmc * vec4(aNormal, 0.0));
	vec3 nord = vec3(matmd * vec4(aNormal, 0.0));
	Normal = normalize(mix(mix(mix(nora, norb, aDeform[0]), norc, aDeform[1]), nord, aDeform[2]));
	
	
	
	mat4 matr = mixm(mixm(mixm(matma, matmb, aDeform[0]), matmc, aDeform[1]), matmd, aDeform[2]);
	
	// Thiccness preservation, normalize the matrix size
	// TODO: use bone local forward vector, lerp between un-normalized and normalized vectors based on local vector's x, y, z
	//vec3 v = normalize(vec3(matr[2][0],matr[2][1],matr[2][2])); // normalize forward
	//matr[2][0] = v.x; matr[2][1] = v.y; matr[2][2] = v.z;
	vec3 v = normalize(vec3(matr[0][0],matr[0][1],matr[0][2])); // normalize right
	matr[0][0] = v.x; matr[0][1] = v.y; matr[0][2] = v.z;
	v = normalize(vec3(matr[1][0],matr[1][1],matr[1][2])); // normalize up
	matr[1][0] = v.x; matr[1][1] = v.y; matr[1][2] = v.z;
	
	//mat4 matr = mixm(matma, matmb, aDeform[0]);
	
	vec4 posa = matma * vec4(aPos, 1.0);
	vec4 posb = matmb * vec4(aPos, 1.0);
	vec4 posc = matmc * vec4(aPos, 1.0);
	vec4 posd = matmd * vec4(aPos, 1.0);
	
	//vec4 posTemp = mix(mix(mix(posa, posb, aDeform[0]), posc, aDeform[1]), posd, aDeform[2]);
	
	
	
	vec4 posTemp = matr * vec4(aPos * vec3(1.f, 1.f, 0.f), 1.0);
	
	posTemp.xyz += aPos.z * vec3(matr[2][0],matr[2][1],matr[2][2]);
	
	//posTemp.rgb += clamp(aDeform[3], -0.75f * 0.5f, 0.75f * 0.5f) * vec3(matmb[0][0],matmb[0][1],matmb[0][2]);
	//posTemp.rgb += clamp(aDeform[3], -0.75f * 0.5f, 0.75f * 0.5f) * vec3(matmb[1][0],matmb[1][1],matmb[1][2]);
	//if (aDeform[0] > 0.45 && aDeform[0] < 0.55)
		//posTemp.rgb += ((1 - aDeform[0]) - aDeform[1]) * vec3(matr[2][0],matr[2][1],matr[2][2]);
	
	
	Pos = posTemp.rgb;
	
	//gl_Position = matp * matv * mix(mix(mix(posa, posb, aDeform[0]), posc, aDeform[1]), posd, aDeform[2]);
	gl_Position = matp * matv * posTemp;
	
	Col = aCol;
	
	LightSpacePos = lightProj * vec4(Pos, 1.0);
	
	// Shading
	
	float ndotl = clamp(dot(Normal, vsun) * Col.r, 0, 1);
	float ndotl_amb = clamp(dot(Normal, vec3(0,1,0)) * 0.5f + 0.75f, 0, 1);
	
	if (lit)
	{
		// WIP
		// edge light
		vec3 vd = Pos - pcam;
		vec3 halfwayDir = normalize(vsun - normalize(-vd));
		float edge_light = pow(max(dot(normalize(Normal), halfwayDir), 0.0), 2.0);
	
		LC =  (cAmb * ndotl_amb) // Ambient light
			+ (cSun * (ndotl + edge_light)) // Sunlight (and rim light)
			+ vec3(Col.g); // Emissive vertices
			
		edge_light = clamp(dot(Normal, normalize(vd)) * 0.5f + 0.5f, 0, 1);
		vec3 spec_light = vec3(pow(max(dot(normalize(Normal), normalize(vd)), 0.0), 6.0));
		
		// Mix between diffuse and metallic
		LC = mix(LC,
			(cAmb * ndotl_amb) // Ambient light
			+ (cSun * max(spec_light * 2, edge_light)) // Sunlight (and rim light)
			+ vec3(Col.g), Col.b); // Emissive vertices
	}
	else
	{
		//LC *= ndotl * 2.f; Moon shading
		LC = vec3(3.f, 0.f, 0.f);
	}
	
	//LC = vec3((1 - aDeform[1]) - aDeform[0]) * 5.f;
	//LC = vec3((1 - aDeform[1])) * 5.f;
}
)"

#define SSRC_VERT_3D_TERRAIN R"(
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in float aTxTr[8];

out vec2 TexCoords;
out vec3 Normal;
out vec3 Pos;
out vec4 Col;
out float TBlend[8];
out vec4 LightSpacePos;
out vec3 LC; // Light Colour

uniform mat4 matm;
uniform mat4 matv;
uniform mat4 matp;
uniform mat4 lightProj;

uniform vec3 pcam;

// Shading variables

uniform float ft; // Time

uniform sampler2D tshadow; // texture shadow

uniform vec3 vsun = normalize(vec3(-1, 1, -1));
uniform vec3 cSun = vec3(0.15, 0.1, 0.1);
uniform vec3 cAmb = vec3(0.18, 0.19, 0.22);
uniform vec3 cFog = vec3(0.1, 0.1, 0.1);
uniform vec3 cLit = vec3(1.5f, 1.5f, 1.5f);
uniform vec3 edgcol = vec3(0.5f, 0.5f, 0.5f);

uniform bool lit = true;

void main()
{
	TexCoords = aTexCoords;

	Normal = normalize(vec3(matm * vec4(aNormal, 0.0)));

	//gl_Position = matp * matv * matm * vec4(aPos, 1.0);

	vec4 pos2 = matm * vec4(aPos, 1.0);

	Pos = pos2.xyz;

	gl_Position = matp * matv * pos2;

	Col = vec4(1.f, 0.f, 0.f, 0.f);
	TBlend = aTxTr;

	LightSpacePos = lightProj * vec4(Pos, 1.0);

	// Shading

	float ndotl = clamp(dot(Normal, vsun) * Col.r, 0, 1);
	float ndotl_amb = clamp(dot(Normal, vec3(0, 1, 0)) * 0.5f + 0.75f, 0, 1);

	if (lit)
	{
		// WIP
		// edge light
		vec3 vd = Pos - pcam;
		vec3 halfwayDir = normalize(vsun - normalize(-vd));
		vec3 edge_light = edgcol * pow(max(dot(normalize(Normal), halfwayDir), 0.0), 2.0);

		float xoffs = -Pos.z + 0.5f - round(-Pos.z + 0.5f);
		float yoffs = -Pos.x + 0.5f - round(-Pos.x + 0.5f);

		LC = (cAmb * ndotl_amb) // Ambient light
			//+ (cSun * ndotl + edge_light) // Sunlight (and rim light)
			+ (cSun * ndotl); // Sunlight
	}
	else
	{
		//LC *= ndotl * 2.f; Moon shading
		LC = vec3(1.5f);
	}
}
)"

//________________________________________________________________________________________________________________________________
// FRAGMENT 3D -------------------------------------------------------------------------------------------------------------------

#define SSRC_MAIN_FRAG_3D R"(
void main() {
	FragColor = texture(texture_diffuse1, TexCoords);
	if (FragColor.a < 0.5) discard;

	//float ndotl = clamp(dot(Normal, vsun) * Col.r, 0, 1);
	//float ndotl_amb = clamp(dot(Normal, vec3(0,-1,0)) + 0.5f, 0, 1);

	// lit part

	vec4 heightmap = texture(thm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f);

	float shadow = clamp(ShadowCalculation(LightSpacePos) * 2.f - 0.5f, 0.f, 1.f); // Sharpened

	FragColor.rgb *= LC * (shadow * 0.4f + 0.6f);

	// Add fog
	float fog_start = 0.1f; // for long distance default to 0.2f
	float fog_solid = 0.1f; // for long distance default to 0.0015f
	// the one we use for long distance
	//FragColor.rgb = mix(FragColor.rgb, fogcol, clamp((length((Pos.xz - pcam.xz) * 0.0015f) - 0.2f), 0.f, 1.f));
	// oppressive
	float fog_mix = clamp((length((Pos.xz - pcam.xz) * fFogDens) - fog_start), 0.f, 1.f);
	float fog_mix_half = clamp((length((Pos.xz - pcam.xz) * fFogDens) - fog_start) * 0.5f, 0.f, 1.f);
	FragColor.rgb = mix(FragColor.rgb, cFog, mix(fog_mix, fog_mix_half, Col.g));
}
)"

#define SSRC_MAIN_FRAG_3D_CHARA R"(
void main() {
	//FragColor = texture(texture_diffuse1, TexCoords);
	//if (FragColor.a < 0.5) discard;

	//vec3 txtr = clamp((texture(texture_diffuse1, TexCoords).rgb * 4.), 0., 1.);
	vec4 txtr = texture(texture_diffuse1, TexCoords);
	FragColor.rgb =  c_a * txtr.r;
	FragColor.rgb += c_b * txtr.g;
	FragColor.rgb += c_c * txtr.b;
	//FragColor.rgb =  c_a * round(txtr.r * 2.f) / 2.f;
	//FragColor.rgb += c_b * round(txtr.g * 2.f) / 2.f;
	//FragColor.rgb += c_c * round(txtr.b * 2.f) / 2.f;
	//FragColor.rgb =                   c_a * clamp(round(txtr.r * 2.f) / 2.f,0.f,1.f);
	//FragColor.rgb = mix(FragColor.rgb, c_b, clamp(round(txtr.g * 2.f) / 2.f,0.f,1.f));
	//FragColor.rgb = mix(FragColor.rgb, c_c, clamp(round(txtr.b * 2.f) / 2.f,0.f,1.f));
	//FragColor.rgb = mix(mix(c_a * round(clamp(txtr.r * 4.f - 1.5f, 0.f, 1.f)), c_b, round(clamp(txtr.g * 4.f - 1.5f, 0.f, 1.f))), c_c, round(clamp(txtr.b * 4.f - 1.5f, 0.f, 1.f)));


	vec3 vd = Pos - pcam;
	float amboffset = dot(normalize(Normal), normalize(vd));

	//float ndotl = clamp(dot(normalize(Normal), vsun), 0, 1);
	//float ndotl = clamp(dot(Normal, vsun), 0, 1);
	//float ndotl = clamp(dot(Normal, vsun) * 0.5 + 0.5, 0, 1);
	//float ndotl = clamp(round(dot(Normal, vsun) + 0.5 + (amboffset * -0.5f)), 0, 1); // Rounded Bent
	float ndotl = clamp(round(dot(Normal, vsun) + 0.5), 0, 1); // Rounded
	//float ndotl = clamp(round((dot(Normal, vsun) + 0.125) * 8) / 2, 0, 1); // Rounded 3-tone
	
	//float ndotl_amb = clamp(dot(Normal, vec3(0,-1,0)) + 0.5f, 0, 1);
	//float ndotl_amb = clamp(round((dot(Normal, vec3(0,-1,0)) + 0.5f)), 0, 1); // rounded
	//float ndotl_amb = clamp(round((dot(Normal, vec3(0,-1,0)) + 0.5f) + amboffset * 0.6f), 0, 1); // rounded
	//float ndotl_amb = clamp(round((dot(Normal, vec3(0,-1,0)) + 0.5f) * 2) / 2, 0, 1); // rounded 3-tone
	float ndotl_amb = 0.5f;
	
	
	
	
	
	vec4 heightmap = texture(thm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f);
	
	vec3 suncol = vec3(0.0,0.0,0.0);
	vec3 skycol = vec3(0.0,0.0,0.0);
	vec3 litcol = vec3(1.5f,1.5f,1.5f);
	
	float shadow = clamp(ShadowCalculation(LightSpacePos) * 2.f - 0.5f, 0.f, 1.f); // Sharpened
	//float shadow = clamp(ShadowCalculation(LightSpacePos) * 3.f - 1.5f, 0.f, 1.f); // Sharpened
	//float shadow = clamp(round(ShadowCalculation(LightSpacePos)), 0.f, 1.f); // Rounded
	//float shadow = ShadowCalculation(LightSpacePos); // Not Sharpened
	
	FragColor.rgb *= LC * (shadow * 0.4f + 0.6f);

	// Add fog
	float fog_start = 0.1f; // for long distance default to 0.2f
	float fog_solid = 0.1f; // for long distance default to 0.0015f
	// the one we use for long distance
	//FragColor.rgb = mix(FragColor.rgb, fogcol, clamp((length((Pos.xz - pcam.xz) * 0.0015f) - 0.2f), 0.f, 1.f));
	// oppressive
	float fog_mix = clamp((length((Pos.xz - pcam.xz) * fFogDens) - fog_start), 0.f, 1.f);
	float fog_mix_half = clamp((length((Pos.xz - pcam.xz) * fFogDens) - fog_start) * 0.5f, 0.f, 1.f);
	FragColor.rgb = mix(FragColor.rgb, cFog, mix(fog_mix, fog_mix_half, Col.g));

	FragColor.a = txtr.a;
}
)"

#define SSRC_MAIN_FRAG_3D_MEAT R"(
void main() {
	// fresnel
	///*
	vec3 vd = Pos - pcam;
	//FragColor.rgb = vec3(   pow(max(dot(normalize(Normal), -normalize(vd)), 0.0), 16.0));
	//float fres = clamp(dot(normalize(Normal), -normalize(vd)) * 3.5f - 0.8f, 0.f, 1.f);
	float fres = clamp(dot(normalize(Normal), -normalize(vd)) * 3.f - 0.5f, 0.f, 1.f);
	//*/

	FragColor = texture(texture_diffuse1, TexCoords);
	if (FragColor.a - (1.f - fres) < 0.4f) discard;
	//FragColor.a = FragColor.a - (1.f - fres);
	//if (FragColor.a * fres < 0.4f) discard;
	//if (FragColor.a < 0.3) discard;


	//float ndotl = clamp(dot(normalize(Normal), vsun), 0, 1);
	//float ndotl = clamp(dot(Normal, vsun) * Col.r, 0, 1);
	float ndotl = 1.f;
	//float ndotl = clamp(dot(Normal, vsun) * 0.5 + 0.5, 0, 1);
	//float ndotl = clamp(round((dot(Normal, vsun) + 0.125) * 8) / 2, 0, 1);
	//float ndotl_amb = clamp(dot(Normal, vec3(0,-1,0)) + 0.5f, 0, 1);
	float ndotl_amb = 0.5f;

	if (lit)
	{
		// Set Depth
		gl_FragDepth = gl_FragCoord.z + (FragColor.g * -30.f + 15.f) * (1.f - gl_FragCoord.z) * (1.f - gl_FragCoord.z);

		//FragColor.g = FragColor.r * 0.95f;
		//FragColor.b = FragColor.r * 0.9f;
		FragColor.g = 0.f;
		FragColor.b = 0.f;

		// Dithered soft intersection
		/*
		int dx2 = int(mod(gl_FragCoord.x, 4));
		int dy2 = int(mod(gl_FragCoord.y, 4));
		gl_FragDepth = gl_FragCoord.z + (indexMat4x4PSX[(dx2 + dy2 * 4)] * 2.f * (1.f - gl_FragCoord.z) * (1.f - gl_FragCoord.z));
		//*/

		//vec3 skycol = mix(texture(ts, vec2(ft, 16.5f / 32.f)).rgb, texture(ts, vec2(ft, 0.f)).rgb, ndotl_amb * shadow_heightmap.g);
		//vec3 skycol = mix(texture(ts, vec2(ft, 0.f)).rgb, texture(ts, vec2(ft, 16.5f / 32.f)).rgb, ndotl_amb * shadow_heightmap.g);
		vec3 skycol = mix(texture(ts, vec2(ft, 0.f)).rgb, texture(ts, vec2(ft, 16.5f / 32.f)).rgb, ndotl_amb);

		vec3 suncol = texture(ts, vec2(ft, 30.5f / 32.f)).rgb * 2.f;
		//vec3 fogcol = texture(ts, vec2(ft, (28.5f / 32.f) - (Pos.y / 256.f))).rgb;
		vec3 fogcol = texture(ts, vec2(ft, (29.f / 32.f) - clamp(Pos.y / 2048.f, 0.f, 1.f / 8.f))).rgb;
		//vec3 fogcol = texture(ts, vec2(ft, (28.5f / 32.f) - (clamp(Pos.y, 0.f, 1.f) / 512.f))).rgb; // todo: clamp position to the maximum terrain height, not just 1
		vec3 litcol = texture(ts, vec2(ft, 29.5f / 32.f)).rgb * 4.f;


		float shadow = clamp(ShadowCalculation(LightSpacePos) * 2.f - 0.5f, 0.f, 1.f); // Sharpened
		//float shadow = clamp(ShadowCalculation(LightSpacePos) * 3.f - 1.5f, 0.f, 1.f); // Sharpened
		//float shadow = clamp(round(ShadowCalculation(LightSpacePos) - 0.25), 0.f, 1.f); // Rounded
		//float shadow = ShadowCalculation(LightSpacePos); // Not Sharpened

		//FragColor.rgb = vec3(shadow_heightmap.g); // test

		// Ambient
		// max: if the sky is lighter than the sun, override it
		//FragColor.rgb *= mix(mix(skycol, max(suncol, skycol), shadow * ndotl) + litcol * texture(tlm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f).g, vec3(1.f), Col.g);
		FragColor.rgb *= mix(mix(skycol, max(suncol, skycol), ndotl) + litcol * texture(tlm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f).g, vec3(1.f), Col.g);
		FragColor.rgb *= (clamp(shadow + (1 - clamp(dot(Normal, vsun) * 4.f, 0.f, 1.f)), 0.f, 1.f)) * 0.4f + 0.6f;
		//fres test
		//FragColor.rgb = vec3(dot(normalize(ViewDir), Normal)) + 1;

		//phong
		//vec3 reflectDir = reflect(vsun, Normal);
		//FragColor.rgb += suncol * (pow(max(dot(normalize(ViewDir), reflectDir), 0.0), 8.0));

		//blinnphong
		/*
		vec3 vd = Pos - pcam;
		vec3 halfwayDir = normalize(vsun - normalize(vd));
		FragColor.rgb += suncol * pow(max(dot(normalize(Normal), halfwayDir), 0.0), 16.0) * shadow;
		//*/

		// fresnel
		/*
		vec3 vd = Pos - pcam;
		vec3 halfwayDir = normalize(vsun - normalize(vd));
		//FragColor.rgb = vec3(   pow(max(dot(normalize(Normal), -normalize(vd)), 0.0), 16.0)     );
		FragColor.rgb = vec3(   dot(normalize(Normal), -normalize(vd)) * 3.5f - 0.8f     );
		//*/

		// Dither
		/*
		int dx = int(mod(gl_FragCoord.x, 4));
		int dy = int(mod(gl_FragCoord.y, 4));
		float rndBy = 16.f;
		FragColor.rgb += indexMat4x4PSX[(dx + dy * 4)] / (rndBy * 4.f);
		FragColor.rgb = round(FragColor.rgb * rndBy) / rndBy;
		//*/

		// Dither
		///*
		int dx = int(mod(gl_FragCoord.x, 4));
		int dy = int(mod(gl_FragCoord.y, 4));
		if (max(max(FragColor.r, FragColor.g), FragColor.b) > 1.f)
		{
			float rndBy = 12.f;
			FragColor.rgb += indexMat4x4PSX[(dx + dy * 4)] / (rndBy * 4.f);
			FragColor.rgb = round(FragColor.rgb * rndBy) / rndBy;
		}
		else
		{
			float rndBy = 24.f;
			FragColor.rgb += indexMat4x4PSX[(dx + dy * 4)] / (rndBy * 4.f);
			FragColor.rgb = round(FragColor.rgb * rndBy) / rndBy;
		}
		//*/

		// Add fog
		//FragColor.rgb = mix(FragColor.rgb, csun, clamp((length((Pos - pcam) * 0.1f) - 0.2f), 0.f, 1.f));
		//FragColor.rgb = mix(FragColor.rgb, csun, clamp((length((Pos.xz - pcam.xz) * 0.1f) - 0.2f), 0.f, 1.f));
		// the one we use
		//FragColor.rgb = mix(FragColor.rgb, fogcol, clamp((length((Pos.xz - pcam.xz) * 0.0015f) - 0.2f), 0.f, 1.f));
	}
	else
	{
		FragColor.rgb *= ndotl * 2.f;
	}
}
)"

#define SSRC_MAIN_FRAG_3D_TERRAIN R"(
void main() {
	//FragColor = mix(texture(tt1, TexCoords), texture(tt2, TexCoords), TBlend.r);
	//FragColor = mix(texture(tt1, TexCoords), texture(tt2, TexCoords), TBlend.r);

	FragColor.rgb = texture(tt1, TexCoords).rgb * TBlend[0];
	FragColor.rgb += texture(tt2, TexCoords).rgb * TBlend[1];
	FragColor.rgb += texture(tt3, TexCoords).rgb * TBlend[2];
	FragColor.rgb += texture(tt4, TexCoords).rgb * TBlend[3];
	FragColor.rgb += texture(tt5, TexCoords).rgb * TBlend[4];
	FragColor.rgb += texture(tt6, TexCoords).rgb * TBlend[5];
	FragColor.rgb += texture(tt7, TexCoords).rgb * TBlend[6];
	FragColor.rgb += texture(tt8, TexCoords).rgb * TBlend[7];

	//float vala = texture(tt1, TexCoords).rgb;
	//float valb;
	//float valc;
	//float vald;
	//
	//FragColor.rgb =                    texture(tt1, TexCoords).rgb * clamp(TBlend.r * 2.f, 0.f, 1.f);
	//FragColor.rgb = max(FragColor.rgb, texture(tt2, TexCoords).rgb * clamp(TBlend.g * 2.f, 0.f, 1.f));
	//FragColor.rgb = max(FragColor.rgb, texture(tt3, TexCoords).rgb * clamp(TBlend.b * 2.f, 0.f, 1.f));
	//FragColor.rgb = max(FragColor.rgb, texture(tt4, TexCoords).rgb * clamp(TBlend.a * 2.f, 0.f, 1.f));

	FragColor.a = 1.f;
	if (FragColor.a < 0.5) discard;

	float ndotl = clamp(dot(Normal, vsun) * Col.r, 0, 1);
	float ndotl_amb = clamp(dot(Normal, vec3(0, -1, 0)) + 0.5f, 0, 1);

	// lit part

	vec4 heightmap = texture(thm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f);

	float shadow = clamp(ShadowCalculation(LightSpacePos) * 2.f - 0.5f, 0.f, 1.f); // Sharpened

	FragColor.rgb *= LC * (shadow * 0.4f + 0.6f);

	// Add fog
	float fog_start = 0.1f; // for long distance default to 0.2f
	float fog_solid = 0.1f; // for long distance default to 0.0015f
	FragColor.rgb = mix(FragColor.rgb, cFog, clamp((length((Pos.xz - pcam.xz) * fFogDens) - fog_start), 0.f, 1.f));
}
)"
