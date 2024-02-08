#version 460
#define MAX_POINT_LIGHTS 24
layout(location=0) out vec4 vFragColor;


#if __VERSION__ > 410
layout(std140, binding=0) uniform Modifiers {
#else
    layout(std140) uniform Modifiers {
    #endif
    vec4  Kd;
    vec3  Ks;
    vec4  Ka;
    bool  use_map_Kd;
    float Ns;
	bool use_map_Ka; //14
    bool use_map_Ks; //16
    bool use_map_Ns; //17
} material;

struct PointLight {
    vec3 position_in_ws;
    vec3 position_in_view_space;
    vec3 color;
    vec3 atn;
} ;

layout(std140, binding=3) uniform Lights {
    PointLight light[MAX_POINT_LIGHTS];
    int num_lights;
} p_light;

in vec2 vertex_texcoords_0;
in vec3 vertex_normal_in_viewspace;
in vec3 vertex_coords_in_viewspace;
in vec4 vertex_position;
in vec3 vertex_normals;

uniform sampler2D map_Ka;
uniform sampler2D map_Kd;
uniform sampler2D map_Ks;

uniform sampler2D maps[3];
uniform vec3 ambient_light;

void main() {
    vec3 normal = normalize(vertex_normals);
    vec3 diffuse_light, specular_light;

    float Ns = material.Ns;
	vec4 Ka = material.Ka;
    vec3 Ks = material.Ks;
    vec4 Kd = material.Kd;

    if (material.use_map_Kd) {
        Kd *= texture(map_Kd, vertex_texcoords_0);
    }

    for(int i = 0; i < p_light.num_lights; ++i)
    {
        PointLight light = p_light.light[i];

        vec3 light_dir = normalize(light.position_in_ws - vertex_position.xyz);
        float diff = max(dot(normal, light_dir), 0.0f);
        diffuse_light += diff * light.color;

        vec3 viewDir = -normalize(vertex_coords_in_viewspace);
        vec3 reflect_dir = reflect(-light_dir, normal);
        float specular = pow(max(dot(viewDir, reflect_dir), 0.0f), Ns);
        specular_light += specular * Ks;
    }

    vFragColor.a = Kd.a;
    vFragColor.rgb = Kd.rgb * (diffuse_light + ambient_light + specular_light);
}