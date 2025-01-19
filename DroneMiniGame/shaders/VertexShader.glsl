#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;  // Not used in this shader.
layout(location = 3) in vec3 normal; // Not used in this shader.

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Output to fragment shader
out float elevationNoise;  // Renamed 'val' to 'elevationNoise' for clarity

// Function to generate random values
float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9998,78.233)))
                 * 43758.5453123);
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
    (c - a)* u.y * (1.0 - u.x) +
    (d - b) * u.x * u.y;
}

void main() {
    // Compute noise based on the XZ components of the position

    vec3 worldPos = (Model * vec4(position, 1.0f)).xyz;
    vec2 st = vec2(worldPos.x, worldPos.z) * 0.1;  // Scaling factor for noise frequency

    float n = noise(st * 1.5f) * 1.3f;
    worldPos.y = 0;
    // Create a new position by adding the noise value to the Y component of the original position
//    vec3 modifiedPosition = mix(worldPos, worldPos + vec3(0, 1, 0), n);  // Only Y is affected
    vec3 modifiedPosition = position;
    modifiedPosition.y += n;
    // Pass the noise value to the fragment shader
    elevationNoise = n;

    // Compute the final position of the vertex
    gl_Position = Projection * View * Model * vec4(modifiedPosition, 1.0);
}
