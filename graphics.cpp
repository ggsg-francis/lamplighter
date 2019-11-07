//TOP INCLUDE
#include "graphics.hpp"

//would like to drop this include
#include <glm\gtc\matrix_transform.hpp>
//i dont know what the point of using this one is
#include <glm\gtc\type_ptr.hpp>

//#include "global.h"
#include "cfg.h"

#include "maths.hpp"
#include "Transform.h"

extern TGA_ORDER *TGA_READER_ARGB;
extern TGA_ORDER *TGA_READER_ABGR;

#define FILE_VERSION_MB 0x0u
#define FILE_VERSION_M 0x0u
#define FILE_VERSION_TEX 0x0u

typedef btui16 version_t;

namespace graphics
{
	//btui8 font_kerning[] =
	//{
	//	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
	//	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
	//	7u,	3u,	6u,	7u,	8u,	7u,	7u,	3u,	4u,	4u,	5u,	6u,	3u,	6u,	3u,	4u,
	//	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	3u,	3u,	7u,	6u,	7u,	6u,
	//	8u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	6u,	6u,	7u,	6u,	8u,	7u,	7u,
	//	7u,	7u,	7u,	7u,	6u,	7u,	7u,	8u,	7u,	7u,	7u,	4u,	4u,	4u,	7u,	7u,
	//	3u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	6u,	5u,	7u,	4u,	8u,	7u,	7u,
	//	7u,	7u,	7u,	7u,	5u,	7u,	7u,	8u,	7u,	7u,	7u,	4u,	2u,	4u,	6u,	7u,

	//	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
	//	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
	//	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
	//	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
	//	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
	//	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
	//	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
	//	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
	//};

	btui8 font_kerning[] =
	{
		0u,	0u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,

		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
		7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,	7u,
	};

	glm::mat4 mat_proj;
	glm::mat4 mat_view;

	// TEMP TEMP TEMP TEMP
	GUIBitmap guibmp;
	int gui_x_start;
	int gui_y_start;

	Shader shader_solid;
	Shader shader_scroll;
	Shader shader_blend;
	Shader shader_terrain;
	Shader shader_shadow;
	Shader shader_sky;
	Shader shader_gui;
	Shader shader_post;

