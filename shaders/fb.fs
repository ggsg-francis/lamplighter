#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

//1px blur
//const float offsetx = 1.0 / 640.0;
//const float offsety = 1.0 / 480.0;
//closest to .5 ps blur
//const float offsetx = 0.9 / 640.0;
//const float offsety = 0.9 / 480.0;

const float offsetx = 0.8 / 640.0;
const float offsety = 0.8 / 480.0;

const float tilesizex = 640 / 1;

float dither(float color, float index_value) {
	float closestColor = (color < 0.5) ? 0 : 1;
	float secondClosestColor = 1 - closestColor;
	float distance = abs(closestColor - color);
	return (distance < index_value) ? closestColor : secondClosestColor;
}

void main()
{ 
    //FragColor = texture(screenTexture, TexCoords);
	
	const int indexMatrix4x4[16] = int[](
		1,	8,	2,	10,
		12,	4,	14,	6,
		3,	11,	1,	9,
		14,	7,	13,	5);
		
	// normally the first one is zero
	const int indexMatrix8x8[64] = int[](
		1,32,8,40,2,34,10,42,
		48,16,56,24,50,18,58,26,
		12,44,4,36,14,46,6,38,
		60,28,52,20,62,30,54,22,
		3,35,11,43,1,33,9,41,
		51,19,59,27,49,17,57,25,
		15,47,7,39,13,45,5,37,
		63,31,55,23,61,29,53,21);
	
	/*
	float indexValue(void) {
		int x = int(mod(gl_FragCoord.x, 4));
		int y = int(mod(gl_FragCoord.y, 4));
		return indexMatrix4x4[(x + y * 4)] / 16.0;
	}

	float dither(float color) {
		float closestColor = (color < 0.5) ? 0 : 1;
		float secondClosestColor = 1 - closestColor;
		float d = indexValue();
		float distance = abs(closestColor - color);
		return (distance < d) ? closestColor : secondClosestColor;
	}
	*/
	

	vec2 offsets[9] = vec2[](
        vec2(-offsetx,  offsety), // top-left
        vec2( 0.0f,    offsety), // top-center
        vec2( offsetx,  offsety), // top-right
        vec2(-offsetx,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offsetx,  0.0f),   // center-right
        vec2(-offsetx, -offsety), // bottom-left
        vec2( 0.0f,   -offsety), // bottom-center
        vec2( offsetx, -offsety)  // bottom-right    
    );
	
	float kernel[9] = float[](
		1.0f/16,2.0f/16,1.0f/16,
		2.0f/16,4.0f/16,2.0f/16,
		1.0f/16,2.0f/16,1.0f/16
	);
    	
	// blur part
	//sample diffuse colour
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
		
	//dither part (wip)
	//col.r = round(col.r * 16) / 16;
	//col.g = round(col.g * 16) / 16;
	//col.b = round(col.b * 16) / 16;
    
	
	//int dx = int(mod(gl_FragCoord.x, 4));
	//int dy = int(mod(gl_FragCoord.y, 4));
	//float index_value = indexMatrix4x4[(dx + dy * 4)] / 16.0;
	
	int dx = int(mod(gl_FragCoord.x, 8));
	int dy = int(mod(gl_FragCoord.y, 8));
	float index_value = indexMatrix8x8[(dx + dy * 8)] / 16.0;
	
	/*float closestColor = (col.r < 0.5) ? 0 : 1;
	float secondClosestColor = 1 - closestColor;
	float distance = abs(closestColor - col.r);
	float red = (distance < index_value) ? closestColor : secondClosestColor;*/
	
	float red = 0;
	float grn = 0;
	float blu = 0;
	
	/*
	if (col.r < 0.125f)
		red = dither(clamp(col.r * 8,0,1), index_value) * 0.125f;
	else if (col.r < 0.25f)
		red = dither(clamp((col.r - 0.125f) * 8,0,1), index_value) * 0.125f + 0.125f;
	else if (col.r < 0.375f)
		red = dither(clamp((col.r - 0.25f) * 8,0,1), index_value) * 0.125f + 0.25f;	
	else if (col.r < 0.5f)
		red = dither(clamp((col.r - 0.375f) * 8,0,1), index_value) * 0.125f + 0.25f;	
	else if (col.r < 0.625f)
		red = dither(clamp((col.r - 0.5f) * 8,0,1), index_value) * 0.125f + 0.5f;	
	
	if (col.g < 0.125f)
		grn = dither(clamp(col.g * 8,0,1), index_value) * 0.125f;
	else if (col.g < 0.25f)
		grn = dither(clamp((col.g - 0.125f) * 8,0,1), index_value) * 0.125f + 0.125f;
	else if (col.g < 0.5f)
		grn = dither(clamp((col.g - 0.25f) * 8,0,1), index_value) * 0.125f + 0.25f;			

	if (col.b < 0.125f)
		blu = dither(clamp(col.b * 8,0,1), index_value) * 0.125f;
	else if (col.b < 0.25f)
		blu = dither(clamp((col.b - 0.125f) * 8,0,1), index_value) * 0.125f + 0.125f;
	else if (col.b < 0.5f)
		blu = dither(clamp((col.b - 0.25f) * 8,0,1), index_value) * 0.125f + 0.25f;	
	*/
	
	//darken
	FragColor = vec4(col - (0.045 * (1 - col)), 1.0);
	//normal
	//FragColor = vec4(col, 1.0);
    //FragColor = vec4(red, grn, blu, 1.0);
}