#version 450 core

in vec2 fTexcoords;

out vec4 out_color;

layout (binding = 0) uniform sampler2D tex_render;
layout (binding = 1) uniform sampler2D tex_aux;
uniform int render_mode;

const mat3 RGB_TO_XYZ_M = mat3(
    0.4124, 0.3576, 0.1805,
    0.2126, 0.7152, 0.0722,
    0.0193, 0.1192, 0.9505
);

const vec3 D65_WHITE = vec3(0.95045592705, 1.0, 1.08905775076);
const vec3 WHITE = D65_WHITE;
const float gamma = 2.4;
const float exposure = 0.5;
const float PI = 3.14159265359;
const float TWO_PI = 6.28318530718;

vec3 RGB_to_XYZ(vec3 rgb)
{
    return rgb * RGB_TO_XYZ_M;
}

float XYZ_TO_LAB_F(float x)
{
    //          (24/116)^3                         1/(3*(6/29)^2)     4/29
    return x > 0.00885645167 ? pow(x, 0.333333333) : 7.78703703704 * x + 0.13793103448;
}

vec3 XYZ_TO_LAB(vec3 xyz)
{
    vec3 xyz_scaled = xyz / WHITE;
    xyz_scaled = vec3(
        XYZ_TO_LAB_F(xyz_scaled.x),
        XYZ_TO_LAB_F(xyz_scaled.y),
        XYZ_TO_LAB_F(xyz_scaled.z)
    );
    return vec3(
        (116.0 * xyz_scaled.y) - 16.0,
        500.0 * (xyz_scaled.x - xyz_scaled.y),
        200.0 * (xyz_scaled.y - xyz_scaled.z)
    );
}

vec3 encode_sRGB(vec3 linear_RGB)
{
    vec3 a = 12.92 * linear_RGB;
    vec3 b = 1.055 * pow(linear_RGB, vec3(1.0 / 2.2)) - 0.055;
    vec3 c = step(vec3(0.0031308), linear_RGB);

    return mix(a, b, c);
}

vec3 reinhard(vec3 v)
{
    v = pow(v, vec3(1. / gamma));
    return vec3(v / (vec3(1.0f) + v));
}

vec3 linearToneMapping(vec3 color)
{
	color = clamp(exposure * color, 0., 1.);
	color = pow(color, vec3(1. / gamma));
	return color;
}

