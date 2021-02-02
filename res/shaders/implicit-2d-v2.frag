#version 330 core

layout(location = 0) out vec4 color;

uniform float u_SupersamplingSide;

uniform vec2 u_Resolution;
uniform vec4 u_BoundingBox;

const int u_ParamsCount = 5;
uniform float u_Params[u_ParamsCount] = { 1, 2, 3, 4, 5 };

const int Size = 1000;
uniform vec2[Size] u_Symbols;

uniform vec3 u_FalseColor;
uniform vec3 u_TrueColor;

vec4 notZero(vec4 a)
{
    return vec4(all(lessThan(a, vec4(0.0))) || all(greaterThan(a, vec4(0.0))));
}

vec4 zero(vec4 a)
{
    return vec4(!(all(lessThan(a, vec4(0.0))) || all(greaterThan(a, vec4(0.0)))));
}

vec4 lessOperator(vec4 a, vec4 b)
{
    return vec4(all(lessThan(a, b)));
}

vec4 lessEqualOperator(vec4 a, vec4 b)
{
    return vec4(!all(greaterThan(a, b)));
}

vec4 greaterOperator(vec4 a, vec4 b)
{
    return vec4(all(greaterThan(a, b)));
}

vec4 greaterEqualOperator(vec4 a, vec4 b)
{
    return vec4(!all(lessThan(a, b)));
}

vec4 equalOperator(vec4 a, vec4 b)
{
    return vec4(!(all(lessThan(a, b)) || all(greaterThan(a, b))));
}

vec4 notEqualOperator(vec4 a, vec4 b)
{
    return vec4(all(lessThan(a, b)) || all(greaterThan(a, b)));
}

vec4 andOperator(vec4 a, vec4 b)
{
    return vec4(bvec4(notZero(a)).x && bvec4(notZero(b)).x);
}

vec4 orOperator(vec4 a, vec4 b)
{
    return vec4(bvec4(notZero(a)).x || bvec4(notZero(b)).x);
}

vec4 xorOperator(vec4 a, vec4 b)
{
    return vec4(bvec4(notZero(a)).x ^^ bvec4(notZero(b)).x);
}

vec4 notOperator( vec4 a )
{
    return vec4(!bvec4(notZero(a)).x);
}

