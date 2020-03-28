#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec4 Col;
in vec4 TBlend;
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
uniform sampler2D tlm; // texture lightmap
uniform sampler2D thm; // texture heightmap
uniform sampler2D ts; // texture sky
uniform sampler2D tshadow; // texture shadow

uniform vec3 vsun = normalize(vec3(-1,1,-1));
uniform vec3 cFog = vec3(0.1,0.1,0.1);
uniform float fFogDens = 0.f;

uniform bool lit = true;

// lower value = more blending
// Standard
//const float shadowDepthBlend = 2048.f;
// smooth
const float shadowDepthBlend = 512.f;

float GetShadowBilinear(vec3 projCoords, ivec2 tsize, vec2 texelSize)
{
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
		shadow += 1 - clamp((12.f - length(Pos.xz - pcam.xz)) / 6.f, 0, 1);
	}
	else shadow = 1.f;
	//else shadow = 0.f; // For debugging
	
    return shadow;
}

void main()
{
	//FragColor = mix(texture(tt1, TexCoords), texture(tt2, TexCoords), TBlend.r);
	//FragColor = mix(texture(tt1, TexCoords), texture(tt2, TexCoords), TBlend.r);
	FragColor.rgb = texture(tt1, TexCoords).rgb * TBlend.r;
	FragColor.rgb += texture(tt2, TexCoords).rgb * TBlend.g;
	FragColor.rgb += texture(tt3, TexCoords).rgb * TBlend.b;
	FragColor.rgb += texture(tt4, TexCoords).rgb * TBlend.a;
	FragColor.a = 1.f;
	if (FragColor.a < 0.5) discard;

	float ndotl = clamp(dot(Normal, vsun) * Col.r, 0, 1);
	float ndotl_amb = clamp(dot(Normal, vec3(0,-1,0)) + 0.5f, 0, 1);
	
	// lit part
	
	vec4 heightmap = texture(thm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f);
	
	float shadow = clamp(ShadowCalculation(LightSpacePos) * 2.f - 0.5f, 0.f, 1.f); // Sharpened
	
	FragColor.rgb *= LC * (shadow * 0.4f + 0.6f);
	
	// Add fog
	float fog_start = 0.1f; // for long distance default to 0.2f
	float fog_solid = 0.1f; // for long distance default to 0.0015f
	FragColor.rgb = mix(FragColor.rgb, cFog, clamp((length((Pos.xz - pcam.xz) * fFogDens) - fog_start), 0.f, 1.f));
}
