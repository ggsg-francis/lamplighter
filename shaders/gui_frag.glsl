#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
	//if (FragColor.a < 0.25)
	//	discard;
	//else FragColor.a = 1.0;
}