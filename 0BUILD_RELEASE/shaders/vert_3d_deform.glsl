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

// Bone forward vectors
uniform vec3 bfwa,bfwb,bfwc,bfwd;

uniform vec3 pcam;

// Shading variables

uniform float ft; // Time

uniform sampler2D tlm; // texture lightmap
uniform sampler2D thm; // texture heightmap
uniform sampler2D ts; // texture sky
uniform sampler2D tshadow; // texture shadow

uniform vec3 vsun = normalize(vec3(-1,1,-1));
uniform vec3 cSun = vec3(0.15,0.1,0.1);
uniform vec3 cAmb = vec3(0.,0.,-0.05);
uniform vec3 cFog = vec3(0.1,0.1,0.1);
uniform vec3 cLit = vec3(1.5f,1.5f,1.5f);
uniform vec3 edgcol = vec3(0.5f,0.5f,0.5f);

uniform bool lit = true;

mat4 mixm(mat4 a, mat4 b, float f)
{
	mat4 m;
	m[0] = mix(a[0], b[0], f);
	m[1] = mix(a[1], b[1], f);
	m[2] = mix(a[2], b[2], f);
	m[3] = mix(a[3], b[3], f);
	return m;
}

void main()
{
	TexCoords = aTexCoords;
	
	//Normal = normalize(vec3(matma * vec4(aNormal, 0.0)));
	vec3 nora = vec3(matma * vec4(aNormal, 0.0));
	vec3 norb = vec3(matmb * vec4(aNormal, 0.0));
	vec3 norc = vec3(matmc * vec4(aNormal, 0.0));
	vec3 nord = vec3(matmd * vec4(aNormal, 0.0));
	Normal = normalize(mix(mix(mix(nora, norb, aDeform[0]), norc, aDeform[1]), nord, aDeform[2]));
	
	
	
	mat4 matr = mixm(mixm(mixm(matma, matmb, aDeform[0]), matmc, aDeform[1]), matmd, aDeform[2]);
	
	// Thiccness preservation, normalize the matrix size
	// TODO: use bone local forward vector, lerp between un-normalized and normalized vectors based on local vector's x, y, z
	//vec3 v = normalize(vec3(matr[2][0],matr[2][1],matr[2][2])); // normalize forward
	//matr[2][0] = v.x; matr[2][1] = v.y; matr[2][2] = v.z;
	vec3 v = normalize(vec3(matr[0][0],matr[0][1],matr[0][2])); // normalize right
	matr[0][0] = v.x; matr[0][1] = v.y; matr[0][2] = v.z;
	v = normalize(vec3(matr[1][0],matr[1][1],matr[1][2])); // normalize up
	matr[1][0] = v.x; matr[1][1] = v.y; matr[1][2] = v.z;
	
	//mat4 matr = mixm(matma, matmb, aDeform[0]);
	
	vec4 posa = matma * vec4(aPos, 1.0);
	vec4 posb = matmb * vec4(aPos, 1.0);
	vec4 posc = matmc * vec4(aPos, 1.0);
	vec4 posd = matmd * vec4(aPos, 1.0);
	
	//vec4 posTemp = mix(mix(mix(posa, posb, aDeform[0]), posc, aDeform[1]), posd, aDeform[2]);
	
	
	
	vec4 posTemp = matr * vec4(aPos * vec3(1.f, 1.f, 0.f), 1.0);
	
	posTemp.xyz += aPos.z * vec3(matr[2][0],matr[2][1],matr[2][2]);
	
	//posTemp.rgb += clamp(aDeform[3], -0.75f * 0.5f, 0.75f * 0.5f) * vec3(matmb[0][0],matmb[0][1],matmb[0][2]);
	//posTemp.rgb += clamp(aDeform[3], -0.75f * 0.5f, 0.75f * 0.5f) * vec3(matmb[1][0],matmb[1][1],matmb[1][2]);
	//if (aDeform[0] > 0.45 && aDeform[0] < 0.55)
		//posTemp.rgb += ((1 - aDeform[0]) - aDeform[1]) * vec3(matr[2][0],matr[2][1],matr[2][2]);
	
	
	Pos = posTemp.rgb;
	
	//gl_Position = matp * matv * mix(mix(mix(posa, posb, aDeform[0]), posc, aDeform[1]), posd, aDeform[2]);
	gl_Position = matp * matv * posTemp;
	
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
		vec3 spec_light = vec3(pow(max(dot(normalize(Normal), normalize(vd)), 0.0), 6.0));
		
		// Mix between diffuse and metallic
		LC = mix(LC,
			(cAmb * ndotl_amb) // Ambient light
			+ (cSun * max(spec_light * 2, edge_light)) // Sunlight (and rim light)
			+ (cLit * light_value * (edge_light * 2.f)) // err
			+ vec3(Col.g), Col.b); // Emissive vertices
	}
	else
	{
		//LC *= ndotl * 2.f; Moon shading
		LC = vec3(1.5f);
	}
	
	//LC = vec3((1 - aDeform[1]) - aDeform[0]) * 5.f;
	//LC = vec3((1 - aDeform[1])) * 5.f;
}
