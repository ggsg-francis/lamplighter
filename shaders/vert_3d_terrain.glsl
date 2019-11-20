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
	
	Normal = normalize(vec3(matm * vec4(aNormal, 0.0)));
	
    //gl_Position = projection * view * model * vec4(aPos, 1.0);

	vec4 pos2 = matm * vec4(aPos, 1.0);
	
	float f = texture(thm, vec2(-pos2.z + 0.5f, pos2.x + 0.5f) / 2048.f).r * 64.f;
	//float f = texture(thm, vec2(-pos2.z - 0.5f, pos2.x - 0.5f) / 128.f).r * 32.f;
	//f = 1.f;
	pos2.y += f;
	
	Pos = pos2.xyz;
	
	float fx = texture(thm, vec2(-pos2.z + 1.5f, pos2.x + 0.5f) / 2048.f).r * 64.f;
	float fx2 = texture(thm, vec2(-pos2.z - 0.5f, pos2.x + 0.5f) / 2048.f).r * 64.f;
	
	float fz = texture(thm, vec2(-pos2.z + 0.5f, pos2.x + 1.5f) / 2048.f).r * 64.f;
	float fz2 = texture(thm, vec2(-pos2.z + 0.5f, pos2.x - 0.5f) / 2048.f).r * 64.f;
	Normal.x = (f - fz) + (fz2 - f);
	Normal.x /= 2.f; // terrain height division is 4
	
	Normal.z = (fx - f) + (f - fx2);
	Normal.z /= 2.f; // terrain height division is 4
	
	Normal.y = 1;
	

    gl_Position = matp * matv * pos2;
	
	Col = aCol;
	
	//affine projection
	//gl_Position.xyz /= clamp(gl_Position.w * fov, 0, 1024); // 1024 is max number
	//gl_Position.w = 1;
	//imprecise projection
    //gl_Position.x = floor(gl_Position.x * vert_precision) / vert_precision;
    //gl_Position.y = floor(gl_Position.y * vert_precision) / vert_precision;
	
	LightSpacePos = lightProj * vec4(Pos, 1.0);
	
	//vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    //vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    //vs_out.TexCoords = aTexCoords;
    //vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    //gl_Position = projection * view * model * vec4(aPos, 1.0);
}
