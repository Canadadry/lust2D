#include "text.h"
#include "ui.h"
#include "vector2.h"
#include <math.h>

#define MAX_LINE 100


Line get_next_line(const char *txt, float width, FontParam param) {
    Line line = {0};
    Line word = {0};
    float last_space_size = 0;
    int last_codepoint = 0;
    if(txt==NULL || txt[0]==0){
        return (Line){.end_at=-1};
    }

    for (int index = 0; txt[index] != '\0'; index++) {
        int codepoint = txt[index];
        last_codepoint = codepoint;
        VECTOR2(float) size = sizer(param, codepoint);

        if (line.size.x + word.size.x + size.x > width && width > 0 && line.size.x > 0) {
            line.size.x -= last_space_size + 2 * param.spacing;
            return line;
        }

        word.end_at = index;
        word.size.x += size.x + param.spacing;
        word.size.y = fmax(word.size.y, size.y);

        if (codepoint == ' ' || codepoint == '\n') {
            last_space_size = size.x;
            line.end_at = word.end_at;
            line.size.x += word.size.x;
            line.size.y = fmax(word.size.y, line.size.y);
            line.max_width = fmax(word.size.x - last_space_size - 2 * param.spacing, line.max_width);
            word.size = (VECTOR2(float)){0.0, 0.0};

            if (codepoint == '\n') {
                line.size.x -= last_space_size + 2 * param.spacing;
                return line;
            }
        }
    }

    line.end_at = fmax(word.end_at, line.end_at);
    line.size.x += word.size.x - param.spacing;
    line.size.y = fmax(word.size.y, line.size.y);

    if (last_codepoint == ' ') {
        line.max_width = fmax(word.size.x - last_space_size - param.spacing, line.max_width);
    } else {
        line.max_width = fmax(word.size.x - param.spacing, line.max_width);
    }

    return line;
}

VECTOR2(int) mesure_text(const char *txt, float width, FontParam param) {
    VECTOR2(int) out = {0, 0};
    const char *remaining_txt = txt;

    for (int i = 0; i < MAX_LINE; i++) {
        Line line = get_next_line(remaining_txt, width, param);
        out.x = fmax(out.x, line.size.x);
        out.y += line.size.y + param.line_spacing;
        if (line.end_at < 0) {
            break;
        }
        remaining_txt = remaining_txt + line.end_at + 1;
    }

    return out;
}

void draw_text(const char *txt, Rectangle rect, FontParam param) {
    VECTOR2(int) dim = mesure_text(txt, (float)rect.width, param);
    const char *remaining_txt = txt;

    float y = rect.y + compute_align(param.align.y, rect.height - (int)dim.y);

    for (int i = 0; i < 100; i++) {
        Line line = get_next_line(remaining_txt, rect.width, param);
        if (line.end_at < 0) {
            break;
        }

        float x = rect.x + compute_align(param.align.x, rect.width - (int)dim.x);

        for (int j = 0; j <= line.end_at; j++) {
            drawer(param, remaining_txt[j], (VECTOR2(int)){x, y});
            VECTOR2(float) size = sizer(param, remaining_txt[j]);
            x += size.x + param.spacing;
        }

        y += line.size.y + param.line_spacing;
        remaining_txt = remaining_txt + line.end_at + 1;
    }
}
