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

// Shading variables

uniform float ft; // Time

uniform sampler2D tlm; // texture lightmap
uniform sampler2D thm; // texture heightmap
uniform sampler2D ts; // texture sky
uniform sampler2D tshadow; // texture shadow

uniform vec3 vsun = normalize(vec3(-1,1,-1));
uniform vec3 cSun = vec3(0.f,0.f,0.f);
//uniform vec3 cAmb = vec3(-0.2,-0.2,-0.3);
uniform vec3 cAmb = vec3(0.,0.,-0.05);
uniform vec3 cFog = vec3(0.1,0.1,0.1);
uniform vec3 cLit = vec3(1.5f,1.5f,1.5f);
uniform vec3 edgcol = vec3(0.5f,0.5f,0.5f);

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
	
	LightSpacePos = lightProj * vec4(Pos, 1.0);
	
	// Shading
	
	float ndotl = clamp(dot(Normal, vsun) * Col.r, 0, 1);
	float ndotl_amb = clamp(dot(Normal, vec3(0,1,0)) * 0.5f + 0.75f, 0, 1);
	
	if (lit)
	{
		// WIP
		// edge light
		vec3 vd = Pos - pcam;
		vec3 halfwayDir = normalize(vsun - normalize(-vd));
		float edge_light = pow(max(dot(normalize(Normal), halfwayDir), 0.0), 2.0);
	
		vec4 lightmap = texture(tlm, vec2(-Pos.z + 0.5f, Pos.x + 0.5f) / 2048.f);
		
		float xoffs = -Pos.z + 0.5f - round(-Pos.z + 0.5f);
		float yoffs = -Pos.x + 0.5f - round(-Pos.x + 0.5f);
		
		float light_value = (1 - clamp(((Pos.y / 64.) - lightmap.a) * 32., 0., 1.)) * lightmap.g;
		
		LC =  (cAmb * ndotl_amb) // Ambient light
			+ (cSun * (ndotl + edge_light)) // Sunlight (and rim light)
			+ (cLit * light_value) // err
			+ vec3(Col.g); // Emissive vertices
		
		edge_light = clamp(dot(Normal, normalize(vd)) * 0.5f + 0.5f, 0, 1);
		halfwayDir = normalize(vsun - normalize(vd));
		float spec_light = pow(max(dot(normalize(Normal), halfwayDir), 0.0), 6.0) * 4.0;
		
		// Mix between diffuse and metallic
		LC = mix(LC,
			(cAmb * edge_light) // Ambient light
			+ (cSun * max(spec_light, edge_light)) // Sunlight (and rim light)
			+ (cLit * light_value * (edge_light * 4.f)) // err
			+ vec3(Col.g), Col.b); // Emissive vertices
			
		//LC = vec3(spec_light);
	}
	else
	{
		//LC *= ndotl * 2.f; Moon shading
		LC = vec3(3.f, 0.f, 0.f);
	}
}
