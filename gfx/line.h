#ifndef __GFX_LINE_H__
#define __GFX_LINE_H__

#include "gfx/common.h"
#include "gfx/pixbuf.h"

__attribute__((regparm(4))) void
DrawLineUnsafe(PixBufT *canvas, int xs, int ys, int xe, int ye);
void DrawLine(PixBufT *canvas, int x1, int y1, int x2, int y2);
void DrawPolyLine(PixBufT *canvas, PointT *points, int n, bool closed);

void DrawLineAA(PixBufT *canvas, float x0, float y0, float x1, float y1);

#endif
