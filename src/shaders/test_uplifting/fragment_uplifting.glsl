#version 450 core

in vec2 fTexcoords;

layout (location = 0) out vec4 out_color;
layout (location = 1) out vec4 debug_1;
layout (location = 2) out vec4 debug_2;
layout (location = 3) out vec4 debug_3;
layout (location = 4) out vec4 debug_4;
layout (location = 5) out vec4 debug_5;
layout (location = 6) out vec4 debug_6;
layout (location = 7) out vec4 debug_7;

layout (binding = 0) uniform sampler2D tex_to_uplift;
layout (binding = 1) uniform sampler3D LUT_1;
layout (binding = 2) uniform sampler3D LUT_2;
layout (binding = 3) uniform sampler3D LUT_3;
layout (binding = 4) uniform sampler1D resp_curve;
layout (binding = 5) uniform sampler1D tex_wavelengths;

uniform bool do_spectral_uplifting;
uniform bool convert_xyz_to_rgb;
uniform int n_wls;
uniform float wl_min;
uniform float wl_max;
uniform float wl_min_resp;
uniform float wl_max_resp;
uniform int res = 64;

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

// const mat3 XYZ_TO_RGB_M = mat3(
//     3.2404542, -1.5371385, -0.4985314,
//     -0.9692660, 1.8760108, 0.0415560,
//     0.0556434, -0.2040259, 1.0572252
//     );

// const mat3 RGB_TO_XYZ_M = mat3(
//     0.4124564, 0.3575761, 0.1804375,
//     0.2126729, 0.7151522, 0.0721750,
//     0.0193339, 0.1191920, 0.9503041
// );

/////////////////////////////////

vec3 encode_sRGB(vec3 linear_RGB)
{
    vec3 a = 12.92 * linear_RGB;
    vec3 b = 1.055 * pow(linear_RGB, vec3(1.0 / 2.4)) - 0.055;
    vec3 c = step(vec3(0.0031308), linear_RGB);

    return mix(a, b, c);
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

// Stolen
float _smoothstep(float x) 
{
    return x * x * (3.0 - 2.0 * x);
}

float scale(float k)
{
    return _smoothstep(_smoothstep(float(k) / float(res - 1)));
}

// Stolen too 
float rgb2spec_find_interval(float x)
{
    float current = 0.0;
    float old = 0.0;

    // Instead of passing the scale[64] array, we generate its contents
    // and blindly search for the closest (rounded down) one to x.
    // We can do this thanks to smoothstep being monotonically increasing
    // (Anyway, it's still sketchy as hell but I want to make this work)
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
vec3 fetch_coeffs_from_lut_trilinear_manual(vec3 rgb)
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

// Another translation attempt from wenzel jakob's rgb2spec code to glsl 
//  (doesn't work properly since one dimension in the 3D LUT is non linear 
//  and returns incorrect values for the spectral coefficients)
vec3 fetch_coeffs_from_lut_opengl_interp(vec3 rgb)
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
          _scale = 1.0 / z,
          x     = (rgb[(i + 1) % 3] * _scale),
          y     = (rgb[(i + 2) % 3] * _scale);

    z = scale(z * (float(res)-1.0));

    vec3 _uv_3d = vec3(x, y, z);
    vec3 coeffs = vec3(0,0,0);
    if(i == 0)
    {
        coeffs = texture(LUT_1, _uv_3d).rgb;
    }
    else if(i == 1)
    {
        coeffs = texture(LUT_2, _uv_3d).rgb ;
    }
    else if(i == 2)
    {
        coeffs = texture(LUT_3, _uv_3d).rgb;
    }
    return coeffs;
}

vec3 XYZ_to_RGB(vec3 xyz)
{
    return xyz * XYZ_TO_RGB_M;
}

vec3 RGB_to_XYZ(vec3 rgb)
{
    return rgb * RGB_TO_XYZ_M;
}

void main()
{
    vec3 _rgb_test = vec3(0.098, 0.556, 0.278);
    vec3 _xyz_target = vec3(0.1174, 0.1999, 0.0714);
    vec3 _xyz_converted = RGB_to_XYZ(_rgb_test);
    vec3 _rgb_from_xyz_from_rgb = XYZ_to_RGB(_xyz_converted);
    debug_1 = vec4(_rgb_test, 1.0);
    debug_2 = vec4(_xyz_target, 1.0);
    debug_3 = vec4(_xyz_converted, 1.0);
    debug_4 = vec4(_rgb_from_xyz_from_rgb, 1.0);

    if(do_spectral_uplifting)
    {
        vec4 color_spectral = vec4(0.0, 0.0, 0.0, 0.0);
        //float _S;
        //vec3 response_for_wl;
        // Perform the uplifting step. First sample the texture normally
        vec3 color_rgb = texture(tex_to_uplift, fTexcoords).rgb;
        for (int i = 0; i < n_wls; i++)
        {
            // Fetch the original rgb color from the texture
            float wavelength = texture(tex_wavelengths, (float(i) / float(n_wls))).r;
            
            // Then fetch the coefficients from the 3D LUT with the rgb color
            vec3 coeffs = fetch_coeffs_from_lut_trilinear_manual(color_rgb);

            // Get the spectral response for our chosen wavelength
            float _S = S(coeffs, wavelength);

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
        out_color = vec4(texture(tex_to_uplift, fTexcoords).rgb, 1.0);
    }
}
