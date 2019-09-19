#version 330 core
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

uniform float blendState = 0.5f;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightProj;

//uniform int vert_precision = 256;
uniform int vert_precision = 192;
uniform float fov = 1.25f;
uniform float clipMult = 0.05f;

uniform sampler2D thm; // texture heightmap

// normalized size of vertical planes based on the 4x3 topdown view
const float z_offset_mult = 0.661437809;

void main()
{
	TexCoords = aTexCoords;
	
	Normal = normalize(vec3(model * vec4(aNor1, 0.0)));
	
	//Height = clamp((model * vec4(aPos, 1.0)).y + 0.25, 0, 1);
	
	//precise projection
	//apos2 halved only temporarily
	vec4 pos2 = model * vec4(mix(aPos1, aPos2, blendState), 1.0);
	Pos = pos2.xyz;
	
    gl_Position = projection * view * pos2;
	
	//gl_Position.xy = round(gl_Position.xy * 128) / 128;
	//gl_Position.xyz = clamp(gl_Position.xyz * gl_Position.w, 0, 1024);
	//gl_Position.w = round(gl_Position.w * 16) / 16;

	Col = vec4(1.f);
	
	//affine projection
	//gl_Position.xyz /= clamp(gl_Position.w * fov, 0, 1024); // 1024 is max number
	//gl_Position.w = 1;
	//imprecise projection
    //gl_Position.x = floor(gl_Position.x * vert_precision) / vert_precision;
    //gl_Position.y = floor(gl_Position.y * vert_precision) / vert_precision;
	
	LightSpacePos = lightProj * vec4(Pos, 1.0);
}