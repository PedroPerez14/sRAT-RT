#version 450 core

#define MAX_NUM_LIGHTS 256

in vec2 fTexcoords;

layout (location = 0) out vec4 out_color;
layout (location = 1) out vec4 debug_1;
layout (location = 2) out vec4 debug_2;
layout (location = 3) out vec4 debug_3;
layout (location = 4) out vec4 debug_4;
layout (location = 5) out vec4 debug_5;
layout (location = 6) out vec4 debug_6;
layout (location = 7) out vec4 debug_7;

layout (binding = 0) uniform sampler3D LUT_1;
layout (binding = 1) uniform sampler3D LUT_2;
layout (binding = 2) uniform sampler3D LUT_3;
layout (binding = 3) uniform sampler1D tex_wavelengths;
layout (binding = 4) uniform sampler1D resp_curve;
layout (binding = 5) uniform sampler1D framebuffer_tex1;    // pos +  mat_id
layout (binding = 6) uniform sampler1D framebuffer_tex2;    // normal + ???
layout (binding = 7) uniform sampler1D framebuffer_tex3;    // albedo + ???
layout (binding = 8) uniform sampler1D framebuffer_tex4;    // free for materials
layout (binding = 9) uniform sampler1D framebuffer_tex5;    // free for materials
layout (binding = 10) uniform sampler1D framebuffer_tex6;   // free for materials
layout (binding = 11) uniform sampler1D framebuffer_tex7;   // free for materials
layout (binding = 12) uniform sampler1D framebuffer_tex8;   // free for materials

uniform bool do_spectral_uplifting;
uniform bool convert_xyz_to_rgb;
uniform int n_wls;
uniform float wl_min;
uniform float wl_max;
uniform float wl_min_resp;
uniform float wl_max_resp;
uniform int res = 64;
uniform vec3 cam_pos;
uniform int num_lights;

//// SOME CONSTANT VARIABLES ////
const mat3 XYZ_TO_RGB_M = mat3(
    3.2406255, -1.5372080, -0.4986286,
    -0.9689307, 1.8757561, 0.0415175,
    0.0557101, -0.2040211, 1.0569959
    );

const mat3 RGB_TO_XYZ_M = mat3(
    0.4124, 0.3576, 0.1805,
    0.2126, 0.7152, 0.0722,
    0.0193, 0.1192, 0.9505
);

const float PI = 3.14159265359;

///////////////////////////////////////// FUNCS /////////////////////////////////////////

float S(vec3 coeffs, float wl)
{
    float c0 = coeffs.r;
    float c1 = coeffs.g;
    float c2 = coeffs.b;

    float x = fma(fma(c0, wl, c1), wl, c2);
    float y  = inversesqrt(fma(x, x, 1.0));
    return fma(.5 * x, y, .5);
}

float _smoothstep(float x) 
{
    return x * x * (3.0 - 2.0 * x);
}

// stolen from jakob's code (might happen again)
float scale(float k)
{
    return _smoothstep(_smoothstep(float(k) / float(res - 1)));
}

// Stolen too (it happened again)
float rgb2spec_find_interval(float x)
{
    float current = 0.0;
    float old = 0.0;

    // Instead of passing the scale[64] array, we generate its contents
    // and blindly search for the closest (rounded down) one to x.
    // We can do this thanks to smoothstep being monotonically increasing
    // even though it _might_ not be optimal
    for (int k = 0; k < res; ++k)
    {
        current = scale(float(k));
        if(current < x)
        {
            old = k;
        }
        else if(current > x)
        {
            k = res;        // break
        }
        else //current = x
        {
            old = k;
            k = res;        // break
        }
    }
    return old;
}

