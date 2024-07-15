#version 450 core

#define MAX_NUM_LIGHTS 16                                   // let's not put too many lights or the shader will cry
#define MAX_FOG_DISTANCE 100.0                              // Should this be an uniform variable? hmm (or maybe the far plane dist)
#define SCENE_BOUNDING_SPHERE_RADIUS 49.8                   // Needed to make dir lights work with fog (needs finetuning for every different scene)

in vec2 fTexcoords;

layout (location = 0) out vec4 out_color;
layout (location = 1) out vec4 fb_out_1;                     // Will be RGB output if we are computing CIE Delta-E 2000
layout (location = 2) out vec4 fb_out_2;                     // Will be Spectral output if we are computing CIE Delta-E 2000
layout (location = 3) out vec4 fb_out_3;                     // free for debugging purposes
layout (location = 4) out vec4 fb_out_4;                     // free for debugging purposes
layout (location = 5) out vec4 fb_out_5;                     // free for debugging purposes
layout (location = 6) out vec4 fb_out_6;                     // free for debugging purposes
layout (location = 7) out vec4 fb_out_7;                     // free for debugging purposes

layout (binding = 0) uniform sampler3D LUT_1;
layout (binding = 1) uniform sampler3D LUT_2;
layout (binding = 2) uniform sampler3D LUT_3;
layout (binding = 3) uniform sampler1D tex_wavelengths;
layout (binding = 4) uniform sampler1D resp_curve;
layout (binding = 5) uniform sampler1DArray l_em_spec;      // array of 1D textures containing the spectral emission of every light in the scene
layout (binding = 6) uniform sampler1D vol_sigma_a_s_spec;  // texture with absorption and scattering spectral coeffs (r and g channels), m^-1
layout (binding = 7) uniform sampler2D depth_buffer_tex;    // TODO: Leave this here?
/////////////////////////////////// FRAMEBUFFER BEGINS HERE ///////////////////////////////////
layout (binding = 8)  uniform sampler2D framebuffer_tex1;   // pos + mat_id
layout (binding = 9)  uniform sampler2D framebuffer_tex2;   // normal + ??? (1 float free for materials to use)
layout (binding = 10) uniform sampler2D framebuffer_tex3;   // albedo + ??? (1 float free for materials to use)
layout (binding = 11) uniform sampler2D framebuffer_tex4;   // ambient occlusion
layout (binding = 12) uniform sampler2D framebuffer_tex5;   // free for materials to use
layout (binding = 13) uniform sampler2D framebuffer_tex6;   // free for materials to use
layout (binding = 14) uniform sampler2D framebuffer_tex7;   // free for materials to use
layout (binding = 15) uniform sampler2D framebuffer_tex8;   // free for materials to use

struct Light                                                // Emission spectrum has to be in a separate texture array :/
{
    vec4 position;                                          // if position.w == 0, directional light, else, point light
    vec3 direction;                                         // only for directional lights
    vec3 attenuation;                                       // constant, linear, quadratic, in that order (unused for now)
    vec3 emission_rgb;                                      // TODO: I still have to do rgb rendering
    float emission_mult;                                    // added by me in case i want to increase the light's radiance artificially
    float wl_min;                                           // The lowest wavelength for this light's radiance emission
    float wl_max;                                           // The highest wavelength for this light's radiance emission
};

