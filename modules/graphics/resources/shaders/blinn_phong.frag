#version 430

in vec2 uv;

layout(std140) uniform Material {
    vec3 mat_color;
    //vec3 specular;
    vec3 specular_color;
    float ambient_strength;
    float specular_strength;
    float alpha;
};

layout(std140) uniform BPLightUniform {
    vec3 lightPos;
    vec3 lightColor;
    vec3 ambient_color;
};

in BPLight {
    vec3 fragPos;
    vec3 viewPos;
    vec3 normal;
} vsOut;

out vec4 color;

void main () {
    vec3 lightDir = lightPos - vsOut.fragPos;
    vec3 normLight = normalize(lightDir);
    vec3 normView = normalize(vsOut.viewPos - vsOut.fragPos);

    vec3 halfVec = normalize(normLight + normView);
    vec3 normal = normalize(vsOut.normal);

    vec3 diffuseIntensity = mat_color * max(dot(normLight, normal), 0.0) * lightColor;
    vec3 specularIntensity = specular_color * pow(max(dot(halfVec, normal), 0.0), alpha) * lightColor;
    vec3 ambientIntensity = ambient_color * mat_color;

    vec3 intensity = ambient_strength * ambientIntensity + (1 - specular_strength) * diffuseIntensity + specular_strength * specularIntensity;

    color = vec4(intensity, 1.0);
}