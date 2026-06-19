#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1; 

void main()
{
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    
    // Si el píxel es transparente, no lo dibujes para que no genere cuadros grises
    if(texColor.a < 0.1)
        discard;
        
    FragColor = texColor;
}