uniform int render_mode;                                    // Controls output: RGB (0), Spectral (1) or CIE Delta-E 2000 (2)
uniform bool enable_fog;                                    // if true, render fog on top of the scene
uniform bool shitty_uplifting;                              // Perform the worst spectral uplifting ever seen by man or woman
uniform bool convert_xyz_to_rgb;                            // in case our response curve is in xyz space
uniform int n_wls;                                          // number of wavelengths to sample for rendering
uniform float wl_min;                                       // the min wavelength of our sampleable range
uniform float wl_max;                                       // the max wavelength of our sampleable range
uniform float wl_min_resp;                                  // the smallest wavelength in the response curve
uniform float wl_max_resp;                                  // the biggest wavelength in the repsonse curve
uniform int res = 64;                                       // don't touch, LUT_3D related
uniform vec3 cam_pos;                                       // the position of our current camera
uniform int num_lights;                                     // real number of lights in the scene
uniform Light scene_lights[MAX_NUM_LIGHTS];                 // the lights in our scene
uniform vec3 vol_sigma_s_rgb;                               // scattering coefficient of the scene's global volume (fog), in rgb (m^-1)
uniform vec3 vol_sigma_a_rgb;                               // absorption coefficient of the scene's global volume (fog), in rgb (m^-1)
uniform vec3 vol_KD_rgb;                                    // Jerlov water's KD coefficient, in RGB values
uniform float wl_min_vol;                                   // the smallest wavelength in our volumetric data
uniform float wl_max_vol;                                   // the biggest wavelength in out volumetric data
uniform float sigma_a_mult;                                 // multiplier for our fog's absorption coefficient
uniform float sigma_s_mult;                                 // multiplier for our fog's scattering coefficient

uniform float jerlov_KD_mult = 1.0;                         // Multiplier for Jerlov's KD coefficients
uniform float water_y_camera_offset = 200.0;                // Essentially, how deep's the camera

uniform mat4 inv_proj_mat;                                  // We need it to compute the world position of a fragment (water rendering stuff)
uniform mat4 inv_view_mat;                                  // We need it to compute the world position of a fragment (water rendering stuff)

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

const float PI = 3.14159265359;         // pi

/// TODO: Should this change
const vec3 L_amb_rgb = vec3(1.0,1.0, 1.0);
const float L_amb_spec = 1.0;

////////////////////////////////////// GLOBAL VARS //////////////////////////////////////

int n_lights = 0;

///////////////////////////////////////// FUNCS /////////////////////////////////////////

float ray_sphere_intersect(vec3 r0, vec3 rd, vec3 s0, float sr)
{
    // - r0: ray origin
    // - rd: normalized ray direction
    // - s0: sphere center
    // - sr: sphere radius
    // - Returns distance from r0 to first intersecion with sphere,
    //   or -1.0 if no intersection.
    float t = -1.0;
    vec3 oc = s0 - r0;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - (sr * sr);

    //ray outside, pointing the other direction
    if (c > 0.0f && b <= 0.0f) {
		return -1.0;
	}

    float disc = b * b - c;

    if(disc <= 0.0f) //If negative, no need to keep computing next steps
    {
        return -1.0;
    }

    if(c > 0.0f)
    {
        t = b - sqrt(disc);
    }
    else{
        t = b + sqrt(disc);
    }
	
    return t;
}

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


// Our completely arbitrary wavelength thresholds for
//  the shitty uplifting process
const float wl_threshold_1 = 495.0;
const float wl_threshold_2 = 570.0;
float shitty_uplifting_func(vec3 albedo_tex_rgb, float wavelength)
{
    if(wavelength < wl_threshold_1)
    {
        return albedo_tex_rgb.b;
    }
    else if(wavelength < wl_threshold_2)
    {
        return albedo_tex_rgb.g;
    }
    //else
    return albedo_tex_rgb.r;
}

