#include "rectangle.h"
#include "../vendor/raylib/rlgl.h"
#include <math.h>


static Color ComputeColor(const RectangleDrawOption *opt, float x, float y)
{
    Vector2 a = opt->start;
    Vector2 b = opt->end;

    Vector2 ab = { b.x - a.x, b.y - a.y };
    Vector2 ap = { x - a.x, y - a.y };

    float s = ab.x*ap.x + ab.y*ap.y;
    float n = ab.x*ab.x + ab.y*ab.y;

    float p = (n > 0.0f) ? (s / n) : 0.0f;
    if (p < 0.0f) p = 0.0f;
    if (p > 1.0f) p = 1.0f;

    Color ca = opt->left;
    Color cb = opt->right;

    Color c;
    c.r = (unsigned char)(cb.r*p + ca.r*(1.0f - p));
    c.g = (unsigned char)(cb.g*p + ca.g*(1.0f - p));
    c.b = (unsigned char)(cb.b*p + ca.b*(1.0f - p));
    c.a = (unsigned char)(cb.a*p + ca.a*(1.0f - p));

    return c;
}

static inline void VertexGradient(const RectangleDrawOption *opt, Vector2 p)
{
    Color c = ComputeColor(opt, p.x, p.y);
    rlColor4ub(c.r, c.g, c.b, c.a);
    rlVertex2f(p.x, p.y);
}

void DrawRectangleRoundedGradient(RectangleDrawOption opt)
{
    Rectangle rec = opt.rec;
    float roundness = opt.roundness;
    int segments = opt.segments;

    if (rec.width < 1 || rec.height < 1)
        return;

    if (roundness <= 0.0f)
    {
        rlBegin(RL_TRIANGLES);

            Vector2 p0 = { rec.x, rec.y };
            Vector2 p1 = { rec.x + rec.width, rec.y };
            Vector2 p2 = { rec.x + rec.width, rec.y + rec.height };
            Vector2 p3 = { rec.x, rec.y + rec.height };

            VertexGradient(&opt, p0);
            VertexGradient(&opt, p2);
            VertexGradient(&opt, p1);

            VertexGradient(&opt, p0);
            VertexGradient(&opt, p3);
            VertexGradient(&opt, p2);

        rlEnd();
        return;
    }

    if (roundness > 1.0f) roundness = 1.0f;

    float recSize = (rec.width < rec.height) ? rec.width : rec.height;
    float radius = (recSize * roundness) * 0.5f;
    float step = 90.0f / segments;

    Vector2 centers[4] = {
        { rec.x + radius, rec.y + radius },
        { rec.x + rec.width - radius, rec.y + radius },
        { rec.x + rec.width - radius, rec.y + rec.height - radius },
        { rec.x + radius, rec.y + rec.height - radius }
    };

    float angles[4] = { 180.0f, 270.0f, 0.0f, 90.0f };

    rlBegin(RL_TRIANGLES);

    for (int k = 0; k < 4; k++)
    {
        float angle = angles[k];
        Vector2 c = centers[k];

        for (int i = 0; i < segments; i++)
        {
            Vector2 p0 = c;
            Vector2 p1 = {
                c.x + cosf(DEG2RAD*(angle + step)) * radius,
                c.y + sinf(DEG2RAD*(angle + step)) * radius
            };
            Vector2 p2 = {
                c.x + cosf(DEG2RAD*angle) * radius,
                c.y + sinf(DEG2RAD*angle) * radius
            };

            VertexGradient(&opt, p0);
            VertexGradient(&opt, p1);
            VertexGradient(&opt, p2);

            angle += step;
        }
    }

    Vector2 p[4] = {
        { rec.x + radius, rec.y },
        { rec.x + rec.width - radius, rec.y },
        { rec.x + rec.width - radius, rec.y + rec.height },
        { rec.x + radius, rec.y + rec.height }
    };

    VertexGradient(&opt, p[0]);
    VertexGradient(&opt, p[2]);
    VertexGradient(&opt, p[1]);

    VertexGradient(&opt, p[0]);
    VertexGradient(&opt, p[3]);
    VertexGradient(&opt, p[2]);

    Vector2 p2[4] = {
        { rec.x             , rec.y + radius},
        { rec.x + rec.width , rec.y + radius},
        { rec.x + rec.width , rec.y + rec.height - radius },
        { rec.x             , rec.y + rec.height - radius}
    };

    VertexGradient(&opt, p2[0]);
    VertexGradient(&opt, p2[2]);
    VertexGradient(&opt, p2[1]);

    VertexGradient(&opt, p2[0]);
    VertexGradient(&opt, p2[3]);
    VertexGradient(&opt, p2[2]);

    rlEnd();
}
