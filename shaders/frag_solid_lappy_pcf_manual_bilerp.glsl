#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec4 LightSpacePos;

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
uniform sampler2D thm; // texture heightmap
uniform sampler2D ts; // texture sky
//uniform sampler2DShadow tshadow; // texture shadow
uniform sampler2D tshadow; // texture shadow

uniform vec3 lightdir = normalize(vec3(-1,1,-1));

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
			1 - clamp((projCoords.z - texture(tshadow, nearestpoint).r) * 128.f, 0.0, 1.0),                          // Shadow A
			1 - clamp((projCoords.z - texture(tshadow, nearestpoint + vec2(1, 0) * texelSize).r) * 128.f, 0.0, 1.0), // Shadow B
			abs(offset.x)),
		mix( // X3, X4 Lerp
			1 - clamp((projCoords.z - texture(tshadow, nearestpoint + vec2(0, 1) * texelSize).r) * 128.f, 0.0, 1.0), // Shadow C
			1 - clamp((projCoords.z - texture(tshadow, nearestpoint + vec2(1, 1) * texelSize).r) * 128.f, 0.0, 1.0), // Shadow D
			abs(offset.x)),
		abs(offset.y));
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
	float shadow = 0.0;
	if (length(Pos.xz - pcam.xz) < 16.f)
	{
		// perform perspective divide
		vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
		// transform to [0,1] range
		projCoords = projCoords * 0.5 + 0.5;
		// get depth of current fragment from light's perspective
		float currentDepth = projCoords.z;

		// texture parameters
		ivec2 tsize = textureSize(tshadow, 0);
		vec2 texelSize = 1.0 / textureSize(tshadow, 0);
		
		float offsetamt = 1.4f * texelSize.x;
		float offsetamtdiag = 0.9f * texelSize.x;
		
		shadow = GetShadowBilinear(projCoords, tsize, texelSize);
		
		//if (shadow > 0.f && shadow < 1.f)
		{
			shadow += GetShadowBilinear(projCoords + vec3(offsetamt, 0.f, 0.f), tsize, texelSize);
			shadow += GetShadowBilinear(projCoords + vec3(0.f, offsetamt, 0.f), tsize, texelSize);
			shadow += GetShadowBilinear(projCoords + vec3(-offsetamt, 0.f, 0.f), tsize, texelSize);
			shadow += GetShadowBilinear(projCoords + vec3(0.f, -offsetamt, 0.f), tsize, texelSize);
			
			shadow += GetShadowBilinear(projCoords + vec3(offsetamtdiag, offsetamtdiag, 0.f), tsize, texelSize);
			shadow += GetShadowBilinear(projCoords + vec3(offsetamtdiag, -offsetamtdiag, 0.f), tsize, texelSize);
			shadow += GetShadowBilinear(projCoords + vec3(-offsetamtdiag, offsetamtdiag, 0.f), tsize, texelSize);
			shadow += GetShadowBilinear(projCoords + vec3(-offsetamtdiag, -offsetamtdiag, 0.f), tsize, texelSize);
			
			shadow /= 9;
		}
	}
	else shadow = 1.f;
	
    return shadow;
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
		float ndotl = clamp(dot(Normal, lightdir) * 2.f, 0, 1);

		vec4 color = texture(texture_diffuse1, TexCoords);
		if (color.a < 0.5)
		{
			//discard;
		};
		FragColor = texture(texture_diffuse1, TexCoords);
		//FragColor = texture(ts, TexCoords);
		//FragColor = texture(thm, TexCoords) * 64;
		vec3 skycol = texture(ts, vec2(ft, 0.1f)).rgb;
		vec3 suncol = texture(ts, vec2(ft, 30.5f / 32.f)).rgb * 2.f;
		//vec3 fogcol = texture(ts, vec2(ft, (28.5f / 32.f) - (Pos.y / 256.f))).rgb;
		vec3 fogcol = texture(ts, vec2(ft, (28.5f / 32.f) - (Pos.y / 512.f))).rgb;
		vec3 litcol = texture(ts, vec2(ft, 29.5f / 32.f)).rgb * 4.f;
		
		float heightthis = texture(thm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f).r;
		float heightnext = texture(thm, vec2(-Pos.z + 5.f, Pos.x + 5.f) / 2048.f).r;
		
		
		float shadow = ShadowCalculation(LightSpacePos);
		
		// Ambient
		//FragColor.rgb *= mix(camb, csun, texture(tlm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 128.f).g);
		//FragColor.rgb *= mix(vec3(0,0,0), csun, texture(tlm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 128.f).g);
		//FragColor.rgb *= mix(skycol, litcol, texture(tlm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f).g);
		//FragColor.rgb *= mix(skycol, suncol, ndotl) + litcol * texture(tlm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f).g;
		// max: if the sky is lighter than the sun, override it
		FragColor.rgb *= mix(skycol, max(suncol, skycol), shadow * ndotl) + litcol * texture(tlm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f).g;
		//FragColor.rgb = mix(FragColor.rgb * skycol, suncol, ndotl) + litcol * texture(tlm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f).g;
		
		//FragColor.rgb = vec3(shadow);
		//FragColor.rgb = vec3(texture(tshadow, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 128.f).r);
		
		int dx = int(mod(gl_FragCoord.x, 4));
		int dy = int(mod(gl_FragCoord.y, 4));
		//float index_value = indexMatrix4x4[(dx + dy * 4)] / 16.0;
		//float index_value = indexMat4x4PSX[(dx + dy * 4)] / 32.0;
		/*	
		int dx = int(mod(gl_FragCoord.x, 8));
		int dy = int(mod(gl_FragCoord.y, 8));
		float index_value = indexMatrix8x8[(dx + dy * 8)] / 16.0;*/
		
		// dither values
		//float lightval = dither(clamp(ndotl * 2,0,1), index_value);
		//float lightval2 = dither(clamp(ndotl * 2 - 1,0,1), index_value);
		
		//FragColor.a = clamp(FragColor.a * 3 - 1.5, 0, 1);
		
		//make alpha more sharp
		//FragColor.a = clamp(FragColor.a * 2 - 0.5, 0, 1);
		
		//gl_FragDepth = (FragColor.r + FragColor.g + FragColor.b) / 3;
		//gl_FragDepth = -Height + 1;
		
		//LIGHT value
		//FragColor.rgb *= mix(camb, csun, clamp(ndotl + 0.1, 0, 1));
		
		// Add fog
		//FragColor.rgb = mix(FragColor.rgb, csun, clamp((length((Pos - pcam) * 0.1f) - 0.2f), 0.f, 1.f));
		//FragColor.rgb = mix(FragColor.rgb, csun, clamp((length((Pos.xz - pcam.xz) * 0.1f) - 0.2f), 0.f, 1.f));
		FragColor.rgb = mix(FragColor.rgb, fogcol, clamp((length((Pos.xz - pcam.xz) * 0.002f) - 0.2f), 0.f, 1.f));
		
		// Dither
		
		float rndBy = 12.f;
		
		// Posterize
		//FragColor.rgb += indexMat4x4PSX[(dx + dy * 4)] / (rndBy * 4.f);
		//FragColor.rgb = round(FragColor.rgb * rndBy) / rndBy;
	}
}
