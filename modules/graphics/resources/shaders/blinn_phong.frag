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
    mat4 BPLight_lightSpace;
    vec3 BPLight_pos;
    vec3 BPLight_dir;
    vec3 BPLight_color;
    vec3 BPLight_ambient_color;
    float BPLight_brightness;
    float BPLight_outer;
    float BPLight_inner;
    int BPLight_type;
    bool BPLight_castShadows;
};

uniform sampler2D ShadowMap;

in BPLight {
    vec3 fragPos;
    vec3 viewPos;
    vec3 normal;
    vec4 fragLightSpacePos;
} vsOut;

out vec4 color;

float CalcShadowSpotDir (vec4 fragLightSpacePos, vec3 normal, vec3 lightDir) {
    vec3 lightSpacePos = fragLightSpacePos.xyz / fragLightSpacePos.w;
    //lightSpacePos.y *= -1;
    if (BPLight_castShadows) {
        lightSpacePos = lightSpacePos * 0.5 + 0.5;

        if (lightSpacePos.z > 1.0) {
            return 1.0;
        }

        float shadowMapDepth = texture(ShadowMap, lightSpacePos.xy).r;
        float maxBias = 0.005;
        float minBias = 0.0005;
        float bias = max(maxBias * (1.0 - dot(normal, lightDir)), minBias);
//        float bias = 0.0;
        return lightSpacePos.z - bias > shadowMapDepth ? 0.0 : 1.0;
    } else {
        return 1.0;
    }
}

float CalcShadowPoint (vec4 lightSpacePos) {
    return 1.0;
}

vec3 CalcLighting (vec3 lightDir, vec3 viewDir, vec3 normal, float attenuation, vec3 matColor, vec3 specularColor, float alpha, vec3 lightColor, float shadow) {
    vec3 halfVec = normalize(lightDir + viewDir);


    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 diffuseIntensity = attenuation * matColor * max(dot(lightDir, normal), 0.0) * lightColor;
    vec3 specularIntensity = attenuation * specularColor * pow(max(dot(halfVec, normal), 0.0), alpha) * lightColor;
    //vec3 specularIntensity = attenuation * specularColor * pow(max(dot(viewDir, reflectDir), 0.0), alpha) * lightColor;
    //vec3 ambientIntensity = BPLight_ambient_color * matColor;
    vec3 ambientIntensity = vec3(0, 0, 0);

    return Material_ambient * ambientIntensity + shadow * (1 - Material_specular) * diffuseIntensity + shadow * Material_specular * specularIntensity;
    //return lightDir * shadow;
    //return vec3(1.0, 1.0, 1.0) * shadow;
}

void main () {
    vec3 normView = normalize(vsOut.viewPos - vsOut.fragPos);
    vec3 lightDir = BPLight_pos - vsOut.fragPos;
    vec3 normLight = normalize(lightDir);
    if (BPLight_type == 0) {
        // Point light
        float attenuation = min(BPLight_brightness / (1 + dot(lightDir, lightDir)), 1.0);
        //float attenuation = BPLight_brightness;
        color = vec4(CalcLighting(normLight, normView, normalize(vsOut.normal), attenuation, Material_color, Material_specular_color, Material_alpha, BPLight_color, CalcShadowPoint(vsOut.fragLightSpacePos)), 0.9);
    } else if (BPLight_type == 1) {
        // Directional light
        color = vec4(CalcLighting(-BPLight_dir, normView, vsOut.normal, BPLight_brightness, Material_color, Material_specular_color, Material_alpha, BPLight_color, CalcShadowSpotDir(vsOut.fragLightSpacePos, vsOut.normal, lightDir)), 0.9);
    } else {
        // Spot light
        float theta = dot(normLight, -BPLight_dir);
        float epsilon = BPLight_inner - BPLight_outer;
        float attenuation = clamp((theta - BPLight_outer) / epsilon, 0.0, 1.0) * min(BPLight_brightness / (1 + dot(lightDir, lightDir)), 1.0);

        color = vec4(CalcLighting(normLight, normView, vsOut.normal, attenuation, Material_color, Material_specular_color, Material_alpha, BPLight_color, CalcShadowSpotDir(vsOut.fragLightSpacePos, vsOut.normal, lightDir)), 0.9);
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