float eval(vec4 v)
{
    vec4[Size] args;
    int n = 0;
    int i = 0;
    while (u_Symbols[i].x != 0.0)
    {
        int type = int(u_Symbols[i].x);
        if (type == 1)
        {
            args[n] = vec4(u_Symbols[i].y);
            ++n;
        }
        else if (type == 2)
        {
            int paramId = int(u_Symbols[i].y);
            args[n] = vec4(u_Params[paramId]);
            ++n;
        }
        else if (type == 3)
        {
            int variable = int(u_Symbols[i].y);
            if (variable == 0)
            {
                args[n] = vec4(v.x, v.x, v.z, v.z);
                ++n;
            }
            else if (variable == 1)
            {
                args[n] = vec4(v.y, v.w, v.w, v.y);
                ++n;
            }
        }
        else if (type == 4 || type == 5)
        {
            int command = int(u_Symbols[i].y);
            switch (command)
            {
                case 0: args[n - 2]=args[n - 2] * args[n - 1]; --n; break;
                case 1: args[n - 2]=args[n - 2] / args[n - 1]; --n; break;
                case 2: args[n - 2]=args[n - 2] + args[n - 1]; --n; break;
                case 3: args[n - 2]=args[n - 2] - args[n - 1]; --n; break;
                case 4: args[n - 2]=lessOperator(args[n - 2], args[n - 1]); --n; break;
                case 5: args[n - 2]=lessEqualOperator(args[n - 2], args[n - 1]); --n; break;
                case 6: args[n - 2]=greaterOperator(args[n - 2], args[n - 1]); --n; break;
                case 7: args[n - 2]=greaterEqualOperator(args[n - 2], args[n - 1]); --n; break;
                case 8: args[n - 2]=equalOperator(args[n - 2], args[n - 1]); --n; break;
                case 9: args[n - 2]=notEqualOperator(args[n - 2], args[n - 1]); --n; break;
                case 10: args[n - 2]=andOperator(args[n - 2], args[n - 1]); --n; break;
                case 11: args[n - 2]=xorOperator(args[n - 2], args[n - 1]); --n; break;
                case 12: args[n - 2]=orOperator(args[n - 2], args[n - 1]); --n; break;
                case 13: args[n - 1]=-args[n - 1]; break;
                case 14: args[n - 1]=+args[n - 1]; break;
                case 15: args[n - 1]=radians(args[n - 1]); break;
                case 16: args[n - 1]=degrees(args[n - 1]); break;
                case 17: args[n - 1]=sin(args[n - 1]); break;
                case 18: args[n - 1]=cos(args[n - 1]); break;
                case 19: args[n - 1]=tan(args[n - 1]); break;
                case 20: args[n - 1]=asin(args[n - 1]); break;
                case 21: args[n - 1]=acos(args[n - 1]); break;
                case 22: args[n - 2]=atan(args[n - 2], args[n - 1]); --n; break;
                case 23: args[n - 1]=atan(args[n - 1]); break;
                case 24: args[n - 1]=sinh(args[n - 1]); break;
                case 25: args[n - 1]=cosh(args[n - 1]); break;
                case 26: args[n - 1]=tanh(args[n - 1]); break;
                case 27: args[n - 1]=asinh(args[n - 1]); break;
                case 28: args[n - 1]=acosh(args[n - 1]); break;
                case 29: args[n - 1]=atanh(args[n - 1]); break;
                case 30: args[n - 2]=pow(args[n - 2], args[n - 1]); --n; break;
                case 31: args[n - 1]=exp(args[n - 1]); break;
                case 32: args[n - 1]=log(args[n - 1]); break;
                case 33: args[n - 1]=exp2(args[n - 1]); break;
                case 34: args[n - 1]=log2(args[n - 1]); break;
                case 35: args[n - 1]=sqrt(args[n - 1]); break;
                case 36: args[n - 1]=inversesqrt(args[n - 1]); break;
                case 37: args[n - 1]=abs(args[n - 1]); break;
                case 38: args[n - 1]=sign(args[n - 1]); break;
                case 39: args[n - 1]=floor(args[n - 1]); break;
                case 40: args[n - 1]=trunc(args[n - 1]); break;
                case 41: args[n - 1]=round(args[n - 1]); break;
                case 42: args[n - 1]=roundEven(args[n - 1]); break;
                case 43: args[n - 1]=ceil(args[n - 1]); break;
                case 44: args[n - 1]=fract(args[n - 1]); break;
                case 45: args[n - 2]=mod(args[n - 2], args[n - 1]); --n; break;
                case 46: args[n - 2]=modf(args[n - 2], args[n - 1]); --n; break;
                case 47: args[n - 2]=min(args[n - 2], args[n - 1]); --n; break;
                case 48: args[n - 2]=max(args[n - 2], args[n - 1]); --n; break;
                case 49: args[n - 3]=clamp(args[n - 3], args[n - 2], args[n - 1]); n -= 2; break;
                case 50: args[n - 3]=mix(args[n - 3], args[n - 2], args[n - 1]); n -= 2; break;
                case 51: args[n - 2]=step(args[n - 2], args[n - 1]); --n; break;
                case 52: args[n - 3]=smoothstep(args[n - 3], args[n - 2], args[n - 1]); n -= 2; break;
                case 53: args[n - 1]=vec4(any(isnan(args[n - 1]))); break;
                case 54: args[n - 1]=vec4(any(isinf(args[n - 1]))); break;
            #if __VERSION__ >= 400
                case 55: args[n - 3]=fma(args[n - 3], args[n - 2], args[n - 1]); n -= 2; break;
                case 56: args[n - 2]=frexp(args[n - 2], args[n - 1]); --n; break;
            #endif
                case 58: { vec4 arg1 = args[n - 1]; args[n - 1]=vec4(length(arg1.x), length(arg1.y), length(arg1.z), length(arg1.w)); break; }
                case 59: { vec4 arg1 = args[n - 1]; vec4 arg2 = args[n - 2]; args[n - 2]=vec4(distance(arg2.x, arg1.x), distance(arg2.y, arg1.y), distance(arg2.z, arg1.z), distance(arg2.w, arg1.w)); --n; break; }
                case 60: { vec4 arg1 = args[n - 1]; vec4 arg2 = args[n - 2]; args[n - 2]=vec4(dot(arg2.x, arg1.x), dot(arg2.y, arg1.y), dot(arg2.z, arg1.z), dot(arg2.w, arg1.w)); --n; } break;
                case 61: args[n - 1]=normalize(args[n - 1]); break;
                case 62: args[n - 3]=faceforward(args[n - 3], args[n - 2], args[n - 1]); n -= 2; break;
                case 63: args[n - 2]=reflect(args[n - 2], args[n - 1]); --n; break;
                //                case 64: args[n - 1]=refract(args[n - 3], args[n - 2], args[n - 1]); n -= 2; break;
                case 65: args[n - 1]=notOperator(args[n - 1]); break;

            }
        }
        ++i;
    }
    return notZero(args[0]).x;
}

void main()
{
    float sum = 0.0;
    for (float i = 0.0; i < u_SupersamplingSide; ++i)
    {
        for (float j = 0.0; j < u_SupersamplingSide; ++j)
        {
            vec2 size = u_BoundingBox.zw / u_Resolution / u_SupersamplingSide;
            vec2 uv = (gl_FragCoord.xy / u_Resolution) * u_BoundingBox.zw + u_BoundingBox.xy + vec2(i, j) * size;

            // .x -> x_min, .y -> y_min, .z -> x_max, .w -> y_max
            vec4 v = vec4(uv, uv + size);
            sum += eval(v);
        }
    }
    color = mix(vec4(u_FalseColor, 1.0), vec4(u_TrueColor, 1.0), sum / (u_SupersamplingSide * u_SupersamplingSide));
}