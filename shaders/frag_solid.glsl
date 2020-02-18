#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec4 Col;
in vec4 LightSpacePos;

uniform uint id; // identity
uniform bool idn; // id null
uniform vec3 pcam;

uniform float ft; // Time

uniform sampler2D texture_diffuse1;
uniform sampler2D tlm; // texture lightmap
uniform sampler2D thm; // texture heightmap
uniform sampler2D ts; // texture sky
uniform sampler2D tshadow; // texture shadow

uniform vec3 vsun = normalize(vec3(-1,1,-1));
uniform vec3 camb = vec3(0.f,0.f,0.f);

uniform bool lit = true;

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

float GetShadowBilinear(vec3 projCoords, ivec2 tsize, vec2 texelSize)
{
	vec2 nearestpoint = (round(projCoords.xy * tsize.x) - 0.5) / tsize.x; // also seems to work, for some reason
	vec2 offset = (projCoords.xy - nearestpoint) / texelSize.x; // known to work
	return mix( // X1xX2 x X3xX4 (Y lerp)
		mix( // X1, X2 Lerp
			1 - clamp((projCoords.z - texture(tshadow, nearestpoint).r) * 2048.f, 0.0, 1.0),                          // Shadow A
			1 - clamp((projCoords.z - texture(tshadow, nearestpoint + vec2(1, 0) * texelSize).r) * 2048.f, 0.0, 1.0), // Shadow B
			abs(offset.x)),
		mix( // X3, X4 Lerp
			1 - clamp((projCoords.z - texture(tshadow, nearestpoint + vec2(0, 1) * texelSize).r) * 2048.f, 0.0, 1.0), // Shadow C
			1 - clamp((projCoords.z - texture(tshadow, nearestpoint + vec2(1, 1) * texelSize).r) * 2048.f, 0.0, 1.0), // Shadow D
			abs(offset.x)),
		abs(offset.y));
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
	float shadow = 0.0;
	if (length(Pos.xz - pcam.xz) < 12.f)
	{
		// perform perspective divide
		vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
		// transform to [0,1] range (coord Z represents current fragment from light's perspective)
		projCoords = projCoords * 0.5 + 0.5;
		
		float currentDepth = projCoords.z;

		// texture parameters
		ivec2 tsize = textureSize(tshadow, 0);
		vec2 texelSize = 1.0 / textureSize(tshadow, 0);
		
		//shadow = GetShadowBilinear(projCoords, tsize, texelSize);
		
		// circle pattern
		///*
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
			//shadow = 1;
		}
		//*/
		//hex pattern
		/*
		float oHexA = 1.5f * texelSize.x;
		float oHexB = 1.29904f * texelSize.x;
		float oHexC = 0.75f * texelSize.x;
		shadow += GetShadowBilinear(projCoords + vec3(0.f, oHexA, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(oHexB, oHexC, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(oHexB, -oHexC, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(0.f, -oHexA, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(-oHexB, -oHexC, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(-oHexB, oHexC, 0.f), tsize, texelSize);
		shadow /= 7;
		//*/
		
		// double hex pattern
		/*
		float oHexA = 1.5f * texelSize.x;
		float oHexB = 1.29904f * texelSize.x;
		float oHexC = 0.75f * texelSize.x;
		shadow += GetShadowBilinear(projCoords + vec3(0.f, oHexA, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(oHexB, oHexC, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(oHexB, -oHexC, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(0.f, -oHexA, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(-oHexB, -oHexC, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(-oHexB, oHexC, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(0.f, oHexA, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(oHexB, oHexC, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(oHexB, -oHexC, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(0.f, -oHexA, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(-oHexB, -oHexC, 0.f), tsize, texelSize);
		shadow += GetShadowBilinear(projCoords + vec3(-oHexB, oHexC, 0.f), tsize, texelSize);
		shadow /= 13;
		//*/
		
		// Blend into clip distance
		shadow += 1 - clamp((12.f - length(Pos.xz - pcam.xz)) / 6.f, 0, 1);
	}
	else shadow = 1.f;
	//else shadow = 0.f; // For debugging
	
    return shadow;
}

