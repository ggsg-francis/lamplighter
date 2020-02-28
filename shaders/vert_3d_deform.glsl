#version 330 core
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
	
	//Normal = normalize(vec3(matma * vec4(aNormal, 0.0)));
	vec3 nora = vec3(matma * vec4(aNormal, 0.0));
	vec3 norb = vec3(matmb * vec4(aNormal, 0.0));
	vec3 norc = vec3(matmc * vec4(aNormal, 0.0));
	vec3 nord = vec3(matmd * vec4(aNormal, 0.0));
	Normal = normalize(mix(mix(mix(nora, norb, aDeform[0]), norc, aDeform[1]), nord, aDeform[2]));
	
	vec4 posa = matma * vec4(aPos, 1.0);
	vec4 posb = matmb * vec4(aPos, 1.0);
	vec4 posc = matmc * vec4(aPos, 1.0);
	vec4 posd = matmd * vec4(aPos, 1.0);
	
	vec4 posTemp = mix(mix(mix(posa, posb, aDeform[0]), posc, aDeform[1]), posd, aDeform[2]);
	Pos = posTemp.rgb;
	
	//gl_Position = matp * matv * mix(pos2, pos3, aDeform[0]);
	//gl_Position = matp * matv * mix(pos2, pos4, aDeform[1]);
	//gl_Position = matp * matv * mix(mix(pos2, pos3, aDeform[0]), pos4, aDeform[1]);
	gl_Position = matp * matv * mix(mix(mix(posa, posb, aDeform[0]), posc, aDeform[1]), posd, aDeform[2]);
	//gl_Position = posTemp;
	
	Col = aCol;

	/*
	//affine projection
	gl_Position.xyz /= clamp(gl_Position.w * fov, 0, 1024); // 1024 is max number
	gl_Position.w = 1;
	//imprecise projection
    gl_Position.x = floor(gl_Position.x * vert_precision) / vert_precision;
    gl_Position.y = floor(gl_Position.y * vert_precision) / vert_precision;
	//*/
	
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