vec3 exposureToneMapping(vec3 color)
{
    vec3 mapped = vec3(1.0) - exp(-color * exposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
    return mapped;
}

// This function gives you the *perceptual* difference between two colors in L*a*b* space.
// Most implementations of it online are are actually wrong!!!
//
// Additionally, although it is often hailed as the current "most accurate" color difference
// formula, it actually contains a pretty decent-sized discontinuity for colors with opposite hues.
//     See "The CIEDE2000 Color-DifferenceFormula: Implementation Notes,
//     Supplementary Test Data, and Mathematical Observations"
//     by G. Sharma et al. for more information. Link:
//     http://www2.ece.rochester.edu/~gsharma/ciede2000/ciede2000noteCRNA.pdf
//
float LAB_DELTA_E_CIE2000(vec3 lab1, vec3 lab2) {
    // b = bar
    // p = prime
    float Cb7 = pow((sqrt(lab1.y*lab1.y + lab1.z*lab1.z) + sqrt(lab1.y*lab1.y + lab1.z*lab1.z))*0.5, 7.0);
    //                                 25^7
    float G = 0.5*(1.0-sqrt(Cb7/(Cb7 + 6103515625.0)));

    float ap1 = lab1.y*(1.0 + G);
    float ap2 = lab2.y*(1.0 + G);

    float Cp1 = sqrt(ap1*ap1 + lab1.z*lab1.z);
    float Cp2 = sqrt(ap2*ap2 + lab2.z*lab2.z);
    
    float hp1 = atan(lab1.z, ap1);
    float hp2 = atan(lab2.z, ap2);
    if(hp1 < 0.0) hp1 = TWO_PI + hp1;
    if(hp2 < 0.0) hp2 = TWO_PI + hp2;
    
    float dLp = lab2.x - lab1.x;
    float dCp = Cp2 - Cp1;
    float dhp = hp2 - hp1;
    dhp += (dhp>PI) ? -TWO_PI: (dhp<-PI) ? TWO_PI : 0.0;
    // don't need to handle Cp1*Cp2==0 case because it's implicitly handled by the next line
    float dHp = 2.0*sqrt(Cp1*Cp2)*sin(dhp/2.0);
    
    float Lbp = (lab1.x + lab2.x)*0.5;
    float Cbp = sqrt(Cp1 + Cp2)/2.0;
    float Cbp7 = pow(Cbp, 7.0);
    
    // CIEDE 2000 Color-Difference \Delta E_{00}
    // This where everyone messes up (because it's a pain)
    // it's also the source of the discontinuity...
    
    // We need to average the angles h'_1 and h'_2 (hp1 and hp2) here.
    // This is a surprisingly nontrivial task.
    // Credit to https://stackoverflow.com/a/1159336 for the succinct formula.
    float hbp = mod( ( hp1 - hp2 + PI), TWO_PI ) - PI;
    hbp = mod((hp2 + ( hbp / 2.0 ) ), TWO_PI);
    if(Cp1*Cp2 == 0.0) hbp = hp1 + hp2;
    
    //                             30 deg                                                  6 deg                            63 deg
    float T = 1.0 - 0.17*cos(hbp - 0.52359877559) + 0.24*cos(2.0*hbp) + 0.32*cos(3.0*hbp + 0.10471975512) - 0.2*cos(4.0*hbp - 1.09955742876);
    
    float dtheta = 30.0*exp(-(hbp - 4.79965544298)*(hbp - 4.79965544298)/25.0);
    float RC = 2.0*sqrt(Cbp7/(Cbp7 + 6103515625.0));
    
    float Lbp2 = (Lbp-50.0)*(Lbp-50.0);
    float SL = 1.0 + 0.015*Lbp2/sqrt(20.0 + Lbp2);
    float SC = 1.0 + 0.045*Cbp;
    float SH = 1.0 + 0.015*Cbp*T;
    
    float RT = -RC*sin(2.0*dtheta)/TWO_PI;
    
    return sqrt(dLp*dLp/(SL*SL) + dCp*dCp/(SC*SC) + dHp*dHp/(SH*SH) + RT*dCp*dHp/(SC*SH));
}

void main()
{
    // if we are rendering in rgb or spectral
    if(render_mode == 0 || render_mode == 1)
    {
        // L=0.2125R+0.7154G+0.0721B
        vec3 col = texture(tex_render, fTexcoords).rgb;
        col = encode_sRGB(col);
        out_color = vec4(col.rgb, 1.0);
    }
    else
    {
        vec3 render_rgb = texture(tex_render, fTexcoords).rgb;
        vec3 render_spec = texture(tex_aux, fTexcoords).rgb;

        float CIE_dE_2000 = LAB_DELTA_E_CIE2000(XYZ_TO_LAB(RGB_to_XYZ(render_rgb)), XYZ_TO_LAB(RGB_to_XYZ(render_spec)));

        
        vec3 _color = vec3(0.0, 0.0, 0.0);

        if(CIE_dE_2000 < 1.0)
        {
            _color = vec3(0.0, 0.0, 1.0);
        }
        else if(CIE_dE_2000 < 2.0)
        {
            _color = vec3(0.0, 1.0, 0.0);
        }
        else if(CIE_dE_2000 < 10.0)
        {
            _color = vec3(1.0, 1.0, 0.0);
        }
        else if(CIE_dE_2000 < 50.0)
        {
            _color = vec3(1.0, 0.0, 0.0);
        }
        else if(CIE_dE_2000 < 1000)// [50.0 .. 100.0]
        {
            _color = vec3(1.0, 0.0,1.0);
        }
        
        //_color = vec4(col_dE, 1.0);
        //_color = vec3(CIE_dE_2000, CIE_dE_2000, CIE_dE_2000);

        _color = encode_sRGB(_color);
        out_color = vec4(_color.rgb, 1.0);
    }
}
