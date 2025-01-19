#version 330

in float elevationNoise;  // Color from vertex shader.
layout(location = 0) out vec4 out_color;  // Output color of the pixel.

void main()
{
    vec3 culoare1 = vec3(152.0f/255.0f, 192.0f/255.0f, 0/255.0f);
    vec3 culoare2 = vec3(0.0f/255, 128.0f/255.0f, 0.0f/255.0f);
    out_color = vec4(mix(culoare1, culoare2, elevationNoise), 1.0f);
}