// A translation attempt from wenzel jakob's rgb2spec code to glsl
//  Probably (surely) has a lot of room for optimization
vec3 fetch_uplifting_lut(vec3 rgb)
{
    // determine biggest component of our rgb color:
    int i = 0;                          // The index of the biggest color, also the lut to be consulted
    for(int j=1; j < 3; j++)
    {
        if(rgb[j] >= rgb[i])
        {
            i = j;
        }
    }

    float z     = rgb[i],
          _scale = float(float(res) - 1) / z,
          x     = rgb[(i + 1) % 3] * _scale,
          y     = rgb[(i + 2) % 3] * _scale;

    /* Trilinearly interpolated lookup */
    uint xi = uint(min(uint(x), uint(res - 2))),
            yi = uint(min(uint(y), uint(res - 2))),
            zi = uint(rgb2spec_find_interval(z));

    float x1 = x - float(xi), x0 = 1.f - x1,
          y1 = y - float(yi), y0 = 1.f - y1,
          z1 = (z - scale(float(zi))) /
               (scale(float(zi + 1)) - scale(float(zi))),
          z0 = 1.f - z1;

    vec3 _offset = vec3(float(xi) / float(res - 1.0), float(yi) / float(res - 1.0), float(zi) / float(res - 1.0));  // res-1 or res-2 ????
    float d_idx = 1.0 / float(res - 1.0);
    vec3 dx = vec3(d_idx, 0, 0);
    vec3 dy = vec3(0, d_idx, 0);
    vec3 dz = vec3(0, 0, d_idx);
    vec3 coeffs = vec3(0.0, 0.0, 0.0);
    if(i == 0)
    {
        // Get coeffs from the first texture
        coeffs=((texture(LUT_1, _offset                 ).rgb * x0 + 
                 texture(LUT_1, _offset + dx            ).rgb * x1) * y0 +
                (texture(LUT_1, _offset + dy            ).rgb * x0 +
                 texture(LUT_1, _offset + dy + dx       ).rgb * x1) * y1) * z0 +
               ((texture(LUT_1, _offset + dy            ).rgb * x0 +
                 texture(LUT_1, _offset + dz + dx       ).rgb * x1) * y0 +
                (texture(LUT_1, _offset + dz + dy       ).rgb * x0 +
                 texture(LUT_1, _offset + dz + dy + dx  ).rgb * x1) * y1) * z1;
    }
    else if (i == 1)
    {
        // Get coeffs from the second texture
        coeffs=((texture(LUT_2, _offset                 ).rgb * x0 + 
                 texture(LUT_2, _offset + dx            ).rgb * x1) * y0 +
                (texture(LUT_2, _offset + dy            ).rgb * x0 +
                 texture(LUT_2, _offset + dy + dx       ).rgb * x1) * y1) * z0 +
               ((texture(LUT_2, _offset + dy            ).rgb * x0 +
                 texture(LUT_2, _offset + dz + dx       ).rgb * x1) * y0 +
                (texture(LUT_2, _offset + dz + dy       ).rgb * x0 +
                 texture(LUT_2, _offset + dz + dy + dx  ).rgb * x1) * y1) * z1;
    }
    else if (i == 2)
    {
        // Get coeffs from the third texture
        coeffs=((texture(LUT_3, _offset                 ).rgb * x0 + 
                 texture(LUT_3, _offset + dx            ).rgb * x1) * y0 +
                (texture(LUT_3, _offset + dy            ).rgb * x0 +
                 texture(LUT_3, _offset + dy + dx       ).rgb * x1) * y1) * z0 +
               ((texture(LUT_3, _offset + dy            ).rgb * x0 +
                 texture(LUT_3, _offset + dz + dx       ).rgb * x1) * y0 +
                (texture(LUT_3, _offset + dz + dy       ).rgb * x0 +
                 texture(LUT_3, _offset + dz + dy + dx  ).rgb * x1) * y1) * z1;
    }
    return coeffs;
}

vec3 jakob_hanika_uplifting(vec3 albedo_tex_rgb, float wavelength)
{
    // Fetch the coefficients from the 3D LUT with the rgb color
    vec3 coeffs = fetch_uplifting_lut(albedo_tex_rgb);
    // Get the spectral response for our chosen wavelength
    return S(coeffs, wavelength);       // Sigmoid
}

vec3 XYZ_to_RGB(vec3 xyz)
{
    return xyz * XYZ_TO_RGB_M;
}

vec3 RGB_to_XYZ(vec3 rgb)
{
    return rgb * RGB_TO_XYZ_M;
}

///////////////////////////////////////// PBR /////////////////////////////////////////

