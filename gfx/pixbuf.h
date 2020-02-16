#ifndef __GFX_PIXBUF_H__
#define __GFX_PIXBUF_H__

#include "std/types.h"
#include "gfx/palette.h"
#include "std/fp16.h"

#define PIXBUF_GRAY 0
#define PIXBUF_CLUT 1
#define PIXBUF_RGB  2
#define PIXBUF_RGBA 3

typedef enum {
  BLIT_NORMAL,
  BLIT_TRANSPARENT,
  BLIT_ADDITIVE,
  BLIT_ADDITIVE_CLIP,
  BLIT_SUBSTRACTIVE,
  BLIT_SUBSTRACTIVE_CLIP,
  BLIT_COLOR_MAP,
  BLIT_COLOR_FUNC
} BlitModeT;

typedef struct PixBuf PixBufT;

struct PixBuf {
  /* Basic information. */
  uint16_t type;
  BlitModeT mode;
  uint32_t width, height;

  /* Pixel data. */
  bool ownership; /* false if PixBuf wraps a table */
  uint8_t *data;

  /* Valid only in GRAY or CLUT mode. */
  uint16_t uniqueColors;  /* stores number of unique colors in the image */
  uint8_t baseColor;
  uint8_t lastColor;

  /* Foreground and background color - for drawing. */
  uint8_t fgColor;
  uint8_t bgColor;

  union {
    /* For BLIT_COLOR_MAP mode. */
    uint8_t *cmap;
    /* For BLIT_COLOR_FUNC mode. */
    uint8_t *cfunc;
  } blit;
};

PixBufT *NewPixBuf(uint16_t type, size_t width, size_t height);
PixBufT *NewPixBufFromFile(const char *fileName);
PixBufT *NewPixBufWrapper(size_t width, size_t height, uint8_t *data);

void PixBufSwapData(PixBufT *buf1, PixBufT *buf2);
void PixBufCopy(PixBufT *dst, PixBufT *src);
void PixBufClear(PixBufT *pixbuf);
void PixBufSetColorMap(PixBufT *pixbuf, PixBufT *colorMap);
void PixBufSetColorFunc(PixBufT *pixbuf, uint8_t *colorFunc);
BlitModeT PixBufSetBlitMode(PixBufT *pixbuf, BlitModeT mode);
void PixBufRemap(PixBufT *pixbuf, PaletteT *palette);
void PixBufCalculateHistogram(PixBufT *pixbuf);

static inline void PutPixel(PixBufT *pixbuf, int x, int y, uint8_t c) {
  pixbuf->data[x + pixbuf->width * y] = c;
}

static inline uint8_t GetPixel(PixBufT *pixbuf, int x, int y) {
  return pixbuf->data[x + pixbuf->width * y];
}

static inline void PutPixelRGB(PixBufT *pixbuf, int x, int y, RGB c) {
  ((uint32_t *)pixbuf->data)[x + pixbuf->width * y] = *(uint32_t *)&c;
}

static inline RGB GetPixelRGB(PixBufT *pixbuf, int x, int y) {
  return *(RGB *)&pixbuf->data[x + pixbuf->width * y];
}

__regargs int GetFilteredPixel(PixBufT *pixbuf, FP16 x, FP16 y);

#endif
