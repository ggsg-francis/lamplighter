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

const float PI = 3.1415926535; // for fisheye

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
		
	col = vec3(texture(screenTexture, TexCoords));		
	
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
	
	//darken
	//FragColor = vec4(col - (0.045 * (1 - col)), 1.0);
	
	const float exposure = 2.2;
	const float gamma = 0.6;
    // Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-col * exposure);
    // Gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
	col = mapped;
	
	//bleach overflow
	float highestOverflow = max(max(col.r, col.g), col.b);
	//if (highestOverflow > 1.f)
		//col += highestOverflow - 1.f;
		col = mix(col, vec3(1,1,1), clamp((highestOverflow - 0.9f) * 0.75f, 0.f, 1.f));
	FragColor.rgb = col;
	
	//FragColor.rgb = col - 0.5f;
	
	//FragColor.rgb = vec3(1.f,1.f,0.f);
	
	//normal
	//FragColor = vec4(col, 1.0);
    //FragColor = vec4(red, grn, blu, 1.0);
	
	// fisheye
	/*
	float aperture = 178.0;
	float apertureHalf = 0.5 * aperture * (PI / 180.0);
	float maxFactor = sin(apertureHalf);
	vec2 uv;
	vec2 xy = 2.0 * TexCoords.xy - 1.0;
	float d = length(xy);
	if (d < (2.0-maxFactor)) {
		d = length(xy * maxFactor);
		float z = sqrt(1.0 - d * d);
		float r = atan(d, z) / PI;
		float phi = atan(xy.y, xy.x);
		uv.x = r * cos(phi) + 0.5;
		uv.y = r * sin(phi) + 0.5;
	}
	else { uv = TexCoords.xy; }
	vec4 c = texture2D(screenTexture, uv);
	FragColor = c;
	*/
	
	//modified fisheye
	/*
	vec2 uv = TexCoords;
	vec2 xy = 1.5 * TexCoords.xy - 0.75;
	float d = length(xy) * 1.5;
	uv = ((uv - 0.5) * d) + 0.5;
	
	vec4 c = texture2D(screenTexture, uv);
	FragColor = c;
	//FragColor = vec4(d);
	*/
}