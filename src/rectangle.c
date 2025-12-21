#include "../vendor/raylib/raylib.h"
#include "../vendor/raylib/rlgl.h"

static Shader shader = (Shader){0};
static int radiusLoc =0;
static int widthLoc =0;
static int heightLoc =0;
static int locGradA =0;
static int locGradB =0;
static int locColA =0;
static int locColB =0;

static const char *round_plus_gradient_fs =
"#version 330\n"
"\n"
"in vec2 fragTexCoord;\n"
"out vec4 finalColor;\n"
"\n"
"uniform vec2 gradA;\n"
"uniform vec2 gradB;\n"
"uniform vec4 colorA;\n"
"uniform vec4 colorB;\n"
"uniform float width;\n"
"uniform float height;\n"
"uniform float radius;\n"
"\n"
"float roundedRect(vec2 p, vec2 b, float r)\n"
"{\n"
"    vec2 q = abs(p) - b + vec2(r);\n"
"    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - r;\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"    // UV corrigées indépendamment des triangles\n"
"    vec2 uv = fragTexCoord;\n"
"\n"
"    // Remap garanti [0..1]\n"
"    uv = clamp(uv, 0.0, 1.0);\n"
"\n"
"    // Passage en pixels, centré\n"
"    vec2 pos = (uv - 0.5) * vec2(width, height);\n"
"\n"
"    float d = roundedRect(pos, vec2(width, height) * 0.5, radius);\n"
"\n"
"    // Anti-aliasing stable\n"
"    float aa = fwidth(d);\n"
"    float alpha = 1.0 - smoothstep(0.0, aa, d);\n"
"\n"
"    if (alpha <= 0.0) discard;\n"
"\n"
"    // UV -> pixels locaux du rectangle\n"
"    vec2 pFrag = fragTexCoord * vec2(width, height);\n"
"\n"
"    vec2 ab = gradB - gradA;\n"
"    vec2 ap = pFrag - gradA;\n"
"\n"
"    float n = dot(ab, ab);\n"
"    float t = (n > 0.0) ? dot(ap, ab) / n : 0.0;\n"
"    t = clamp(t, 0.0, 1.0);\n"
"\n"
"    finalColor = mix(colorA, colorB, t);\n"
"}\n";

#define COLOR_TO_VEC(col) (Vector4){\
    .x=(col).r/255.0,\
    .y=(col).g/255.0,\
    .z=(col).b/255.0,\
    .w=(col).a/255.0,\
}

void DrawRectangleRoundedGradient(Rectangle rec,float roundness,Vector2 start,Vector2 end,Color from,Color to){
    if(shader.id==0){
        shader = LoadShaderFromMemory(0, round_plus_gradient_fs);
        radiusLoc = GetShaderLocation(shader, "radius");
        widthLoc = GetShaderLocation(shader, "width");
        heightLoc = GetShaderLocation(shader, "height");
        locGradA = GetShaderLocation(shader, "gradA");
        locGradB = GetShaderLocation(shader, "gradB");
        locColA = GetShaderLocation(shader, "colorA");
        locColB = GetShaderLocation(shader, "colorB");
    }

    if (roundness > 1.0f) roundness = 1.0f;
    float recSize = (rec.width < rec.height) ? rec.width : rec.height;
    float radius = (recSize * roundness) * 0.5f;
    start.x = start.x*rec.width;
    start.y = start.y*rec.height;
    end.x = end.x*rec.width;
    end.y = end.y*rec.height;
    SetShaderValue(shader, radiusLoc, &radius, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, widthLoc, &rec.width, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, heightLoc, &rec.height, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, locGradA, &start, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, locGradB, &end, SHADER_UNIFORM_VEC2);
    Vector4 fromVec = COLOR_TO_VEC(from);
    SetShaderValue(shader, locColA, &fromVec, SHADER_UNIFORM_VEC4);
    Vector4 toVec = COLOR_TO_VEC(to);
    SetShaderValue(shader, locColB, &toVec, SHADER_UNIFORM_VEC4);

    BeginShaderMode(shader);
        Vector2 q[4] = {
            { rec.x, rec.y  },
            { rec.x + rec.width, rec.y  },
            { rec.x + rec.width, rec.y + rec.height  },
            { rec.x, rec.y + rec.height  }
        };
        rlBegin(RL_TRIANGLES);
        rlTexCoord2f(0,0);rlVertex2f(q[0].x,q[0].y);
        rlTexCoord2f(1,1);rlVertex2f(q[2].x,q[2].y);
        rlTexCoord2f(1,0);rlVertex2f(q[1].x,q[1].y);
        rlTexCoord2f(0,0);rlVertex2f(q[0].x,q[0].y);
        rlTexCoord2f(0,1);rlVertex2f(q[3].x,q[3].y);
        rlTexCoord2f(1,1);rlVertex2f(q[2].x,q[2].y);
        rlEnd();
    EndShaderMode();
}
