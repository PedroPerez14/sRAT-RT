#version 450 core

in vec2 fTexcoords;

layout (location = 0) out vec4 out_color;

layout (location = 1) out vec4 uplift_wl_0_3;
layout (location = 2) out vec4 uplift_wl_4_7;
layout (location = 3) out vec4 uplift_wl_8_11;

layout (binding = 0) uniform sampler2D tex_to_uplift;
layout (binding = 1) uniform sampler3D LUT_1;
layout (binding = 2) uniform sampler3D LUT_2;
layout (binding = 3) uniform sampler3D LUT_3;

uniform int n_wls;
uniform int res = 64;
// uniform float wls[40];

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
    // (Anyway, it's still sketchy as fuck but I want to make this shit work)
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

// a translation attempt from wenzel jakob's rgb2spec code to glsl
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

// a translation attempt from wenzel jakob's rgb2spec code to glsl
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

void main()
{
    float _WAVELENGTH = 532.0;

    // Perform the uplifting here    
    vec3 color_rgb = texture(tex_to_uplift, fTexcoords).rgb;

    vec3 coeffs = fetch_coeffs_from_lut_trilinear_manual(color_rgb);
    //vec3 coeffs = fetch_coeffs_from_lut_opengl_interp(color_hardcoded);
    
    for(int i = 0; i < min(n_wls, 12); i++)
    {
        if(i <4)
        {
            uplift_wl_0_3[i % 4] = S(coeffs, _WAVELENGTH);
        }
        else if(i <8)
        {
            int _i = 0;                          // The index of the biggest color, also the lut to be consulted
            for(int j=1; j < 3; j++)
            {
                if(color_rgb[j] >= color_rgb[_i])
                {
                    _i = j;
                }
            }

            float z = color_rgb[_i];
            float _z = scale(z * float(float(res) - 1.0));
            uplift_wl_4_7.rgba = vec4(z, z, _z, _z);
            //uplift_wl_4_7[i % 4] = S(coeffs, _WAVELENGTH);
        }
        else if(i <12)
        {
            // uplift_wl_8_11[i% 4] = S(coeffs, _WAVELENGTH);
            uplift_wl_8_11.rgba = vec4(coeffs, 1.0);
        }
    }
    out_color = vec4(color_rgb, 1.0);
}
