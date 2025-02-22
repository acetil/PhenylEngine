#version 430

in vec2 uv;

layout(std140) uniform Material {
    vec3 Material_color;
    //vec3 specular;
    vec3 Material_specular_color;
    float Material_ambient;
    float Material_specular;
    float Material_alpha;
};

layout(std140) uniform BPLightUniform {
    vec3 BPLight_pos;
    vec3 BPLight_dir;
    vec3 BPLight_color;
    vec3 BPLight_ambient_color;
    float BPLight_brightness;
    float BPLight_outer;
    float BPLight_inner;
    int BPLight_type;
};

in BPLight {
    vec3 fragPos;
    vec3 viewPos;
    vec3 normal;
} vsOut;

out vec4 color;

vec3 CalcLighting (vec3 lightDir, vec3 viewDir, vec3 normal, float attenuation, vec3 matColor, vec3 specularColor, float alpha, vec3 lightColor) {
    vec3 halfVec = normalize(lightDir + viewDir);

    vec3 diffuseIntensity = attenuation * matColor * max(dot(lightDir, normal), 0.0) * lightColor;
    vec3 specularIntensity = attenuation * specularColor * pow(max(dot(halfVec, normal), 0.0), alpha) * lightColor;
    vec3 ambientIntensity = BPLight_ambient_color * matColor;

    return Material_ambient * ambientIntensity + (1 - Material_specular) * diffuseIntensity + Material_specular * specularIntensity;
}

void main () {
    vec3 normView = normalize(vsOut.viewPos - vsOut.fragPos);
    if (BPLight_type == 0) {
        // Point light
        vec3 lightDir = BPLight_pos - vsOut.fragPos;
        vec3 normLight = normalize(lightDir);

        float attenuation = min(BPLight_brightness / (1 + dot(lightDir, lightDir)), 1.0);
        color = vec4(CalcLighting(normLight, normView, vsOut.normal, attenuation, Material_color, Material_specular_color, Material_alpha, BPLight_color), 1.0);
    } else if (BPLight_type == 1) {
        // Directional light
        color = vec4(CalcLighting(-BPLight_dir, normView, vsOut.normal, BPLight_brightness, Material_color, Material_specular_color, Material_alpha, BPLight_color), 1.0);
    } else {
        // Spot light
        vec3 lightDir = BPLight_pos - vsOut.fragPos;
        vec3 normLight = normalize(lightDir);
        float theta = dot(normLight, -BPLight_dir);
        float epsilon = BPLight_inner - BPLight_outer;
        float attenuation = clamp((theta - BPLight_outer) / epsilon, 0.0, 1.0) * min(BPLight_brightness / (1 + dot(lightDir, lightDir)), 1.0);

        color = vec4(CalcLighting(normLight, normView, vsOut.normal, attenuation, Material_color, Material_specular_color, Material_alpha, BPLight_color), 1.0);
    }

//    //vec3 lightDir = lightPos - vsOut.fragPos;
//    vec3 lightDir = BPLight_pos.xyz - vsOut.fragPos;
//
//    vec3 normLight = normalize(lightDir);
//    vec3 normView = normalize(vsOut.viewPos - vsOut.fragPos);
//    float attenuation = min(BPLight_brightness / (1 + dot(lightDir, lightDir)), 1.0);
//
//    color = vec4(CalcLighting(normLight, normView, vsOut.normal, attenuation, Material_color, Material_specular_color, Material_alpha, BPLight_color), 1.0);
}