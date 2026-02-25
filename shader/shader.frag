#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTex;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform texturePC
{
    int textureIndex;
} tpc;

layout(set = 0, binding = 0) uniform sampler2D textures[];

void main()
{
    if(tpc.textureIndex >= 0)
    {
        outColor = texture(textures[tpc.textureIndex], fragTex);
    }
    else
    {
        outColor = vec4(fragColor, 1.0);
    }
}