float jakob_hanika_uplifting(vec3 coeffs, float wavelength)
{
    // Fetch the coefficients from the 3D LUT with the rgb color
    //vec3 coeffs = fetch_uplifting_lut(albedo_tex_rgb);
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

////////////////////////////////// Ocean Scattering //////////////////////////////////

// This is supposed to get the world position from the depth buffer
//   (the depth is not linearized! I think we don't have to)
vec3 world_pos_from_depth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(fTexcoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = inv_proj_mat * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = inv_view_mat * viewSpacePosition;

    return worldSpacePosition.xyz;
}

float E(float E_0, float kD, float depth)
{
    return E_0 * exp(-kD * depth);
}

vec3 E(vec3 E_0, vec3 kD, vec3 depth)
{
    return E_0 * exp(-kD * depth);
}

vec3 ocean_volume_rgb(vec3 albedo, vec3 N, Light l, vec3 world_pos)
{
    vec3 sigma_s_rgb = vol_sigma_s_rgb * vec3(sigma_s_mult);
    vec3 sigma_a_rgb = vol_sigma_a_rgb * vec3(sigma_a_mult);
    vec3 sigma_t_rgb = sigma_s_rgb + sigma_a_rgb;
    vec3 _vol_KD_rgb = vol_KD_rgb * vec3(jerlov_KD_mult);

    vec3 frag_to_cam = cam_pos - world_pos;
    //float y_S = frag_to_cam.y + water_y_camera_offset;                      // Vertical dist from water to frag (offset relative to cam pos)
    float y_S = (water_y_camera_offset - world_pos.y); //abs?

    float y_W = -(frag_to_cam.y / length(frag_to_cam));                                             // Vertical distance from frag to cam (invert sign??)

    vec3 E_0 = l.emission_rgb * vec3(l.emission_mult);

    vec3 radiance_to_frag = exp(-sigma_t_rgb * vec3(y_S)) * (E(E_0, _vol_KD_rgb, vec3(y_S)) * (albedo / vec3(PI)) * vec3(max(0.7, normalize(N).y)));
    vec3 scattering = ( ( (sigma_s_rgb * E(E_0,  _vol_KD_rgb, vec3((water_y_camera_offset - cam_pos.y)))) ) / ( vec3(4.0 * PI) * (_vol_KD_rgb * vec3(y_W) - sigma_t_rgb ) ) ) 
                        * ( exp((_vol_KD_rgb * vec3(y_W) - sigma_t_rgb) * vec3(abs(length(frag_to_cam))) ) - vec3(1.0));

    vec3 Lo = scattering + radiance_to_frag;

    return Lo;
}

float ocean_volume_spectral(float wavelength, float albedo, vec3 N, Light l, vec3 world_pos)
{
    float wl_sample_uv = (wavelength - wl_min_vol) / (wl_max_vol - wl_min_vol);
    vec3 vol_sample_spec = texture(vol_sigma_a_s_spec, wl_sample_uv).rgb;   // Kd (unused) is .b
    
    float vol_sigma_a_spec = vol_sample_spec.r * sigma_a_mult;              // sigma_a is .r
    float vol_sigma_s_spec = vol_sample_spec.g * sigma_s_mult;              // sigma_s is .g
    float vol_KD_spec = vol_sample_spec.b * jerlov_KD_mult;                 // jerlov KD
    float vol_sigma_t_spec = vol_sigma_a_spec + vol_sigma_s_spec;                

    vec3 frag_to_cam = cam_pos - world_pos;
    //float y_S = frag_to_cam.y + water_y_camera_offset;                    // Vertical dist from water to frag (offset relative to cam pos)
    float y_S = (water_y_camera_offset - world_pos.y); //abs?
    float y_W = -(frag_to_cam.y / length(frag_to_cam));                   // Vertical distance from frag to cam (invert sign??)

    float _l_emission_coord = (wavelength - l.wl_min) / (l.wl_max - l.wl_min);
    float E_0 = texture(l_em_spec, vec2(_l_emission_coord, float(0))).r * l.emission_mult;


    float radiance_to_frag = exp(-vol_sigma_t_spec * y_S) * (E(E_0, vol_KD_spec, y_S) * (albedo / PI) * max(0.7, normalize(N).y));
    float scattering = ( ( (vol_sigma_s_spec * E(E_0,  vol_KD_spec, (water_y_camera_offset - cam_pos.y))) ) / ( (4.0 * PI) * (vol_KD_spec * y_W - vol_sigma_t_spec ) ) ) 
                        * ( exp((vol_KD_spec * y_W - vol_sigma_t_spec) * abs(length(frag_to_cam)) ) - 1.0);

    float Lo = scattering + radiance_to_frag;
    return Lo;
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
    float ggx2 = geom_schlick_ggx_dist(n_dot_v, roughness);
    float ggx1 = geom_schlick_ggx_dist(n_dot_l, roughness);

    return ggx1 * ggx2;
}

vec3 fresnel_schlick_approx(float cos_th, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cos_th, 0.0, 1.0), 5.0);
}

