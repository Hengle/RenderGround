#version 450

#extension GL_ARB_separate_shader_objects : enable
layout(early_fragment_tests) in;


layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform PUniformBufferObject {
    vec4 CameraInfo;
    vec4 ScreenInfo;
} pubo;


layout(location = 0) out vec4 outColor;

void main() {
    //outColor = vec4(fragColor, 1.0);
    outColor = vec4(0,10,0, 1.0);
    //outColor = texture(texSampler, fragTexCoord);
    //outColor = outColor*outColor;
}