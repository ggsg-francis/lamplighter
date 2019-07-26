#version 330 core
layout (location = 0) in vec3 aPos1;
layout (location = 1) in vec3 aPos2;
layout (location = 2) in vec3 aNor1;
layout (location = 3) in vec3 aNor2;
layout (location = 4) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out float Height;

uniform float blendState = 0.5f;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//uniform int vert_precision = 256;
uniform int vert_precision = 192;
uniform float fov = 1.25f;
uniform float clipMult = 0.05f;

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
	//skew
	//pos2.z -= pos2.y * 0.7f;
	//pos2.z *= 0.75f;
	//pos2.z -= pos2.y * z_offset_mult;
	
	//set the height value for use in the fragment shader
	//Height = clamp(pos2.y + 0.25, 0, 1);
	
    //gl_Position = projection * view * model * vec4(aPos, 1.0);
	
    gl_Position = projection * view * pos2;

	// Affine perspective transform
	//gl_Position.w = (1 + gl_Position.z) * fov;
	gl_Position.w = 1 + gl_Position.z;
	//affine attempt
	gl_Position.xyz /= clamp(gl_Position.w * fov, 0, 1024); // 1024 is max number
	//if (gl_Position.w > 0.f) gl_Position.xyz /= gl_Position.w * fov; // 1024 is max number
	gl_Position.z *= clipMult;
	gl_Position.w = 1;
	
	//Height = clamp( -gl_Position.z + 0.5, 0, 1);
	Height = gl_Position.z;
	
	//imprecise projection
    gl_Position.x = floor(gl_Position.x * vert_precision) / vert_precision;
    gl_Position.y = floor(gl_Position.y * vert_precision) / vert_precision;
}