#ifndef _TEXT_H
#define _TEXT_H

#include "vector2.h"
#include "ui.h"

 typedef struct {
	int end_at;
	VECTOR2(float) size;
	float max_width;
}Line;


typedef struct  {
    void* Font;
    float size;
    float spacing;
    float line_spacing;
    UiColor color;
    VECTOR2(Align) align;
}FontParam;

VECTOR2(float) sizer(FontParam param, int rune);
void drawer(FontParam param, int rune, VECTOR2(int) pos);

VECTOR2(int) mesure_text(const char *txt, float width, FontParam param);
void draw_text(const char *txt, Rectangle rect, FontParam param);

#endif //_TEXT_H
