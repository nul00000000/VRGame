#version 400 core

flat in vec3 colors[3];
flat in vec3 coords[3];
//in vec3 colorPass;
flat in vec3 normal;
in vec3 pos;

out vec4 fragColor;

uniform vec3 light[10];
uniform vec3 lightColor[10];
uniform mat4 camera;
uniform float specStrength;
uniform int lightCount;
uniform vec3 skyAmbience;

void main(void) {
    vec3 c1 = pos - coords[0];
    vec3 c2 = pos - coords[1];
    vec3 c3 = pos - coords[2];
    float d1 = dot(c1, c1);
    float d2 = dot(c2, c2);
    float d3 = dot(c3, c3);
    //float d1 = distance(pos, coords[0]);
    //float d2 = distance(pos, coords[1]);
    //float d3 = distance(pos, coords[2]);
    int a = (d1 < d2 && d1 < d3) ? 0 : (d2 < d3 ? 1 : 2);
    vec3 colorPass = colors[a];
    vec3 norm = normalize(normal);
    //vec3 lightDir = normalize(light - fragPos);
    vec3 ambient = vec3(0.1) * colorPass;

    vec3 totalDiffuse = vec3(0);
    for(int i = 0; i < lightCount; i++) {
        vec3 lightDir = normalize(light[i] - pos);
        float diff = clamp(dot(norm, lightDir), 0.0, 1.0);
        //float len = distance(light, fragPos);
        vec3 diffuse = colorPass * lightColor[i] * diff;// / (len * len);
        totalDiffuse += diffuse;
    }

    float sd = clamp(dot(norm, vec3(0, 1, 0)), 0.0, 1.0);
    vec3 sky = colorPass * sd * skyAmbience;

    //Specular
    //vec3 cam = normalize(inverse(camera)[3].xyz - fragPos);
    //vec3 ref = reflect(-lightDir, norm);
    //float cosAlpha = clamp(dot(cam, ref), 0.0, 1.0);
    //vec3 spec = lightColor * specStrength * ceil(diff) * pow(cosAlpha, 5) / (len * len);
    fragColor = vec4(ambient + totalDiffuse + sky, 1.0);
}