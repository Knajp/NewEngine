#version 450

layout(location = 0) in vec2 GlyphPosition;
layout(location = 1) in vec2 GlyphSize;
layout(location = 2) in vec4 GlyphUV;
layout(location = 3) in vec4 GlyphColor;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec2 FragUV;

const vec2 corners[6] = 
{
    vec2(0,0), vec2(1,0), vec2(1,1),
    vec2(0,0), vec2(1,1), vec2(0,1)
};

layout(binding = 0) uniform ScreenSize
{
    vec2 resolution;
};

void main()
{
    vec2 corner = corners[gl_VertexIndex];
    vec2 pixelPos = GlyphPosition + corner * GlyphSize;

    vec2 ndc = (pixelPos / resolution) * 2.0 - 1.0;
    gl_Position = vec4(ndc.x, -ndc.y, 0.0, 1.0);
    
    FragUV = mix(GlyphUV.xy, GlyphUV.zw, corner);
    FragColor = GlyphColor;

}