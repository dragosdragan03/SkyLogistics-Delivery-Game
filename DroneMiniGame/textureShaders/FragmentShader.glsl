#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture_1;
uniform sampler2D texture_2;
// TODO(student): Declare various other uniforms
uniform int mix_textures;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Calculate the out_color using the texture2D() function.
    // calculate the out_color using the texture2D() function
    vec4 colour1 = texture(texture_1, texcoord);
    vec4 colour2 = texture(texture_2, texcoord);

    if(mix_textures == 1) {
        out_color = mix(colour1, colour2, .5f);
    }
    out_color = colour1;

    if (out_color.a < .5f) // alpha discard
    {
        discard;
    }

}