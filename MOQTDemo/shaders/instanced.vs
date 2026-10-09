#version 330
in vec3 vertexPosition;
in mat4 instanceTransform;

uniform mat4 mvp;

out vec4 fragColor;

void main()
{
    float r = instanceTransform[0][3];
    float g = instanceTransform[1][3];
    float b = instanceTransform[2][3];
    fragColor = vec4(r, g, b, 1.0);

    vec3 position = vertexPosition + instanceTransform[3].xyz;

    gl_Position = mvp * vec4(position, 1.0);
}