void main()
{
	FragColor = texture(texture_diffuse1, TexCoords);
	//FragColor = texture(texture_diffuse1, vec2(Pos.x * 0.25f, Pos.z * 0.25f));
	if (FragColor.a < 0.5) discard;

	//float ndotl = clamp(dot(normalize(Normal), vsun), 0, 1);
	float ndotl = clamp(dot(Normal, vsun) * Col.r, 0, 1);
	//float ndotl = clamp(dot(Normal, vsun) * 0.5 + 0.5, 0, 1);
	//float ndotl = clamp(round((dot(Normal, vsun) + 0.125) * 8) / 2, 0, 1);
	float ndotl_amb = clamp(dot(Normal, vec3(0,-1,0)) + 0.5f, 0, 1);
	
	if (lit)
	{
		vec4 heightmap = texture(thm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f);
		
		vec3 suncol = vec3(0.0,0.0,0.0);
		vec3 skycol = vec3(0.0,0.0,0.0);
		vec3 fogcol = vec3(0.1,0.1,0.1);
		vec3 litcol = vec3(1.5f,1.5f,1.5f);
		
		
		float shadow = clamp(ShadowCalculation(LightSpacePos) * 2.f - 0.5f, 0.f, 1.f); // Sharpened
		//float shadow = clamp(ShadowCalculation(LightSpacePos) * 3.f - 1.5f, 0.f, 1.f); // Sharpened
		//float shadow = clamp(round(ShadowCalculation(LightSpacePos) - 0.25), 0.f, 1.f); // Rounded
		//float shadow = ShadowCalculation(LightSpacePos); // Not Sharpened
		
		//FragColor.rgb = vec3(shadow_heightmap.g); // test
		
		// Dither
		/*
		int dx = int(mod(gl_FragCoord.x, 4));
		int dy = int(mod(gl_FragCoord.y, 4));
		float rndBy = 6.f;
		FragColor.rgb += indexMat4x4PSX[(dx + dy * 4)] / (rndBy * 4.f);
		// Posterize
		FragColor.rgb = round(FragColor.rgb * rndBy) / rndBy;
		//*/
		
		// Ambient
		// max: if the sky is lighter than the sun, override it
		//FragColor.rgb *= mix(mix(skycol, max(suncol, skycol), shadow * ndotl) + litcol * texture(tlm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f).g, vec3(1.f), Col.g);
		//FragColor.rgb *= mix(mix(skycol, max(suncol, skycol), ndotl) + litcol * texture(tlm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f).g, vec3(1.f), Col.g);
		//FragColor.rgb *= skycol + litcol * texture(tlm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f).g * (ndotl * shadow * 0.4f + 0.6f);
		FragColor.rgb *= mix(skycol + litcol * texture(tlm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f).g * (shadow * 0.4f + 0.6f), vec3(1.f), Col.g);
		//FragColor.rgb *= (clamp(shadow + (1 - clamp(dot(Normal, vsun) * 4.f, 0.f, 1.f)), 0.f, 1.f)) * 0.4f + 0.6f;		
		//fres test
		//FragColor.rgb = vec3(dot(normalize(ViewDir), Normal)) + 1;
		
		//phong
		//vec3 reflectDir = reflect(vsun, Normal);
		//FragColor.rgb += suncol * (pow(max(dot(normalize(ViewDir), reflectDir), 0.0), 8.0));
		
		//blinnphong
		/*
		vec3 vd = Pos - pcam;
		vec3 halfwayDir = normalize(vsun - normalize(vd));
		FragColor.rgb += suncol * pow(max(dot(normalize(Normal), halfwayDir), 0.0), 16.0) * shadow * shadow_terrain;
		//*/
		
		// Dither
		/*
		int dx = int(mod(gl_FragCoord.x, 4));
		int dy = int(mod(gl_FragCoord.y, 4));
		float rndBy = 16.f;
		FragColor.rgb += indexMat4x4PSX[(dx + dy * 4)] / (rndBy * 4.f);
		FragColor.rgb = round(FragColor.rgb * rndBy) / rndBy;
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
