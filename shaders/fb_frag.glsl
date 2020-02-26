#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform float wx = 640.f;
uniform float wy = 480.f;

const float PI = 3.1415926535; // for fisheye

// normally the first one is zero
const int indexMatrix4x4[16] = int[](
	0,	8,	2,	10,
	12,	4,	14,	6,
	3,	11,	1,	9,
	15,	7,	13,	5);

float kernel[9] = float[](
	1.0f/16,2.0f/16,1.0f/16,
	2.0f/16,4.0f/16,2.0f/16,
	1.0f/16,2.0f/16,1.0f/16
);

/*
float dither(float color, float index_value) {
	float closestColor = (color < 0.5) ? 0 : 1;
	float secondClosestColor = 1 - closestColor;
	float distance = abs(closestColor - color);
	return (distance < index_value) ? closestColor : secondClosestColor;
}*/

const int indexMat4x4PSX[16] = int[]
(
	-4,	0,	-3,	1,
	2,	-2,	3,	-1,
	-3,	1,	-4,	0,
	3,	-1,	2,	-2
);

//*
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
//*/

void main()
{ 
	//FragColor = texture(screenTexture, TexCoords);
	
	// blur part
	//sample diffuse colour
	//vec3 sampleTex[9];
	vec3 sampleTex[18];

	// 0.9 is like PS 0.5px blur
	float ofsx = 1.f / wx;
	float ofsy = 1.f / wy;
	
	sampleTex[0] = vec3(texture(screenTexture, TexCoords.st + vec2(-ofsx, ofsy)));
	sampleTex[1] = vec3(texture(screenTexture, TexCoords.st + vec2(0.0f,  ofsy)));
	sampleTex[2] = vec3(texture(screenTexture, TexCoords.st + vec2(ofsx,  ofsy)));
	sampleTex[3] = vec3(texture(screenTexture, TexCoords.st + vec2(-ofsx, 0.0f)));
	sampleTex[4] = vec3(texture(screenTexture, TexCoords.st + vec2(0.0f,  0.0f)));
	sampleTex[5] = vec3(texture(screenTexture, TexCoords.st + vec2(ofsx,  0.0f)));
	sampleTex[6] = vec3(texture(screenTexture, TexCoords.st + vec2(-ofsx,-ofsy)));
	sampleTex[7] = vec3(texture(screenTexture, TexCoords.st + vec2(0.0f, -ofsy)));
	sampleTex[8] = vec3(texture(screenTexture, TexCoords.st + vec2(ofsx, -ofsy)));
	
	// extra samples
	// Since I am going to come back to this some day and wonder wtf is going on here
	// Basically: I don't know why modx and mody make the blur work so well
	// They were meant to be used the other way around to create a dither effect!
	int modx = int(mod(gl_FragCoord.x, 2));
	int mody = int(mod(gl_FragCoord.y, 2));
	// Right hand side
	sampleTex[9] = vec3(texture(screenTexture, TexCoords.st + vec2(ofsx * (-2 - modx), ofsy)));
	sampleTex[10] = vec3(texture(screenTexture, TexCoords.st + vec2(ofsx * (-2 - modx), 0.0f)));
	sampleTex[11] = vec3(texture(screenTexture, TexCoords.st + vec2(ofsx * (-2 - modx), -ofsy)));
	sampleTex[12] = vec3(texture(screenTexture, TexCoords.st + vec2(ofsx * (-4 - modx), ofsy)));
	sampleTex[13] = vec3(texture(screenTexture, TexCoords.st + vec2(ofsx * (-4 - modx), 0.f)));
	sampleTex[14] = vec3(texture(screenTexture, TexCoords.st + vec2(ofsx * (-4 - modx), -ofsy)));
	// Top side
	sampleTex[15] = vec3(texture(screenTexture, TexCoords.st + vec2(-ofsx, ofsy * (-2 - mody))));
	sampleTex[16] = vec3(texture(screenTexture, TexCoords.st + vec2(0.0f,  ofsy * (-2 - mody))));
	sampleTex[17] = vec3(texture(screenTexture, TexCoords.st + vec2(ofsx,  ofsy * (-2 - mody))));
	
	vec3 col = vec3(0.0);
	for(int i = 0; i < 18; i++)
		col += sampleTex[i] / 18.f;
	
	col.rgb = normalize(col.rgb);
	
	float difR = (col.r - ((col.g + col.b) * 0.5f)) + 0.5f;
	float difG = (col.g - ((col.r + col.b) * 0.5f)) + 0.5f;
	float difB = (col.b - ((col.r + col.g) * 0.5f)) + 0.5f;
	col = vec3(0.0);
	for(int i = 0; i < 9; i++)
		//col += sampleTex[i] / 9.f;
		col += sampleTex[i] * kernel[i];
	float brightness = (col.r + col.g + col.b) / 1.5f;
	col.r = brightness * difR;
	col.g = brightness * difG;
	col.b = brightness * difB;
	//col.r = min(brightness, difR);
	//col.g = min(brightness, difG);
	//col.b = min(brightness, difB);
	
	
	// temp activate to disable blur	
	//vec3 col = vec3(texture(screenTexture, TexCoords));		
	
	//darken
	FragColor = vec4(col - (0.045 * (1 - col)), 1.0);
	
	// Gamma correction
	/*
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
	//*/
	
	FragColor.rgb = col;
	FragColor.a = 1.f - (brightness * 0.08f);
	//FragColor.a = 0.89f;
	
	//FragColor.rgb = col - 0.5f;
	
	//FragColor.rgb = vec3(1.f,1.f,0.f);
	
	//normal
	//FragColor = vec4(col, 1.0);
	//FragColor = vec4(red, grn, blu, 1.0);
	
	//FragColor = texture(screenTexture, TexCoords);
	//FragColor -= texture(noise, TexCoords);
	
	// Dither
	/*
	int dx = int(mod(gl_FragCoord.x, 4));
	int dy = int(mod(gl_FragCoord.y, 4));
	float rndBy = 16.f;
	FragColor.rgb += indexMat4x4PSX[(dx + dy * 4)] / (rndBy * 4.f);
	//FragColor.rgb += (indexMatrix4x4[(dx + dy * 4)] - 8.f) / (rndBy * 4.f);
	// Posterize
	FragColor.rgb = round(FragColor.rgb * rndBy) / rndBy;
	//*/
	
	//FragColor = vec4(0.5f);
	
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
	//*/
	
	//modified fisheye
	/*
	vec2 uv = TexCoords;
	vec2 xy = 1.5 * TexCoords.xy - 0.75;
	float d = length(xy) * 1.5;
	uv = ((uv - 0.5) * d) + 0.5;
	
	vec4 c = texture2D(screenTexture, uv);
	FragColor = c;
	//FragColor = vec4(d);
	//*/
}