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

uniform mat4 matm;
uniform mat4 matv;
uniform mat4 matp;
uniform mat4 lightProj;

uniform vec3 pcam;

uniform float ft; // Time

uniform sampler2D texture_diffuse1;

uniform vec3 vsun = normalize(vec3(-1,1,-1));

//uniform int vert_precision = 256;
uniform int vert_precision = 192;
uniform float fov = 1.25f;
uniform float clipMult = 0.05f;

// normalized size of vertical planes based on the 4x3 topdown view
const float z_offset_mult = 0.661437809;

void main()
{
	TexCoords = aTexCoords;
	
	Normal = normalize(vec3(matm * vec4(aNormal, 0.0)));
	
    //gl_Position = matp * matv * matm * vec4(aPos, 1.0);

	vec4 pos2 = matm * vec4(aPos, 1.0);
	
	Pos = pos2.xyz;
	
    gl_Position = matp * matv * pos2;
	
	//Col = aCol;
	
	vec3 vd = normalize(Pos - pcam);
	
	Col = texture(texture_diffuse1, vec2(ft,
				(dot(vd, vec3(0,-1,0)) * 0.5 + (0.5 - 0.06)) + (dot(vd, vsun) * 0.06)
			));
	
	LightSpacePos = lightProj * vec4(Pos, 1.0);
}
