#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec4 FragColor;
layout(location = 1) in vec2 FragUV;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform atlasPC
{
    int atlasIndex;
} apc;

layout(set = 0, binding = 1) uniform sampler2D atlases[];

float median(float r, float g, float b)
{
    return max(min(r,g), min(max(r,g),b));
}

void main()
{

    vec3 msdf = texture(atlases[apc.atlasIndex], FragUV).rgb;
    float dist = median(msdf.r, msdf.g, msdf.b);
    float width = fwidth(dist);
    float alpha = smoothstep(0.5 + width, 0.5 - width, dist);

    outColor = vec4(FragColor.rgb, FragColor.a * alpha);
}