	void CompileShaderTest()
	{
		char* c = 
		R"(
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
		uniform sampler2D ttsm; // texture terrain shadow map
		uniform sampler2D tshadow; // texture shadow

		uniform vec3 vsun = normalize(vec3(-1, 1, -1));

		uniform bool lit = true;

		const int indexMatrix4x4old[16] = int[](
			0, 8, 2, 10,
			12, 4, 14, 6,
			3, 11, 1, 9,
			15, 7, 13, 5);

		const int indexMatrix4x4[16] = int[](
			1, 8, 2, 10,
			12, 4, 14, 6,
			3, 11, 1, 9,
			14, 7, 13, 5);

		const int indexMat4x4PSX[16] = int[]
		(
			-4, 0, -3, 1,
			2, -2, 3, -1,
			-3, 1, -4, 0,
			3, -1, 2, -2
			);

		// normally the first one is zero
		const int indexMatrix8x8[64] = int[](
			0, 32, 8, 40, 2, 34, 10, 42,
			48, 16, 56, 24, 50, 18, 58, 26,
			12, 44, 4, 36, 14, 46, 6, 38,
			60, 28, 52, 20, 62, 30, 54, 22,
			3, 35, 11, 43, 1, 33, 9, 41,
			51, 19, 59, 27, 49, 17, 57, 25,
			15, 47, 7, 39, 13, 45, 5, 37,
			63, 31, 55, 23, 61, 29, 53, 21);

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
					1 - clamp((projCoords.z - texture(tshadow, nearestpoint).r) * 512.f, 0.0, 1.0),                          // Shadow A
					1 - clamp((projCoords.z - texture(tshadow, nearestpoint + vec2(1, 0) * texelSize).r) * 512.f, 0.0, 1.0), // Shadow B
					abs(offset.x)),
				mix( // X3, X4 Lerp
					1 - clamp((projCoords.z - texture(tshadow, nearestpoint + vec2(0, 1) * texelSize).r) * 512.f, 0.0, 1.0), // Shadow C
					1 - clamp((projCoords.z - texture(tshadow, nearestpoint + vec2(1, 1) * texelSize).r) * 512.f, 0.0, 1.0), // Shadow D
					abs(offset.x)),
				abs(offset.y));
		}

		float ShadowCalculation(vec4 fragPosLightSpace)
		{
			float shadow = 0.0;
			if (length(Pos.xz - pcam.xz) < 32.f)
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
				shadow += 1 - clamp((32.f - length(Pos.xz - pcam.xz)) / 8.f, 0, 1);
			}
			else shadow = 1.f;
			//else shadow = 0.f; // For debugging

			return shadow;
		}

		void main()
		{
			if (gl_FragCoord.x > 319 && gl_FragCoord.x < 321 && gl_FragCoord.y > 239 && gl_FragCoord.y < 241)
			{
				vec4 col = texture(texture_diffuse1, TexCoords);
				if (col.a < 0.5) discard;
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
				FragColor = texture(texture_diffuse1, TexCoords);
				if (FragColor.a < 0.5) discard;

				//float ndotl = clamp(dot(normalize(Normal), vsun), 0, 1);
				float ndotl = clamp(dot(Normal, vsun), 0, 1);
				//float ndotl = clamp(dot(Normal, vsun) * 0.5 + 0.5, 0, 1);
				//float ndotl = clamp(round((dot(Normal, vsun) + 0.125) * 8) / 2, 0, 1);
				float ndotl_amb = clamp(dot(Normal, vec3(0, -1, 0)) + 0.5f, 0, 1);

				if (lit)
				{
					vec4 heightmap = texture(thm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f);
					vec4 shadow_heightmap = texture(ttsm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f);
					//float shadow_terrain = clamp(Pos.y - (shadow_heightmap.r * 64.f) + 1.f, 0, 1);
					//float shadow_terrain = clamp((Pos.y - (shadow_heightmap.r * 64.f) + 0.125f) * 8, 0, 1);
					float shadow_terrain = clamp((Pos.y - (shadow_heightmap.r * 64.f)) * 4, 0, 1);
					//float shadow_terrain = shadow_heightmap.g;
					//float shadow_terrain = clamp(((Pos.y - (shadow_heightmap.r * 64.f) + 0.125f) * 8) * (shadow_heightmap.g), 0, 1);
					//float shadow_terrain = clamp(Pos.y - 16.f, 0, 1);


					//vec3 skycol = mix(texture(ts, vec2(ft, 16.5f / 32.f)).rgb, texture(ts, vec2(ft, 0.f)).rgb, ndotl_amb * shadow_heightmap.g);
					//vec3 skycol = mix(texture(ts, vec2(ft, 0.f)).rgb, texture(ts, vec2(ft, 16.5f / 32.f)).rgb, ndotl_amb * shadow_heightmap.g);
					vec3 skycol = mix(texture(ts, vec2(ft, 0.f)).rgb, texture(ts, vec2(ft, 16.5f / 32.f)).rgb, ndotl_amb * shadow_heightmap.g);

					vec3 suncol = texture(ts, vec2(ft, 30.5f / 32.f)).rgb * 2.f;
					//vec3 fogcol = texture(ts, vec2(ft, (28.5f / 32.f) - (Pos.y / 256.f))).rgb;
					vec3 fogcol = texture(ts, vec2(ft, (28.5f / 32.f) - (clamp(Pos.y, 0.f, 1.f) / 512.f))).rgb; // todo: clamp position to the maximum terrain height, not just 1
					vec3 litcol = texture(ts, vec2(ft, 29.5f / 32.f)).rgb * 4.f;


					float shadow = clamp(ShadowCalculation(LightSpacePos) * 2.f - 0.5f, 0.f, 1.f); // Sharpened
																								   //float shadow = clamp(ShadowCalculation(LightSpacePos) * 3.f - 1.5f, 0.f, 1.f); // Sharpened
																								   //float shadow = clamp(round(ShadowCalculation(LightSpacePos) - 0.25), 0.f, 1.f); // Rounded
																								   //float shadow = ShadowCalculation(LightSpacePos); // Not Sharpened

																								   //FragColor.rgb = vec3(shadow_heightmap.g); // test

																								   // Ambient
																								   // max: if the sky is lighter than the sun, override it
																								   //if (shadow_heightmap.g < 0.5f)
					FragColor.rgb *= mix(skycol, max(suncol, skycol), shadow * ndotl * shadow_terrain) + litcol * texture(tlm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f).g;
					//else
					//	FragColor.rgb *= mix(skycol, max(suncol, skycol), shadow * ndotl) + litcol * texture(tlm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f).g;

					// Add fog
					//FragColor.rgb = mix(FragColor.rgb, csun, clamp((length((Pos - pcam) * 0.1f) - 0.2f), 0.f, 1.f));
					//FragColor.rgb = mix(FragColor.rgb, csun, clamp((length((Pos.xz - pcam.xz) * 0.1f) - 0.2f), 0.f, 1.f));
					FragColor.rgb = mix(FragColor.rgb, fogcol, clamp((length((Pos.xz - pcam.xz) * 0.0015f) - 0.2f), 0.f, 1.f));


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
					///*
					int dx = int(mod(gl_FragCoord.x, 4));
					int dy = int(mod(gl_FragCoord.y, 4));
					float rndBy = 12.f;
					FragColor.rgb += indexMat4x4PSX[(dx + dy * 4)] / (rndBy * 4.f);
					// Posterize
					FragColor.rgb = round(FragColor.rgb * rndBy) / rndBy;
					//*/
				}
				else
				{
					FragColor.rgb *= ndotl * 2.f;
				}
			}
		})";
	}

	void Init()
	{
		guibmp.Init();
		gui_x_start = -(int)cfg::iWinX / 4;
		gui_y_start = -(int)cfg::iWinY / 2;

		shader_solid = graphics::Shader("shaders/vert_3d.glsl", "shaders/frag_solid.glsl");
		shader_blend = graphics::Shader("shaders/vert_3d_blend.glsl", "shaders/frag_solid_chara.glsl");
		//shader_blend = graphics::Shader("shaders/vert_3d_blend.glsl", "shaders/frag_solid.glsl");
		shader_terrain = graphics::Shader("shaders/vert_3d_terrain.glsl", "shaders/frag_terrain.glsl");
		shader_sky = graphics::Shader("shaders/vert_3d_sky.glsl", "shaders/frag_sky.glsl");
		shader_gui = graphics::Shader("shaders/gui_vert.glsl", "shaders/gui_frag.glsl");
		shader_post = graphics::Shader("shaders/fb_vert.glsl", "shaders/fb_frag.glsl");
	}

	FRow4 operator*(const FRow4& row, const btf32 mult)
	{
		return FRow4(row[0] * mult, row[1] * mult, row[2] * mult, row[3] * mult);
	};
	FRow4 operator+(const FRow4& row_a, const FRow4& row_b)
	{
		return FRow4(row_a[0] + row_b[0], row_a[1] + row_b[1], row_a[2] + row_b[2], row_a[3] + row_b[3]);
	};

	Matrix4x4 MatrixTranslate(Matrix4x4 const& m, m::Vector3 const& v)
	{
		Matrix4x4 result = m;
		result[3] = m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3];
		return result;
		//matrix[3] = matrix[0] * pos.x + matrix[1] * pos.y + matrix[2] * pos.z + matrix[3];
	}

	Matrix4x4 MatrixRotate(Matrix4x4 const& m, btf32 angle, m::Vector3 const& v)
	{
		btf32 const c = cos(angle);
		btf32 const s = sin(angle);

		m::Vector3 axis = m::Normalize(v);
		m::Vector3 temp = (1 - c) * axis;

		Matrix4x4 rot;
		rot[0][0] = c + temp.x * axis.x;
		rot[0][1] = temp.x * axis.y + s * axis.z;
		rot[0][2] = temp.x * axis.z - s * axis.y;

		rot[1][0] = temp.y * axis.x - s * axis.z;
		rot[1][1] = c + temp.y * axis.y;
		rot[1][2] = temp.y * axis.z + s * axis.x;

		rot[2][0] = temp.z * axis.x + s * axis.y;
		rot[2][1] = temp.z * axis.y - s * axis.x;
		rot[2][2] = c + temp.z * axis.z;

		Matrix4x4 result;
		result[0] = m[0] * rot[0][0] + m[1] * rot[0][1] + m[2] * rot[0][2];
		result[1] = m[0] * rot[1][0] + m[1] * rot[1][1] + m[2] * rot[1][2];
		result[2] = m[0] * rot[2][0] + m[1] * rot[2][1] + m[2] * rot[2][2];
		result[3] = m[3];
		return result;
	}

	Matrix4x4 MatrixScale(Matrix4x4 const& m, m::Vector3 const& v)
	{
		Matrix4x4 Result;
		Result[0] = m[0] * v.x;
		Result[1] = m[1] * v.y;
		Result[2] = m[2] * v.z;
		Result[3] = m[3];
		return Result;
	}

	Matrix4x4 MatrixLookAt(Matrix4x4 const& matrix, m::Vector3 const& src, m::Vector3 const& targ, m::Vector3 const& up)
	{
		m::Vector3 f = m::Normalize(targ - src);
		m::Vector3 s = m::Normalize(m::Cross(f, up));
		m::Vector3 u = m::Cross(s, f);

		Matrix4x4 matr;
		matr[0][0] = s.x;
		matr[1][0] = s.y;
		matr[2][0] = s.z;
		matr[0][1] = u.x;
		matr[1][1] = u.y;
		matr[2][1] = u.z;
		matr[0][2] = -f.x;
		matr[1][2] = -f.y;
		matr[2][2] = -f.z;
		matr[3][0] = -m::Dot(s, src);
		matr[3][1] = -m::Dot(u, src);
		matr[3][2] = m::Dot(f, src);
		return matr;
	}

	void MatrixTransform(Matrix4x4& matrix, m::Vector3 const& pos)
	{
		// Translate matrix
		matrix[3] = FRow4(pos.x, pos.y, -pos.z, 1.f);
	}

	void MatrixTransform(Matrix4x4& matrix, m::Vector3 const& pos, btf32 yaw)
	{
		// Translate matrix
		matrix[3] = FRow4(pos.x, pos.y, -pos.z, 1.f);

		// Rotate matrix
		btf32 c = cos(yaw); btf32 s = sin(yaw);
		Matrix4x4 m = matrix;
		matrix[0] = m[0] * c + m[2] * -s;
		matrix[1] = m[1] * (c + (1 - c));
		matrix[2] = m[0] * s + m[2] * c;
	}

	void MatrixTransform(Matrix4x4& matrix, m::Vector3 const& pos, btf32 yaw, btf32 pitch)
	{
		// Translate matrix
		matrix[3] = FRow4(pos.x, pos.y, -pos.z, 1.f);

		// Rotate matrix (yaw)
		btf32 c = cos(yaw); btf32 s = sin(yaw);
		Matrix4x4 m = matrix;
		matrix[0] = m[0] * c + m[2] * -s;
		matrix[1] = m[1] * (c + (1 - c));
		matrix[2] = m[0] * s + m[2] * c;

		// Rotate matrix (pitch)
		c = cos(pitch); s = sin(pitch);
		m = matrix;
		matrix[0] = m[0] * (c + (1 - c));
		matrix[1] = m[1] * c + m[2] * s;
		matrix[2] = m[1] * -s + m[2] * c;
	}

	void MatrixTransform(Matrix4x4& matrix, m::Vector3 const& pos, m::Vector3 const& dir, m::Vector3 const& up = m::Vector3(0, 1, 0))
	{
		// Translate matrix
		matrix[3] = FRow4(pos.x, pos.y, -pos.z, 1.f);

		// Rotate matrix
		// Generate forward, right, up vectors for use in the matrix
		m::Vector3 vForw = m::Normalize(dir);
		m::Vector3 vSide = m::Normalize(m::Cross(up, dir));
		// We need to make sure we have a local up vector at right angles with forwards, so cross side and forward
		m::Vector3 vLoUp = m::Normalize(m::Cross(vForw, vSide));

		// The 3 direction vectors translate directly into the matrix
		// The third values are inverted for some reason beyond me.
		// (Don't know why forward must be inverted but will leave for now)
		matrix[0][0] = vSide.x; matrix[0][1] = vSide.y; matrix[0][2] = -vSide.z;
		matrix[1][0] = vLoUp.x; matrix[1][1] = vLoUp.y; matrix[1][2] = -vLoUp.z;
		matrix[2][0] = vForw.x; matrix[2][1] = vForw.y; matrix[2][2] = -vForw.z;
	}
	void MatrixTransformXFlip(Matrix4x4& matrix, m::Vector3 const& pos, m::Vector3 const& dir, m::Vector3 const& up = m::Vector3(0, 1, 0))
	{
		// Translate matrix
		matrix[3] = FRow4(pos.x, pos.y, -pos.z, 1.f);

		// Rotate matrix
		// Generate forward, right, up vectors for use in the matrix
		m::Vector3 vForw = m::Normalize(dir);
		m::Vector3 vSide = m::Normalize(m::Cross(up, dir));
		// We need to make sure we have a local up vector at right angles with forwards, so cross side and forward
		m::Vector3 vLoUp = m::Normalize(m::Cross(vForw, vSide));

		// The 3 direction vectors translate directly into the matrix
		// The third values are inverted for some reason beyond me.
		// (Don't know why forward must be inverted but will leave for now)
		matrix[0][0] = vSide.x; matrix[0][1] = vSide.y; matrix[0][2] = -vSide.z;
		matrix[1][0] = vLoUp.x; matrix[1][1] = vLoUp.y; matrix[1][2] = -vLoUp.z;
		matrix[2][0] = vForw.x; matrix[2][1] = vForw.y; matrix[2][2] = -vForw.z;

		matrix[0] = matrix[0] * -1.f;
	}

	void SetMatProj(btf32 fovMult)
	{
		cfg::bEditMode ?
			mat_proj = glm::perspective(glm::radians(cfg::fCameraFOV), (float)cfg::iWinX  / (float)cfg::iWinY, cfg::fCameraNearClip * fovMult, cfg::fCameraFarClip * fovMult) :
			mat_proj = glm::perspective(glm::radians(cfg::fCameraFOV), ((float)cfg::iWinX / 2.f) / (float)cfg::iWinY, cfg::fCameraNearClip * fovMult, cfg::fCameraFarClip * fovMult);
	}
	void SetMatView(void* t)
	{
		// this is not.... good.....
		#define T ((Transform3D*)t)
		mat_view = glm::lookAt((T->pos_glm + (T->GetUp() * 0.2f + T->GetForward() * 0.3f)) * glm::vec3(1.f, 1.f, -1.f),
			(T->pos_glm + (T->GetUp() * 0.2f + T->GetForward() * 1.3f)) * glm::vec3(1.f, 1.f, -1.f),
			(glm::vec3)T->GetUp() * glm::vec3(1.f, 1.f, -1.f));
		#undef T
	}
	//#define LIGHT_FAR 32.f
	//#define LIGHT_HALF 16.f
	#define LIGHT_FAR 256.f
	#define LIGHT_HALF 128.f
	#define LIGHT_WIDTH 16.f
	// Lightsource
	void SetMatProjLight()
	{
		mat_proj = glm::ortho(-LIGHT_WIDTH, LIGHT_WIDTH, -LIGHT_WIDTH, LIGHT_WIDTH, 0.f, LIGHT_FAR);
	}
	void SetMatViewLight(float x, float y, float z, float vx, float vy, float vz)
	{
		//mat_view = glm::lookAt(glm::vec3(x + -2.0f, y + 4.0f, z + -1.0f), glm::vec3(x, y, z), glm::vec3(0.0f, 1.0f, 0.0f));
		mat_view = glm::lookAt(glm::vec3(x - (vx * LIGHT_HALF), y - (vy * LIGHT_HALF), z - (vz * LIGHT_HALF)), glm::vec3(x, y, z), glm::vec3(0.0f, 1.0f, 0.0f));
		//mat_view = glm::lookAt(T->pos_glm + glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(0.0f, 4.0f, -.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void SetMatViewEditor(void* t)
	{
		// this is not.... good.....
		#define T ((Transform3D*)t)
		mat_view = glm::lookAt((T->pos_glm + (T->GetForward() * -5.f)) * glm::vec3(1.f, 1.f, -1.f),
			T->pos_glm * glm::vec3(1.f, 1.f, -1.f),
			(glm::vec3)T->GetUp() * glm::vec3(1.f, 1.f, -1.f));
		#undef T
	}

	glm::mat4 GetMatProj()
	{
		return mat_proj;
	}
	glm::mat4 GetMatView()
	{
		return mat_view;
	}

	void SetFrontFace()
	{
		glFrontFace(GL_CW);
	}
	void SetFrontFaceInverse()
	{
		glFrontFace(GL_CCW);
	}

	// good, but replace the std::vectors
	void BindBuffers(std::vector<vert> &vertices, std::vector<btui32> &indices, GLuint VAO, GLuint VBO, GLuint EBO)
	{
		glBindVertexArray(VAO); // Bind this vertex array
		glBindBuffer(GL_ARRAY_BUFFER, VBO); // Create vertex buffer in opengl
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vert), &vertices[0], GL_STATIC_DRAW); // Pass vertex struct to opengl
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Create index buffer in opengl
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(btui32), &indices[0], GL_STATIC_DRAW); // Pass index struct to opengl

		glEnableVertexAttribArray(vns::vi_pos); // Set Vertex positions
		glVertexAttribPointer(vns::vi_pos, 3, GL_FLOAT, GL_FALSE, sizeof(vert), (void*)vns::v_pos);
		glEnableVertexAttribArray(vns::vi_nor); // Set Vertex normals
		glVertexAttribPointer(vns::vi_nor, 3, GL_FLOAT, GL_FALSE, sizeof(vert), (void*)vns::v_nor);
		glEnableVertexAttribArray(vns::vi_uvc); // Set Vertex texture coords
		glVertexAttribPointer(vns::vi_uvc, 2, GL_FLOAT, GL_FALSE, sizeof(vert), (void*)vns::v_uvc);
		glEnableVertexAttribArray(vns::vi_col); // Set Vertex colour
		glVertexAttribPointer(vns::vi_col, 3, GL_FLOAT, GL_FALSE, sizeof(vert), (void*)vns::v_col);
	}

	void DrawGUITexture(Texture* texture, Shader* shader, bti32 x, bti32 y, bti32 w, bti32 h)
	{
		// TEEEEMMP TEMP TEMP TEMP
		guibmp.SetShader(shader);
		guibmp.SetTexture(texture->glID);
		guibmp.Draw(x, y, w, h);
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- SHADER ---------------------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	Shader::Shader()
	{
	}
	Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
	{
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			// if geometry shader path is present, also load a geometry shader
			if (geometryPath != nullptr)
			{
				gShaderFile.open(geometryPath);
				std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char * fShaderCode = fragmentCode.c_str();
		// 2. compile shaders
		unsigned int vertex, fragment;
		//int success;
		//char infoLog[512];
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		CheckCompileErrors(vertex, "VERTEX");
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		CheckCompileErrors(fragment, "FRAGMENT");
		// if geometry shader is given, compile geometry shader
		unsigned int geometry;
		if (geometryPath != nullptr)
		{
			const char * gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			CheckCompileErrors(geometry, "GEOMETRY");
		}
		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (geometryPath != nullptr)
			glAttachShader(ID, geometry);
		glLinkProgram(ID);
		CheckCompileErrors(ID, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryPath != nullptr)
			glDeleteShader(geometry);
	}
	void Shader::Use()
	{ glUseProgram(ID); }
	void Shader::SetBool(const std::string &name, bool value) const
	{ glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); }
	void Shader::SetInt(const std::string &name, int value) const
	{ glUniform1i(glGetUniformLocation(ID, name.c_str()), value); }
	void Shader::SetUint(const std::string &name, unsigned int value) const
	{ glUniform1ui(glGetUniformLocation(ID, name.c_str()), value); }
	void Shader::SetFloat(const std::string &name, float value) const
	{ glUniform1f(glGetUniformLocation(ID, name.c_str()), value); }
	void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
	{ glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
	void Shader::setVec2(const std::string &name, float x, float y) const
	{ glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); }
	void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
	{ glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
	void Shader::setVec3(const std::string &name, float x, float y, float z) const
	{ glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); }
	void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
	{ glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
	void Shader::setVec4(const std::string &name, float x, float y, float z, float w)
	{ glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); }
	void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
	{ glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
	void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
	{ glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
	void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
	{ glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
	void Shader::setMat4(const std::string& name, const Matrix4x4& mat) const
	{ glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
	void Shader::CheckCompileErrors(GLuint shader, const std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- TEXTURE --------------------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	ModifiableTexture::~ModifiableTexture()
	{
		delete[] buffer;
	}
	void ModifiableTexture::Init(btui16 sx, btui16 sy, colour col)
	{
		glGenTextures(1, &glID); // Initialize GL texture

		width = sx;
		height = sy;
		// Read pixel buffer
		buffer = new colour[width * height];
		for (btui32 i = 0ui32; i < (btui32)(width * height); ++i)
		{
			buffer[i] = col;
		}
	}
	void ModifiableTexture::LoadFile(char * fn)
	{
		glGenTextures(1, &glID); // Initialize GL texture

		FILE *file = fopen(fn, "rb"); // Open the file
		if (file)
		{
			// Seek the beginning of the file
			fseek(file, 0, SEEK_SET);
			// Read version
			version_t v;
			fread(&v, sizeof(version_t), 1, file);
			TextureFilterMode fm;
			TextureEdgeMode em;
			fread(&fm, 1, 1, file);
			fread(&em, 1, 1, file);
			// Read dimensions
			fread(&width, sizeof(btui16), 1, file);
			fread(&height, sizeof(btui16), 1, file);
			// Read pixel buffer
			buffer = new colour[width * height];
			//if (sizeof(graphics::colour) * width * height > sizeof(buffer)) return;
			fread(buffer, sizeof(graphics::colour), width * height, file);

			fclose(file);
		}
	}
	void ModifiableTexture::SetPixel(btui16 x, btui16 y, colour c)
	{
		buffer[x * width + y] = c;
	}
	void ModifiableTexture::SetPixelChannelR(btui16 x, btui16 y, btui8 val)
	{
		buffer[x * width + y].r = val;
	}
	void ModifiableTexture::SetPixelChannelG(btui16 x, btui16 y, btui8 val)
	{
		buffer[x * width + y].g = val;
	}
	void ModifiableTexture::SetPixelChannelB(btui16 x, btui16 y, btui8 val)
	{
		buffer[x * width + y].b = val;
	}
	void ModifiableTexture::SetPixelChannelA(btui16 x, btui16 y, btui8 val)
	{
		buffer[x * width + y].a = val;
	}
	colour ModifiableTexture::GetPixel(btui16 x, btui16 y)
	{
		return buffer[x * width + y];
	}
	void FMNearest() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	void FMNearestMip() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	void FMLinear() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	void FMLinearMip() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	void(*SetFilterMode[])() = { FMNearest, FMNearestMip, FMLinear, FMLinearMip };
	void EMRepeat() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	void EMRepeatMirror() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	}
	void EMClamp() { // Also can use GL_CLAMP_TO_BORDER
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	void EMRepeatXClampY() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	void EMClampXRepeatY() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	void(*SetEdgeMode[])() = { EMRepeat, EMRepeatMirror, EMClamp, EMRepeatXClampY, EMClampXRepeatY };
	void ModifiableTexture::ReBindGL(TextureFilterMode filter_mode, TextureEdgeMode edge_mode)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, glID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

		SetFilterMode[filter_mode]();
		SetEdgeMode[edge_mode]();
	}

	void Texture::LoadFile(char * fn)
	{
		glGenTextures(1, &glID);

		FILE *file = fopen(fn, "rb"); // Open the file
		if (file)
		{
			// Seek the beginning of the file
			fseek(file, 0, SEEK_SET);
			// Read version
			version_t v;
			fread(&v, sizeof(version_t), 1, file);
			TextureFilterMode fm;
			TextureEdgeMode em;
			fread(&fm, 1, 1, file);
			fread(&em, 1, 1, file);
			// Read dimensions
			fread(&width, sizeof(btui16), 1, file);
			fread(&height, sizeof(btui16), 1, file);
			// Read pixel buffer
			graphics::colour* buffer = new colour[width * height];
			fread(buffer, sizeof(graphics::colour), width * height, file);

			fclose(file);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, glID);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

			SetFilterMode[fm]();
			SetEdgeMode[em]();

			delete[] buffer;
		}
	}
	void Texture::InitRenderTexture(int x, int y, bool linear)
	{
		width = x;
		height = y;

		// The texture we're going to render to
		//unsigned int rendertexture;
		glGenTextures(1, &glID);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		#ifdef DEF_MULTISAMPLE // Multisampled
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, glID); // "Bind" the newly created texture : all future texture functions will modify this texture
		#ifdef DEF_HDR // HDR Texture
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, DEF_MULTISAMPLE_DEPTH, GL_RGBA16F, x, y, GL_TRUE); //create a blank image
		#else // Not HDR Texture
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, DEF_MULTISAMPLE_DEPTH, GL_RGBA, x, y, GL_TRUE); //create a blank image
		#endif
		#else // Not Multisampled
		glBindTexture(GL_TEXTURE_2D, glID); // "Bind" the newly created texture : all future texture functions will modify this texture
		#ifdef DEF_HDR // HDR Texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, x, y, 0, GL_RGBA, GL_FLOAT, NULL); //create a blank image
		#else // Not HDR Texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); //create a blank image
		#endif
		#endif
		EMClamp();
		linear ? FMLinear() : FMNearest();
		//attach this texture to the framebuffer
		#ifdef DEF_MULTISAMPLE
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, glID, 0);
		#else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glID, 0);
		#endif
	}
	void Texture::InitDepthTexture(int x, int y, bool linear)
	{
		width = x;
		height = y;

		glGenTextures(1, &glID);

		glBindTexture(GL_TEXTURE_2D, glID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, x, y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if (linear)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}

		//attach this texture to the framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, glID, 0);
	}
	void Texture::InitShadowTexture(int x, int y, bool linear)
	{
		width = x;
		height = y;

		glGenTextures(1, &glID);
		glBindTexture(GL_TEXTURE_2D, glID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, x, y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

		if (linear)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}

		//attach this texture to the framebuffer
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, id, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, glID, 0);

		//We only need the depth information when rendering the scene from the light's perspective
		//so there is no need for a color buffer. A framebuffer object however is not complete without
		//a color buffer so we need to explicitly tell OpenGL we're not going to render any color data.
		//We do this by setting both the read and draw buffer to GL_NONE with glDrawBuffer and glReadbuffer.

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	void Texture::InitDepthBuffer(int x, int y, bool linear)
	{
		width = x;
		height = y;

		//render buffer object (depth)
		//unsigned int depthbuffer;
		glGenRenderbuffers(1, &glID);
		glBindRenderbuffer(GL_RENDERBUFFER, glID);
		#ifdef DEF_MULTISAMPLE
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, DEF_MULTISAMPLE_DEPTH, GL_DEPTH24_STENCIL8, x, y);
		#else
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, x, y);
		#endif

		//Once we've allocated enough memory for the renderbuffer object we can unbind the renderbuffer
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		//bind rbo to framebuffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glID);
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- MESH -----------------------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	void Mesh::Draw(unsigned int tex, unsigned int shd)
	{
		glActiveTexture(GL_TEXTURE0); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(shd, "texture_diffuse1"), 0);
		glBindTexture(GL_TEXTURE_2D, tex); // Bind the texture
		glBindVertexArray(vao); // Bind vertex array
		glDrawElements(GL_TRIANGLES, glID, GL_UNSIGNED_INT, 0); // Draw call
		glBindVertexArray(0); glActiveTexture(GL_TEXTURE0); // Return buffers to default (texture is duplicate call)
	}
	void Mesh::LoadFile(char* fn)
	{
		std::cout << "Loading " << fn << "... ";

		//-------------------------------- OPEN FILE

		FILE* in = fopen(fn, "rb");
		if (in != NULL)
		{
			vert* vces; // Vertices
			size_t vces_size;
			btui32* ices; // Indices
			size_t ices_size;

			fseek(in, 0, SEEK_SET); // Seek the beginning of the file
			version_t v; fread(&v, sizeof(version_t), 1, in); // Read version

			//-------------------------------- READ VERTICES

			fread(&vces_size, sizeof(size_t), 1, in); // Read number of vertices
			vces = (vert*)malloc(sizeof(vert) * vces_size); // Allocate buffer to hold our vertices
			fread(&vces[0], sizeof(vert), vces_size, in); // Read vertices

			//-------------------------------- READ INDICES

			fread(&ices_size, sizeof(size_t), 1, in); // Read number of indices
			ices = (btui32*)malloc(sizeof(btui32) * ices_size); // Allocate buffer to hold our indicess
			fread(&ices[0], sizeof(unsigned int), ices_size, in); // Read indices

			fclose(in);

			glID = (GLuint)ices_size; // Set number of indices used in Draw()

			//-------------------------------- INITIALIZE OPENGL BUFFER

			glGenVertexArrays(1, &vao); // Create vertex buffer
			glGenBuffers(1, &vbo); glGenBuffers(1, &ebo); // Generate vertex and element buffer

			glBindVertexArray(vao); // Bind this vertex array
			glBindBuffer(GL_ARRAY_BUFFER, vbo); // Create vertex buffer in opengl
			glBufferData(GL_ARRAY_BUFFER, vces_size * sizeof(vert), &vces[0], GL_STATIC_DRAW); // Pass vertex struct to opengl
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Create index buffer in opengl
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, ices_size * sizeof(btui32), &ices[0], GL_STATIC_DRAW); // Pass index struct to opengl

			glEnableVertexAttribArray(vns::vi_pos); // Set Vertex positions
			glVertexAttribPointer(vns::vi_pos, 3, GL_FLOAT, GL_FALSE, sizeof(vert), (void*)vns::v_pos);
			glEnableVertexAttribArray(vns::vi_nor); // Set Vertex normals
			glVertexAttribPointer(vns::vi_nor, 3, GL_FLOAT, GL_FALSE, sizeof(vert), (void*)vns::v_nor);
			glEnableVertexAttribArray(vns::vi_uvc); // Set Vertex texture coords
			glVertexAttribPointer(vns::vi_uvc, 2, GL_FLOAT, GL_FALSE, sizeof(vert), (void*)vns::v_uvc);
			glEnableVertexAttribArray(vns::vi_col); // Set Vertex colour
			glVertexAttribPointer(vns::vi_col, 3, GL_FLOAT, GL_FALSE, sizeof(vert), (void*)vns::v_col);

			glBindVertexArray(0); // Bind default vertex array
			free(vces);
			free(ices);

			std::cout << "Generated mesh!" << std::endl;
		}
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- MESHBLEND ------------------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	void MeshBlend::Draw(unsigned int tex, unsigned int shd)
	{
		glActiveTexture(GL_TEXTURE0); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(shd, "texture_diffuse1"), 0);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, tex);

		// draw mesh
		glBindVertexArray(vao);
		//glDrawElements(GL_TRIANGLES, ices.size(), GL_UNSIGNED_INT, 0);
		glDrawElements(GL_TRIANGLES, glID, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}
	void MeshBlend::LoadFile(char* fn)
	{
		std::cout << "Loading " << fn << "... ";

		//-------------------------------- OPEN FILE

		FILE* in = fopen(fn, "rb");
		if (in != NULL)
		{
			vert_blend* vces; // Vertices
			size_t vces_size;
			btui32* ices; // Indices
			size_t ices_size;

			fseek(in, 0, SEEK_SET); // Seek the beginning of the file
			version_t v; fread(&v, sizeof(version_t), 1, in); // Read version

			//-------------------------------- READ VERTICES

			fread(&vces_size, sizeof(size_t), 1, in); // Read number of vertices
			vces = (vert_blend*)malloc(sizeof(vert_blend) * vces_size); // Allocate buffer to hold our vertices
			fread(&vces[0], sizeof(vert_blend), vces_size, in); // Read vertices

			//-------------------------------- READ INDICES

			fread(&ices_size, sizeof(size_t), 1, in); // Read number of indices
			ices = (btui32*)malloc(sizeof(btui32) * ices_size); // Allocate buffer to hold our indicess
			fread(&ices[0], sizeof(unsigned int), ices_size, in); // Read indices

			fclose(in);

			glID = (GLuint)ices_size; // Set number of indices used in Draw()

			//-------------------------------- INITIALIZE OPENGL BUFFER

			glGenVertexArrays(1, &vao); // Create vertex buffer
			glGenBuffers(1, &vbo); glGenBuffers(1, &ebo); // Generate vertex and element buffer

			glBindVertexArray(vao); // Bind this vertex array
			glBindBuffer(GL_ARRAY_BUFFER, vbo); // Create vertex buffer in opengl
			glBufferData(GL_ARRAY_BUFFER, vces_size * sizeof(vert_blend), &vces[0], GL_STATIC_DRAW); // Pass vertex struct to opengl
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Create index buffer in opengl
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, ices_size * sizeof(btui32), &ices[0], GL_STATIC_DRAW); // Pass index struct to opengl

			glEnableVertexAttribArray(graphics::vns::vbi_pos_a); // Set Vertex positions
			glVertexAttribPointer(graphics::vns::vbi_pos_a, 3, GL_FLOAT, GL_FALSE, sizeof(vert_blend), (void*)graphics::vns::vb_pos_a);
			glEnableVertexAttribArray(graphics::vns::vbi_pos_b);
			glVertexAttribPointer(graphics::vns::vbi_pos_b, 3, GL_FLOAT, GL_FALSE, sizeof(vert_blend), (void*)graphics::vns::vb_pos_b);

			glEnableVertexAttribArray(graphics::vns::vbi_nor_a); // Set Vertex normals
			glVertexAttribPointer(graphics::vns::vbi_nor_a, 3, GL_FLOAT, GL_FALSE, sizeof(vert_blend), (void*)graphics::vns::vb_nor_a);
			glEnableVertexAttribArray(graphics::vns::vbi_nor_b);
			glVertexAttribPointer(graphics::vns::vbi_nor_b, 3, GL_FLOAT, GL_FALSE, sizeof(vert_blend), (void*)graphics::vns::vb_nor_b);

			glEnableVertexAttribArray(graphics::vns::vbi_uvc); // Set Vertex texture coords
			glVertexAttribPointer(graphics::vns::vbi_uvc, 2, GL_FLOAT, GL_FALSE, sizeof(vert_blend), (void*)graphics::vns::vb_uvc);

			glBindVertexArray(0); // Bind default vertex array
			free(vces);
			free(ices);

			std::cout << "Generated mesh blend!" << std::endl;
		}
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- GUIBITMAP ------------------------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	GUIBitmap::GUIBitmap()
	{
	}
	void GUIBitmap::Init()
	{
		// set up vertex data (and buffer(s)) and configure vertex attributes
		//float vertices[] = {
		//	// positions          // colors           // texture coords
		//	1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		//	1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		//	-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		//	-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
		//};
		float vertices[] = { //invert Y
							 // positions          // colors           // texture coords
			1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // top right
			1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // bottom right
			-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, // bottom left
			-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f  // top left 
		};
		//ccw
		unsigned int indices[] = {
			0, 3, 1, // first triangle
			1, 3, 2  // second triangle
		};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
	void GUIBitmap::SetTexture(GLuint tex)
	{
		tid = tex;
	}
	void GUIBitmap::SetShader(Shader* shader)
	{
		s = shader;
	}
	void GUIBitmap::Draw(int posx, int posy, int w, int h)
	{
		// create transformations
		glm::mat4 transform = glm::mat4(1.0f);
		//scale to screen space

		//transform = glm::scale(transform, glm::vec3(2.f / cfg::iWinX, 2.f / cfg::iWinY, 1.0f));
		transform = glm::scale(transform, glm::vec3(4.f / cfg::iWinX, 2.f / cfg::iWinY, 1.0f));
		transform = glm::translate(transform, glm::vec3(posx, posy, 0));
		transform = glm::scale(transform, glm::vec3(w / 2, h / 2, 1.0f));

		//scale 0.5 height for orthographic
		//transform = glm::scale(transform, glm::vec3(1.f, 0.5f, 1.0f));
		//rotate for ortho (clipping range problem but has depth buffer)
		//transform = glm::rotate(transform, 45.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		//spin rotate
		//transform = glm::rotate(transform, (float)time, glm::vec3(0.0f, 0.0f, 1.0f));

		//transform = glm::perspective(glm::radians(45.0f), (float)winx / (float)winy, 1.0f, 10.0f);

		// get matrix's uniform location and set matrix
		unsigned int transformLoc = glGetUniformLocation(s->ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		//....................................... DRAW

		glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
		glBindTexture(GL_TEXTURE_2D, tid);
		s->Use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}












	// -----------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// (copied from ltr, yet to be optimized, integrated or fixed

	vert createvert(float x, float y, float z, float r, float g, float b, float u, float v)
	{
		vert v2;

		v2.pos.x = x;
		v2.pos.y = y;
		v2.pos.z = z;

		v2.col.r = r;
		v2.col.g = g;
		v2.col.b = b;

		v2.uvc.x = u;
		v2.uvc.y = v;

		return v2;
	}
	struct quad
	{
		float x1, x2, y1, y2, u1, u2, v1, v2;
		quad(float _x1, float _x2, float _y1, float _y2, float _u1, float _u2, float _v1, float _v2)
		{
			x1 = _x1; x2 = _x2;
			y1 = _y1; y2 = _y2;
			u1 = _u1; u2 = _u2;
			v1 = _v1; v2 = _v2;
		}
	};
	void AddQuad(std::vector<vert>& vertices, std::vector<unsigned int>& indices, quad p, int index)
	{
		// Create vertices
		vertices.push_back(createvert(p.x1, p.y2, 0.f, 1.f, 1.f, 1.f, p.u1, p.v1)); // Top left
		vertices.push_back(createvert(p.x2, p.y2, 0.f, 1.f, 1.f, 1.f, p.u2, p.v1)); // Top right
		vertices.push_back(createvert(p.x1, p.y1, 0.f, 1.f, 1.f, 1.f, p.u1, p.v2)); // Bottom left
		vertices.push_back(createvert(p.x2, p.y1, 0.f, 1.f, 1.f, 1.f, p.u2, p.v2)); // Bottom right
		// Create indices
		indices.push_back(0 + index); // Top Left Tri
		indices.push_back(2 + index); // -
		indices.push_back(1 + index); // -
		indices.push_back(3 + index); // Bottom Right Tri
		indices.push_back(1 + index); // -
		indices.push_back(2 + index); // -
	}

	//--------------------------------***************************************************************************
	//--------------------------------***************************************************************************
	//--------------------------------***************************************************************************
	//--------------------------------***************************************************************************

	void GUIBox::Init()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		initialized = true;
	}
	void GUIBox::ReGen(bti16 _xA, bti16 _xB, bti16 _yA, bti16 _yB, btui16 mg, btui16 bl)
	{
		if (!initialized) Init();

		// create transformations
		transform = glm::mat4(1.0f);

		//manual vector version
		std::vector<vert> vertices;
		std::vector<unsigned int> indices;

		bti16 xA = _xA - bl;
		bti16 xB = _xB + bl;
		bti16 yA = _yA - bl;
		bti16 yB = _yB + bl;

		AddQuad(vertices, indices, quad(xA, xA + mg, yA, yA + mg, 0.f, 0.5f, 0.5f, 1.f), 0); // Down Left
		AddQuad(vertices, indices, quad(xA + mg, xB - mg, yA, yA + mg, 0.5f, 0.5f, 0.5f, 1.f), 4); // Down Mid
		AddQuad(vertices, indices, quad(xB - mg, xB, yA, yA + mg, 0.5f, 1.f, 0.5f, 1.f), 8); // Down Right
		AddQuad(vertices, indices, quad(xA, xA + mg, yA + mg, yB - mg, 0.f, 0.5f, 0.5f, 0.5f), 12); // Mid Left
		AddQuad(vertices, indices, quad(xA + mg, xB - mg, yA + mg, yB - mg, 0.5f, 0.5f, 0.5f, 0.5f), 16); // Mid Mid
		AddQuad(vertices, indices, quad(xB - mg, xB, yA + mg, yB - mg, 0.5f, 1.f, 0.5f, 0.5f), 20); // Mid Right
		AddQuad(vertices, indices, quad(xA, xA + mg, yB - mg, yB, 0.f, 0.5f, 0.f, 0.5f), 24); // Up Left
		AddQuad(vertices, indices, quad(xA + mg, xB - mg, yB - mg, yB, 0.5f, 0.5f, 0.f, 0.5f), 28); // Up Mid
		AddQuad(vertices, indices, quad(xB - mg, xB, yB - mg, yB, 0.5f, 1.f, 0.f, 0.5f), 32); // Up Right

		BindBuffers(vertices, indices, VAO, VBO, EBO);

		// create transformations
		transform = glm::mat4(1.0f);
		//scale to screen space
		transform = glm::scale(transform, glm::vec3(4.f / cfg::iWinX, 2.f / cfg::iWinY, 1.0f));
	}
	void GUIBox::Draw(Shader* s, Texture* t)
	{
		glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
		glBindTexture(GL_TEXTURE_2D, t->glID);
		s->Use();

		// get matrix's uniform location and set matrix
		unsigned int transformLoc = glGetUniformLocation(s->ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		glm::mat4 mattemp = glm::mat4(1);

		// get matrix's uniform location and set matrix
		unsigned int offsetLoc = glGetUniformLocation(s->ID, "offset");
		glUniformMatrix4fv(offsetLoc, 1, GL_FALSE, glm::value_ptr(mattemp));

		// Draw

		glBindTexture(GL_TEXTURE_2D, t->glID);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6 * 9, GL_UNSIGNED_INT, 0);
	}

	void GUIText::Init()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		initialized = true;
	}
	void GUIText::SetOffset(int X, int Y)
	{
		// create transformations
		offset = glm::mat4(1.0f);
		//add offset
		offset = glm::translate(offset, glm::vec3(X, Y, 0));
	}

	void GUIText::GetTextBounds()
	{

	}

	//backup
	//void GUIText::ReGen(char* c, unsigned int stringlength, bti16 _xa, bti16 _xb, bti16 _y)

	void GUIText::ReGen(char* c, bti16 _xa, bti16 _xb, bti16 _y)
	{
		if (!initialized) Init();

		xa = _xa; xb = _xb; y = _y;

		int letterspace = 1;

		//reset lines number
		lines = 1;

		// create transformations
		transform = glm::mat4(1.0f);

		//manual vector version
		std::vector<vert> vertices;
		std::vector<unsigned int> indices;

		//character height
		int ch = 12;

		int x = xa; int y2 = y - ch - 2;

		cnum = 0;
		width = 0;
		lastLineWidth = 0;

		sizex = sizey = 0u;
		sizey += ch;

		btui32 stringlength = strlen(c);

		//print text
		if (stringlength > 0)
		{
			lines++;

			int last_space = 0;

			//insert linebreaks
			for (int i = 0; i <= stringlength; i++)
			{
				if (c[i] == CHARCODE::space)
				{
					last_space = i;
				}
				else
				{
					//line break if we've gone past the edge
					if (x + font_kerning[c[i]] + 2 > xb)
					{
						if (last_space != 0) c[last_space] = CHARCODE::LB;
						x = xa;
						y2 -= ch;
						lines++;
						sizey += ch;
						lastLineWidth = 0;
					}
					else
					{
						cnum++;
						x += font_kerning[c[i]] + letterspace;
						width += font_kerning[c[i]] + letterspace;
						lastLineWidth += font_kerning[c[i]] + letterspace;
						if (lastLineWidth > sizex)
							sizex += font_kerning[c[i]] + letterspace;
					}
				}
			}

			x = xa; y2 = y - ch - 2;

			cnum = 0;
			width = 0;
			lastLineWidth = 0;

			sizex = sizey = 0u;
			sizey += ch;

			// print
			for (int i = 0; i <= stringlength; i++)
			{
				//set UV coords
				int u = c[i] % 16;
				int v = floor(c[i] / 16);
				////line break if we've gone past the edge
				//if (c[i] == CHARCODE::LB)
				//{
				//	x = xa;
				//	y2 -= ch;
				//	lines++;
				//	sizey += ch;
				//	lastLineWidth = 0;
				//}
				//else
				//{
				//	AddQuad(vertices, indices, quad(x, x + 8, y2, y2 + 16, (float)(u) / 16.f, (float)(u + 1) / 16.f, (float)v / 16, (float)(v + 1.f) / 16.f), cnum * 4);
				//	cnum++;
				//	x += font_kerning[c[i]] + letterspace;
				//	width += font_kerning[c[i]] + letterspace;
				//	lastLineWidth += font_kerning[c[i]] + letterspace;
				//	if (lastLineWidth > sizex)
				//		sizex += font_kerning[c[i]] + letterspace;
				//}

				AddQuad(vertices, indices, quad(x, x + 8, y2, y2 + 16, (float)(u) / 16.f, (float)(u + 1) / 16.f, (float)v / 16, (float)(v + 1.f) / 16.f), cnum * 4);
				cnum++;
				x += font_kerning[c[i]] + letterspace;
				width += font_kerning[c[i]] + letterspace;
				lastLineWidth += font_kerning[c[i]] + letterspace;
				if (lastLineWidth > sizex)
					sizex += font_kerning[c[i]] + letterspace;
				if (c[i] == CHARCODE::LB)
				{
					x = xa;
					y2 -= ch;
					lines++;
					sizey += ch;
					lastLineWidth = 0;
				}
			}
		}
		else
		{
			//set UV coords
			int u = 0xa1 % 16;
			int v = floor(0xa1 / 16);
			AddQuad(vertices, indices, quad(x, x + 8, y2, y2 + 16, (float)(u) / 16.f, (float)(u + 1) / 16.f, (float)v / 16, (float)(v + 1.f) / 16.f), cnum * 4);
		}

		BindBuffers(vertices, indices, VAO, VBO, EBO);

		// create transformations
		transform = glm::mat4(1.0f);

		//scale to screen space
		transform = glm::scale(transform, glm::vec3(4.f / cfg::iWinX, 2.f / cfg::iWinY, 1.0f));
	}
	//the 'scroll' boolean feels kind of clunky but it will work for now
	void GUIText::Draw(Shader* s, Texture* t)
	{
		s->Use();

		// get matrix's uniform location and set matrix
		unsigned int transformLoc = glGetUniformLocation(s->ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		// get matrix's uniform location and set matrix
		unsigned int offsetLoc = glGetUniformLocation(s->ID, "offset");
		glUniformMatrix4fv(offsetLoc, 1, GL_FALSE, glm::value_ptr(offset));

		//....................................... DRAW

		glBindTexture(GL_TEXTURE_2D, t->glID);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, cnum * 6, GL_UNSIGNED_INT, 0);
	}
}