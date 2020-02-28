#version 330 core
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

uniform int vert_precision = 256;
//uniform int vert_precision = 192;
uniform float fov = 1.25f;
uniform float clipMult = 0.05f;

// Shading variables

uniform float ft; // Time

uniform sampler2D texture_diffuse1;
uniform sampler2D tlm; // texture lightmap
uniform sampler2D thm; // texture heightmap
uniform sampler2D ts; // texture sky
uniform sampler2D tshadow; // texture shadow

uniform vec3 vsun = normalize(vec3(-1,1,-1));
uniform vec3 csun = vec3(0.15,0.1,0.1);
uniform vec3 camb = vec3(-0.2,-0.2,-0.3);
uniform vec3 fogcol = vec3(0.1,0.1,0.1);
uniform vec3 litcol = vec3(1.5f,1.5f,1.5f);

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
	//Col.r = 1;
	
	/*
	// non-screen-affine projection
	gl_Position.xyz /= clamp(gl_Position.w * fov, 0, 1024); // 1024 is max number
	// imprecise projection
    gl_Position.x = floor(gl_Position.x * vert_precision) / vert_precision;
    gl_Position.y = floor(gl_Position.y * vert_precision) / vert_precision;
	// de-project
	gl_Position.xyz *= clamp(gl_Position.w * fov, 0, 1024); // 1024 is max number
	//*/
	
	LightSpacePos = lightProj * vec4(Pos, 1.0);
	
	// Shading
	
	//float ndotl = clamp(dot(normalize(Normal), vsun), 0, 1);
	float ndotl = clamp(dot(Normal, vsun) * Col.r, 0, 1);
	float ndotl_amb = clamp(dot(Normal, vec3(0,-1,0)) + 0.5f, 0, 1);
	
	if (lit)
	{
		vec4 heightmap = texture(thm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f);
		LC = clamp(mix(camb + (litcol * texture(tlm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f).g) + (csun * ndotl), vec3(1.f), Col.g), 0.0, 10.0);
	}
	else
	{
		//LC *= ndotl * 2.f; Moon shading
		LC = vec3(1.5f);
	}
}
