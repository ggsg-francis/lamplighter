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

// Shading variables

uniform float ft; // Time

uniform sampler2D texture_diffuse1;
uniform sampler2D tlm; // texture lightmap
uniform sampler2D thm; // texture heightmap
uniform sampler2D ts; // texture sky
uniform sampler2D tshadow; // texture shadow

uniform vec3 vsun = normalize(vec3(-1,1,-1));
uniform vec3 csun = vec3(0.15,0.1,0.1);
uniform vec3 camb = vec3(0.,0.,-0.05);
uniform vec3 fogcol = vec3(0.1,0.1,0.1);
uniform vec3 litcol = vec3(1.5f,1.5f,1.5f);
uniform vec3 edgcol = vec3(0.5f,0.5f,0.5f);

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
	
	gl_Position = matp * matv * mix(mix(mix(posa, posb, aDeform[0]), posc, aDeform[1]), posd, aDeform[2]);
	
	Col = aCol;
	
	LightSpacePos = lightProj * vec4(Pos, 1.0);
	
	// Shading
	
	float ndotl = clamp(dot(Normal, vsun) * Col.r, 0, 1);
	float ndotl_amb = clamp(dot(Normal, vec3(0,-1,0)) + 0.5f, 0, 1);
	
	if (lit)
	{
		// WIP
		// edge light
		vec3 vd = Pos - pcam;
		vec3 halfwayDir = normalize(vsun - normalize(-vd));
		vec3 edge_light = edgcol * pow(max(dot(normalize(Normal), halfwayDir), 0.0), 2.0);
	
		vec4 lightmap = texture(tlm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f);
		
		float xoffs = -Pos.z + 0.5f - round(-Pos.z + 0.5f);
		float yoffs = -Pos.x + 0.5f - round(-Pos.x + 0.5f);
		
		float light_value = (1 - clamp(           (((Pos.y / 64.) - lightmap.a))*32.          , 0., 1.)) * lightmap.g;
		//float light_value = mix(
		//	1 - clamp((((pos2.y) / 64.) - lightmap.a)*16., 0., 1.) * lightmap.g,
		//	1 - clamp((((pos2.y + 1) / 64.) - lightmap.a)*16., 0., 1.) * lightmap.g, xoffs);
		LC = clamp(mix(camb + ((litcol + (csun * ndotl) + edge_light) * light_value), vec3(1.f), Col.g), 0.0, 10.0);
		//LC = heightmap.rgb;
	}
	else
	{
		//LC *= ndotl * 2.f; Moon shading
		LC = vec3(1.5f);
	}
}
