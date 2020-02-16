#include "ellipse.h"

typedef struct Segment {
  uint8_t *upper;
  uint8_t *lower;
  size_t stride;
  uint8_t color;
} SegmentT;

static void DrawEllipseSegment(SegmentT *seg, int x) {
  int i;

  for (i = -x; i < x; i++) {
    seg->upper[i] = seg->color;
    seg->lower[i] = seg->color;
  }

  seg->upper += seg->stride;
  seg->lower -= seg->stride;
}

void DrawEllipse(PixBufT *canvas, int xc, int yc, int a, int b) {
  int a2 = a * a;
  int b2 = b * b;
  int x = 0;
  int y = b;

  /* kryterium przejscia z stałego przyrostu X na staly przyrost Y */
  int dx = 2 * b2 * x;
  int dy = 2 * a2 * y;

  /* poczatkowa wartosc (b - a/2) ^ 2 */
  int d = b2 - a2 * b + a2 / 4;

  SegmentT seg;

  seg.stride = canvas->width;
  seg.color = canvas->fgColor;
  seg.upper = canvas->data + (yc - b) * seg.stride + xc;
  seg.lower = canvas->data + (yc + b) * seg.stride + xc;

  while (dx < dy) {
    if (d > 0) {
      DrawEllipseSegment(&seg, x);
      y--;

      // dSE = dE - 2 * a^2 * Y(i+1)
      dy -= 2 * a2;
      d -= dy;
    }

    x++;

    // dE = 2 * b^2 * X(i+1) + b^2
    dx += 2 * b2;
    d += dx + b2;
  }

  d += ((3 * (a2 - b2)) / 2 - (dx + dy)) / 2;

  while (y >= 0) {
    DrawEllipseSegment(&seg, x);
    y--;

    if (d < 0) {
      x++;

      // dSE = dS + 2 * b^2 * X(i+1)
      dx += 2 * b2;
      d += dx;
    }

    // dS = a^2 - 2 * a^2 * Y(i+1)
    dy -= 2 * a2;
    d += a2 - dy;
  }
}
