#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform float opa; // Opacity

void main()
{
    FragColor = texture(ourTexture, TexCoord);
	FragColor.a *= opa;
	FragColor.a = clamp(FragColor.a, 0.0, 1.0);
	//if (FragColor.a < 0.25)
	//	discard;
	//else FragColor.a = 1.0;
}