float fresnel_schlick_approx(float cos_th, float F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cos_th, 0.0, 1.0), 5.0);
}

// Based on learnopengl tutorials for the cook-torrance shading model
// concretely, the pbr chapter adapted to my spectral pipeline
/// INFO: Spectral version
float pbr_material_shading(vec3 world_pos, float wavelength, float albedo)
{
    // Decode values from the framebuffer textures
    vec4 _metallic_tex_sample = texture(framebuffer_tex2, fTexcoords).rgba;
    float metallic =    texture(framebuffer_tex2, fTexcoords).a;//_metallic_tex_sample.a;
    float roughness =           texture(framebuffer_tex3, fTexcoords).a;
    float ao =                  texture(framebuffer_tex4, fTexcoords).r;

    vec3 N =                    normalize(_metallic_tex_sample.xyz);
    vec3 V =                    normalize(cam_pos - world_pos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    float F0 = 0.04; 
    //float F0 = albedo; 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    float Lo = 0.0;

    // Get light radiance (depends on type, either point or dir light)
    for(int i = 0; i < n_lights; i++)
    {
        vec3 L = vec3(0,0,0);           // assign values later
        vec3 H = vec3(0,0,0);           // assign values later
        float att = 1.0;

        Light l = scene_lights[i];
        if(l.position.w == 0)   // dir light
        {
            L = normalize(-l.direction);
            H = normalize(V + L);
            att = 1.0;
            if(enable_fog)
            {
                float dist_dir_light = ray_sphere_intersect(world_pos, normalize(-l.direction), vec3(0.0), SCENE_BOUNDING_SPHERE_RADIUS);
                float wl_sample_uv = (wavelength - wl_min_vol) / (wl_max_vol - wl_min_vol);
                vec3 vol_sample_spec = texture(vol_sigma_a_s_spec, wl_sample_uv).rgb;   // Kd (unused) is .b
                
                float vol_sigma_a_spec = vol_sample_spec.r * sigma_a_mult;              // sigma_a is .r
                float vol_sigma_s_spec = vol_sample_spec.g * sigma_s_mult;              // sigma_s is .g
                float vol_sigma_t_spec = vol_sigma_a_spec + vol_sigma_s_spec;
                if (dist_dir_light == -1.0)
                {
                    dist_dir_light = 1.0;
                }
                att *= exp(-vol_sigma_t_spec * dist_dir_light);
            }
        }
        else    // point light
        {
            L = normalize(l.position.xyz - world_pos);
            H = normalize(V + L);  // halfway vector
            float distance = length(l.position.xyz - world_pos);
            att = 1.0 / dot(vec3(1.0, distance, distance * distance), l.attenuation);
            if(enable_fog)
            {
                /// TODO: Needs testing!
                float wl_sample_uv = (wavelength - wl_min_vol) / (wl_max_vol - wl_min_vol);
                vec3 vol_sample_spec = texture(vol_sigma_a_s_spec, wl_sample_uv).rgb;   // Kd (unused) is .b
                
                float vol_sigma_a_spec = vol_sample_spec.r * sigma_a_mult;              // sigma_a is .r
                float vol_sigma_s_spec = vol_sample_spec.g * sigma_s_mult;              // sigma_s is .g
                float vol_sigma_t_spec = vol_sigma_a_spec + vol_sigma_s_spec;
                att *= exp(-vol_sigma_t_spec * distance);
            }
        }
        // per-light radiance (we assume wavelength is in range [l.wl_min, l-wl_max] )
        float _l_emission_coord = (wavelength - l.wl_min) / (l.wl_max - l.wl_min);
        float radiance = texture(l_em_spec, vec2(_l_emission_coord, float(i))).r * l.emission_mult * att;

        // Cook-Torrance BRDF
        float NDF = ggx_dist(N, H, roughness);
        float G = geom_smith(N, V, L, roughness);
        float F = fresnel_schlick_approx(max(dot(H, V), 0.0), F0);
        
        float num = NDF * G * F;
        float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // prevent a division by 0
        
        float specular = num / denom;
        
        float kS = F;
        // energy conservation, diff and spec can't be above 1.0
        float kD = 1.0 - kS;
        kD *= (1.0 - metallic);
        float n_dot_l = max(dot(N, L), 0.0);
        // Add to outgoing radiance Lo (we already multiplied by fresnel)
        Lo += ((kD * albedo / PI) + specular) * radiance * n_dot_l;
    }
    // TODO: IBL or Ambient Occlusion
    //float ambient = 0.03 * albedo * ao;
    float ambient = 0.0;
    float color = ambient + Lo;
    return color;
}

/// INFO: Spectral Version
float diffuse_material_shading(vec3 world_pos, float wavelength, float albedo)
{
    vec3 N = normalize(texture(framebuffer_tex2, fTexcoords).xyz);
    float Lo = 0.0;

    for(int i = 0; i < n_lights; i++)
    {
        vec3 L = vec3(0,0,0);           // assign values later
        float att = 1.0;

        Light l = scene_lights[i];
        if(l.position.w == 0)   // dir light
        {
            L = normalize(-l.direction);
            att = 1.0;
            if(enable_fog)
            {
                float my_depth = texture(depth_buffer_tex, fTexcoords).r;
                vec3 my_world_pos = world_pos_from_depth(my_depth);

                Lo = ocean_volume_spectral(wavelength, albedo, N, l, my_world_pos);
            }
            else
            {
                float _l_emission_coord = (wavelength - l.wl_min) / (l.wl_max - l.wl_min);
                float radiance = texture(l_em_spec, vec2(_l_emission_coord, float(i))).r * l.emission_mult * att;

                float n_dot_l = max(dot(N, L), 0.0);
                // Add to outgoing radiance Lo
                Lo += (albedo / PI) * radiance * n_dot_l * att;
            }
        }
        else    // point light
        {
            L = normalize(l.position.xyz - world_pos);
            float distance = abs(length(l.position.xyz - world_pos));
            att = 1.0 / dot(vec3(1.0, distance, distance * distance), l.attenuation);

            /// TODO: Now we should do Néstor's Eurographics approximation
            if(!enable_fog)
            {
                float _l_emission_coord = (wavelength - l.wl_min) / (l.wl_max - l.wl_min);
                float radiance = texture(l_em_spec, vec2(_l_emission_coord, float(i))).r * l.emission_mult * att;

                float n_dot_l = max(dot(N, L), 0.0);
                // Add to outgoing radiance Lo
                Lo += (albedo / PI) * radiance * n_dot_l * att;
            }
        }
        // per-light radiance (we assume wavelength is in range [l.wl_min, l-wl_max] ) 
    }
    float ambient = 0.0;    // In case I need to tune something
    return Lo + ambient;
}

/// INFO: RGB Version
vec3 pbr_material_shading(vec3 world_pos)
{
    // Decode values from the framebuffer textures
    vec4 _metallic_tex_sample = texture(framebuffer_tex2, fTexcoords).rgba;
    vec4 _albedo_tex_sample = texture(framebuffer_tex3, fTexcoords).rgba;

    vec3 albedo =       pow(_albedo_tex_sample.rgb, vec3(2.2));
    float roughness =   _albedo_tex_sample.a;
    float metallic =    texture(framebuffer_tex2, fTexcoords).a;//_metallic_tex_sample.a;
    vec3 N =            normalize(_metallic_tex_sample.xyz);
    vec3 ao =           texture(framebuffer_tex4, fTexcoords).rgb;

    vec3 V = normalize(cam_pos - world_pos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    //vec3 F0 = albedo; 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);

    // Get light radiance (depends on type, either point or dir light)
    for(int i = 0; i < n_lights; i++)
    {
        vec3 L = vec3(0,0,0);           // assign values later
        vec3 H = vec3(0,0,0);           // assign values later
        vec3 att = vec3(1.0);

        Light l = scene_lights[i];
        if(l.position.w == 0)   // dir light
        {
            L = normalize(-l.direction);
            H = normalize(V + L);
            vec3 att = vec3(1.0);
            if(enable_fog)
            {
                float dist_dir_light = ray_sphere_intersect(world_pos, normalize(-l.direction), vec3(0.0), SCENE_BOUNDING_SPHERE_RADIUS);
                vec3 sigma_s_rgb = vol_sigma_s_rgb * vec3(sigma_s_mult);
                vec3 sigma_a_rgb = vol_sigma_a_rgb * vec3(sigma_a_mult);
                vec3 sigma_t_rgb = sigma_s_rgb + sigma_a_rgb;
                if (dist_dir_light == -1.0)
                {
                    dist_dir_light = 1.0;
                }
                att *= exp(-sigma_t_rgb * vec3(dist_dir_light, dist_dir_light, dist_dir_light));
            }
        }
        else    // point light
        {
            L = normalize(l.position.xyz - world_pos);
            H = normalize(V + L);  // halfway vector
            float distance = length(l.position.xyz - world_pos);
            att = vec3(1.0 / dot(vec3(1.0, distance, distance * distance), l.attenuation));
            if(enable_fog)
            {
                /// TODO: Work in progress!
                vec3 sigma_s_rgb = vol_sigma_s_rgb * vec3(sigma_s_mult);
                vec3 sigma_a_rgb = vol_sigma_a_rgb * vec3(sigma_a_mult);
                vec3 sigma_t_rgb = sigma_s_rgb + sigma_a_rgb;
                att *= exp(-sigma_t_rgb * vec3(distance));
            }
        }
        // per-light radiance
        vec3 radiance = l.emission_rgb * l.emission_mult * att;

        // Cook-Torrance BRDF
        float NDF = ggx_dist(N, H, roughness);
        float G = geom_smith(N, V, L, roughness);
        vec3 F = fresnel_schlick_approx(max(dot(H, V), 0.0), F0);
        
        vec3 num = NDF * G * F;
        float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular = num / max(denom, 0.0001);  // prevent a division by 0
        
        vec3 kS = F;
        // energy conservation, diff and spec can't be above 1.0
        vec3 kD = vec3(1.0) - kS;
        kD *= (1.0 - metallic);
        float n_dot_l = max(dot(N, L), 0.0);

        // Add to outgoing radiance Lo (we already multiplied by fresnel)
        Lo += ((kD * albedo / PI) + specular) * radiance * n_dot_l;
    }

    // TODO: IBL or Ambient Occlusion
    // vec3 ambient = vec3(0.03) * albedo * ao.r;
    vec3 ambient = vec3(0.0, 0.0, 0.0);
    vec3 color = ambient + Lo;
    return color;
}

/// INFO: RGB Version
vec3 diffuse_material_shading(vec3 world_pos)
{
    vec3 albedo = pow(texture(framebuffer_tex3, fTexcoords).rgb, vec3(2.2));
    vec3 N = normalize(texture(framebuffer_tex2, fTexcoords).xyz);
    vec3 Lo = vec3(0.0);

    for(int i = 0; i < n_lights; i++)
    {
        vec3 L = vec3(0,0,0);           // assign values later
        vec3 att = vec3(1.0);

        Light l = scene_lights[i];
        if(l.position.w == 0)   // dir light
        {
            L = normalize(-l.direction);
            att = vec3(1.0);
            if(enable_fog)
            {
                float my_depth = texture(depth_buffer_tex, fTexcoords).r;
                vec3 my_world_pos = world_pos_from_depth(my_depth);

                Lo = ocean_volume_rgb(albedo, N, l, my_world_pos);
            }
            else
            {
                vec3 radiance = l.emission_rgb.rgb * l.emission_mult;
                float n_dot_l = max(dot(N, L), 0.0);
                // Add to outgoing radiance Lo
                Lo += (albedo / PI) * radiance * n_dot_l * att;
            }
        }
        else    // point light
        {
            L = normalize(l.position.xyz - world_pos);
            float distance = abs(length(l.position.xyz - world_pos));
            att = vec3(1.0 / dot(vec3(1.0, distance, distance * distance), l.attenuation));
            if(enable_fog)
            {
                /// TODO: Work in progress!
                vec3 sigma_s_rgb = vol_sigma_s_rgb * vec3(sigma_s_mult);
                vec3 sigma_a_rgb = vol_sigma_a_rgb * vec3(sigma_a_mult);
                vec3 sigma_t_rgb = sigma_s_rgb + sigma_a_rgb;
                att *= exp(-sigma_t_rgb * vec3(distance));
            }
            else
            {
                vec3 radiance = l.emission_rgb.rgb * l.emission_mult;
                float n_dot_l = max(dot(N, L), 0.0);
                // Add to outgoing radiance Lo
                Lo += (albedo / PI) * radiance * n_dot_l * att;
            }
        }
    }
    vec3 ambient = vec3(0.0, 0.0, 0.0);    // In case I need to tune something
    return Lo + ambient;
}

///////////////////////////////////////////////////////////////////////////////////////


// Spectral version (PBR MATERIAL LEAKS NANs SOMEWHERE!)
float material_lighting(int mat_id, vec3 world_pos, float wavelength, float albedo_response)
{
    // I have to put all the material IDs in a single file, 
    //      even id it's just for reference and readability

    if(mat_id == 1) // V O I D
    {
        if(enable_fog)
        {
            
            float my_depth = texture(depth_buffer_tex, fTexcoords).r;
            vec3 my_world_pos = world_pos_from_depth(my_depth);
            float albedo = 0.0;
            Light l = scene_lights[0];
            vec3 N = normalize(cam_pos - my_world_pos);

            return ocean_volume_spectral(wavelength, albedo, N, l, my_world_pos);
        }
        else
        {
            return 0.0;
        }
    }
    if(mat_id == 2) // pbr
    {
        return pbr_material_shading(world_pos, wavelength, albedo_response);
    }
    if(mat_id == 3) // diffuse
    {
        return diffuse_material_shading(world_pos, wavelength, albedo_response);
    }
    // else (material id not known, return black)
    return 0.0f;
}

// RGB version
vec3 material_lighting(int mat_id, vec3 world_pos)
{
    // I have to put all the material IDs in a single file, 
    //      even id it's just for reference and readability

    if(mat_id == 1) // V O I D
    {
        if(enable_fog)
        {
            
            float my_depth = texture(depth_buffer_tex, fTexcoords).r;
            vec3 my_world_pos = world_pos_from_depth(my_depth);
            vec3 albedo = vec3(0.0);
            Light l = scene_lights[0];
            vec3 N = normalize(cam_pos - my_world_pos);

            return ocean_volume_rgb(albedo, N, l, my_world_pos);
        }
        else
        {
            return vec3(0.0);
        }
    }
    if(mat_id == 2) // pbr
    {
        return pbr_material_shading(world_pos);
    }
    if(mat_id == 3) // diffuse
    {
        return diffuse_material_shading(world_pos);
    }
    // else (material id not known, return black)
    return vec3(0.0, 0.0, 0.0);
}

///////////////////////////////////////// MAIN /////////////////////////////////////////
void main()
{
    n_lights = num_lights;
    if(num_lights > MAX_NUM_LIGHTS)
    {
        n_lights = MAX_NUM_LIGHTS;
    }

    vec4 fb_1_read = texture(framebuffer_tex1, fTexcoords).rgba;
    vec3 world_pos = fb_1_read.rgb;
    int mat_id = int(fb_1_read.a);

    if(render_mode == 1 || render_mode == 2)
    {
        // For all the chosen wavelengths
        vec4 final_xyz_color = vec4(0.0, 0.0, 0.0, 0.0);
        // If we fetch the albedo tex here we save #wls-1 texture reads,
        //      at the cost of 1 extra param for every function that follows
        vec3 albedo_tex_rgb = pow(texture(framebuffer_tex3, fTexcoords).rgb, vec3(2.2));   // Fetch original rgb color from tex
        
        // Fetch the LUTs out of the loop FFS
        if(!shitty_uplifting)
        {
            albedo_tex_rgb = fetch_uplifting_lut(albedo_tex_rgb);
        }

        
        for (int i = 0; i < n_wls; i++)
        {
            // Get the currently sampled wl (from the texture we stored them in)
            float wavelength = texture(tex_wavelengths, (float(i) / float(n_wls))).r;
            // Perform the uplifting step for our rgb color:
            float albedo_spectral_response = 0.0;
            if(shitty_uplifting)
            {
                albedo_spectral_response = shitty_uplifting_func(albedo_tex_rgb, wavelength);
            }
            else
            {
                albedo_spectral_response = jakob_hanika_uplifting(albedo_tex_rgb, wavelength);
            }

            // Actual material calculations for lighting
            float Lo = material_lighting(mat_id, world_pos, wavelength, albedo_spectral_response);

            // Sensor response (tristimulus, can be XYZ or RGB)
            float wl_range = (wavelength - wl_min_resp) / (wl_max_resp - wl_min_resp);
            vec3 response_for_wl = texture(resp_curve, wl_range).rgb;
            if(!convert_xyz_to_rgb)
            {
                response_for_wl = RGB_to_XYZ(response_for_wl.rgb);
            }
            // Cumulative sum of responses for Riemann integration
            final_xyz_color += vec4(vec3(Lo * response_for_wl.rgb), response_for_wl.g);
        }
        // Riemann sum final step: Divide by number and size of steps
        final_xyz_color = (( float(wl_max - wl_min) / float(n_wls) ) * final_xyz_color);

        // Final color space conversion (gamma and tonemapping should be done in the postprocess step)
        vec3 out_rgb = XYZ_to_RGB(final_xyz_color.rgb / final_xyz_color.a);   // XYZ luminance Y normalization to 100 (or 1)
        
        if(render_mode == 1)
        {
            // If spectral render, output only this
            out_color = vec4(out_rgb, 1.0);
        }
        else // render_mode == 2
        {
            // If rendering CIE Delta-E 2000, put RGB in FB0 and spectral in FB1
            fb_out_1 = vec4(out_rgb, 1.0);
        }
        // float wavelength = mix(wl_min, wl_max, fTexcoords.x);
        // Light l = scene_lights[0];
        // float _l_emission_coord = (wavelength - l.wl_min) / (l.wl_max - l.wl_min);
        // float xyz_coord = (wavelength - wl_min_resp) / (wl_max_resp - wl_min_resp);
        // out_color = vec4(XYZ_to_RGB(texture(l_em_spec, vec2(_l_emission_coord, 0.0)).rrr * texture(resp_curve, xyz_coord).rgb), 1.0);
    }
    if(render_mode == 0 || render_mode == 2)
    {
        /// Render in RGB instead of spectrally
        // world_pos, mat_id are known 
        vec3 Lo = material_lighting(mat_id, world_pos); 

        // In this case we don't have to output to a different texture, this one's fine
        out_color = vec4(Lo, 1.0);
    }
}
