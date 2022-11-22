#version 450 core
layout (location = 0) out vec4 aAlbedo;

layout(location = 0) in vec3 f_LocalPos;

layout(binding = 1) uniform samplerCube u_EnvironmentCubemap;

const float kPI = 3.14159265359;

void main()
{
    vec3 normal = normalize(f_LocalPos);
    vec3 irradiance = vec3(0.0);

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for(float phi = 0.0; phi < 2.0 * kPI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * kPI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            irradiance += texture(u_EnvironmentCubemap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = kPI * irradiance * (1.0 / float(nrSamples));

    aAlbedo = vec4(irradiance, 1.0);
}