float ggx_dist(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float n_dot_h = max(dot(N, H), 0.0);
    float n_dot_h_2 = n_dot_h*n_dot_h;

    float nom   = a2;
    float denom = (n_dot_h_2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float geom_schlick_ggx_dist(float n_dot_v, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = n_dot_v;
    float denom = n_dot_v * (1.0 - k) + k;

    return nom / denom;
}

float geom_smith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float n_dot_v = max(dot(N, V), 0.0);
    float n_dot_l = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(n_dot_v, roughness);
    float ggx1 = GeometrySchlickGGX(n_dot_l, roughness);

    return ggx1 * ggx2;
}

vec3 fresnel_schlick_approx(float cos_th, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cos_th, 0.0, 1.0), 5.0);
}

// Based on learnopengl tutorials for the cook-torrance shading model
// concretely, the pbr chapter adapted to my spectral pipeline
float pbr_material_shading(vec3 world_pos, float albedo)
{
    // Decode values from the framebuffer textures
    vec4 _metallic_tex_sample = texture(framebuffer_tex2, fTexcoords).rgba;
    float metallic = _metallic_tex_sample.a;
    float roughness = texture(framebuffer_tex3, fTexcoords).a;
    vec3 N = _metallic_tex_sample.rgb;
    vec3 V = normalize(cam_pos - world_pos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    float F0 = 0.04; 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    float Lo = 0.0;

    for(int i = 0; i < num_lights; i++)
    {
        // per-light radiance
        vec3 L = normalize(light_positions[i] - world_pos);
        vec3 H = normalize(V + L);  // halfway vector

        float dist = length(light_positions[i] - world_pos);
        /// TODO: Define light attenuation for lights, also
        //      I should specify how to put lights here !!!
        float att = 1.0;
        
    }
}

///////////////////////////////////////////////////////////////////////////////////////

float material_lighting(int mat_id, vec3 world_pos, float albedo_response)
{
    // I have to put all the material IDs in a single file, 
    //      even id it's just for reference and readability

    if(mat_id == 0) // debgug material
    {
        return vec3(1.0, 0.0, 1.0);
    }
    if(mat_id == 1) // pbr
    {
        return pbr_material_shading(world_pos, albedo_response);
    }
    // else (material id not known)
    return vec3(0.0, 0.0, 0.0);
}

///////////////////////////////////////// MAIN /////////////////////////////////////////

void main()
{
    fb_1_read = texture(framebuffer_tex1, fTexcoords);
    vec3 world_pos = fb_1_read.rgb;
    int mat_id = int(fb_1_read.a);

    if(do_spectral_uplifting)
    {
        // For all the chosen wavelengths
        vec4 final_xyz_color = vec4(0.0, 0.0, 0.0, 0.0);
        vec3 albedo_tex_rgb = texture(framebuffer_tex3, fTexcoords).rgb;   // Fetch original rgb color from tex
        for (int i = 0; i < n_wls; i++)
        {
            // Get the currently sampled wl (from the texture we stored them in)
            float wavelength = texture(tex_wavelengths, (float(i) / float(n_wls))).r;
            // Perform the uplifting step for our rgb color:
            float albedo_spectral_response = jakob_hanika_uplifting(albedo_tex_rgb, wavelength);


            float Lo = material_lighting(mat_id, world_pos, albedo_spectral_response);




            // And the sensor response (tristimulus, can be XYZ or RGB)
            float wl_range = (wavelength - wl_min_resp) / (wl_max_resp - wl_min_resp);
            vec3 response_for_wl = texture(resp_curve, wl_range).rgb;
            if(!convert_xyz_to_rgb)
            {
                response_for_wl = RGB_to_XYZ(response_for_wl.rgb);
            }

            // Cumulative sum for Riemann integration
            color_spectral += vec4(_S * response_for_wl, response_for_wl.g);
        }

        // Riemann sum final step: Divide by number and size of steps
        color_spectral = (( float(wl_max - wl_min) / float(n_wls) ) * color_spectral);

        vec3 aux = XYZ_to_RGB(color_spectral.rgb / color_spectral.a);   // XYZ luminance Y normalization to 100
        out_color = vec4(aux, 1.0);
    }
    else
    {
        // Simply return the rgb texture colors as normal
        out_color = vec4(texture(framebuffer_tex3, fTexcoords).rgb, 1.0);
    }
}
