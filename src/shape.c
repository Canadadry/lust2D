#include "shape.h"
#include "dynamicarray.h"
#define MAX_INTERSECTION 512

WRITE_ARRAY_IMPL(Point);

static int intersect_scanline(Point a, Point b, int y, int* x_out) {
    double dy1 = a.y - y;
    double dy2 = b.y - y;
    if ((dy1 < 0 && dy2 >= 0) || (dy2 < 0 && dy1 >= 0)) {
        *x_out = (int)(a.x + (y - a.y) * (b.x - a.x) / (b.y - a.y));
        return 1;
    }
    return 0;
}

static void sort_nodes(int* nodes, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (nodes[j] < nodes[i]) {
                int t = nodes[i];
                nodes[i] = nodes[j];
                nodes[j] = t;
            }
        }
    }
}

inline static BufColor blend_colors(BufColor a, BufColor b) {
    BufColor result;
    float alpha_a = a.a / 255.0f;
    float alpha_b = b.a / 255.0f;
    float alpha_o = alpha_a + alpha_b * (1 - alpha_a);

    if (alpha_o < 1e-6f) {
        result.r = result.g = result.b = result.a = 0;
        return result;
    }

    result.r = (unsigned char)((a.r * alpha_a + b.r * alpha_b * (1 - alpha_a)) / alpha_o);
    result.g = (unsigned char)((a.g * alpha_a + b.g * alpha_b * (1 - alpha_a)) / alpha_o);
    result.b = (unsigned char)((a.b * alpha_a + b.b * alpha_b * (1 - alpha_a)) / alpha_o);
    result.a = (unsigned char)(alpha_o * 255);
    return result;
}

static void draw_scanline_rgba(ImageBuffer img, int y, int x1, int x2, BufColor color) {
    if (y < 0 || (size_t)y >= img.h || x2 < 0 || x1 >= (int)img.w) return;
    if (x1 < 0) x1 = 0;
    if (x2 >= (int)img.w) x2 = (int)img.w - 1;
    for (int x = x1; x <= x2; x++) {
        unsigned char * pix = img.buf+4*(img.w*y+x);
        *(BufColor*)pix =blend_colors(color,*(BufColor*)pix);
    }
}

void fill_polygon(ARRAY(Point) pts, ImageBuffer buffer, BufColor color) {
    int nodes[MAX_INTERSECTION];
    for (int y = 0; y < buffer.h; y++) {
        int count = 0;
        for (int i = 0; i < pts.len; i++) {
            int j = (i + 1) % pts.len;
            if (intersect_scanline(pts.data[j], pts.data[i], y, &nodes[count])){
                count++;
            }
        }
        if (count < 2) continue;
        sort_nodes(nodes, count);
        for (int i = 0; i + 1 < count; i += 2) {
            draw_scanline_rgba(buffer, y, nodes[i], nodes[i + 1], color);
        }
    }
}


int bezier_to(ARRAY(Point)* pts, Point p2, Point p3, Point p4, int n) {
    if(pts->len==0){
        return 0;
    }
    if(n<2){
        return 0;
    }
    Point p1 = pts->data[pts->len-1];
    double dt = 1.0 / (n - 1);
    for (int i = 1; i < n; i++) {
        double t = i * dt;
        double t2 = t * t;
        double t3 = t2 * t;
        double u = 1 - t;
        double u2 = u * u;
        double u3 = u2 * u;
        array_append_Point(pts,(Point){
            .x = u3*p1.x + 3*u2*t*p2.x + 3*u*t2*p3.x + t3*p4.x,
            .y = u3*p1.y + 3*u2*t*p2.y + 3*u*t2*p3.y + t3*p4.y,
        });
    }
    return 1;
}
