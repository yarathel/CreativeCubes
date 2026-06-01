#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture_diffuse1; // Nota que en tu main pusiste texture_diffuse1

void main()
{
    vec4 texColor = texture(texture_diffuse1, TexCoord);
    
    // Si la textura es completamente negra o transparente, forzamos un color llamativo (Verde o Azul)
    if(texColor.rgb == vec3(0.0) || texColor.a < 0.1)
    {
        FragColor = vec4(0.0f, 0.8f, 0.3f, 1.0f); // Bloque verde para testear
    }
    else
    {
        FragColor = texColor;
    }
}
