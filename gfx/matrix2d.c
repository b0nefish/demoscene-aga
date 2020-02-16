#include <math.h>

#include "gfx/matrix2d.h"

#define M_FOREACH(I, J)           \
  for ((I) = 0; (I) < 3; (I)++)   \
    for ((J) = 0; (J) < 3; (J)++)

#define M(A,I,J) (*A)[I][J]

void Multiply2D(Matrix2D *d, Matrix2D *a, Matrix2D *b) {
  int i, j, k;

  M_FOREACH(i, j) {
    M(d,i,j) = 0.0f;

    for (k = 0; k < 3; k++)
      M(d,i,j) += M(a,i,k) * M(b,k,j);
  }
}

void Transpose2D(Matrix2D *d, Matrix2D *a) {
  int i, j; 

  M_FOREACH(i, j)
    M(d,i,j) = M(a,j,i);
}

void LoadIdentity2D(Matrix2D *d) {
  int i, j; 

  M_FOREACH(i, j)
    M(d,i,j) = (i == j) ? 1.0f : 0.0f;
}

void LoadRotation2D(Matrix2D *d, float angle) {
  LoadIdentity2D(d);

  angle *= 3.14159265f / 180.0f;

  M(d,0,0) = cos(angle);
  M(d,1,0) = -sin(angle);
  M(d,0,1) = sin(angle);
  M(d,1,1) = cos(angle);
}

void LoadScaling2D(Matrix2D *d, float scaleX, float scaleY) {
  LoadIdentity2D(d);

  M(d,0,0) = scaleX;
  M(d,1,1) = scaleY;
}

void LoadTranslation2D(Matrix2D *d, float moveX, float moveY) {
  LoadIdentity2D(d);

  M(d,2,0) = moveX;
  M(d,2,1) = moveY;
}

void Transform2D(PointT *dst, PointT *src, int n, Matrix2D *m) {
  int i;

  for (i = 0; i < n; i++) {
    float x = src[i].x;
    float y = src[i].y;

    dst[i].x = (int16_t)(M(m,0,0) * x + M(m,1,0) * y + M(m,2,0));
    dst[i].y = (int16_t)(M(m,0,1) * x + M(m,1,1) * y + M(m,2,1));
  }
}
