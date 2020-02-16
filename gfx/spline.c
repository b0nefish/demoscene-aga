#include "std/debug.h"
#include "std/math.h"
#include "std/memory.h"
#include "gfx/spline.h"

/*
 * http://en.wikipedia.org/wiki/Hermite_curve
 *
 * @param t: [0.0, 1.0]
 */
typedef struct CubicPolynomial {
  float h00;
  float h10;
  float h01;
  float h11;
} CubicPolynomialT;

__regargs static
void HermiteCubicPolynomial(float t, CubicPolynomialT *poly) {
  /*
   * h00(t) = 2*t^3 - 3*t^2 + 1
   *        = 2*(t^3 - t^2) - (t^2 + 1)
   *        = 2*h11(t) - t^2 + 1
   *
   * h10(t) = t^3 - 2*t^2 + t
   *        = (t^3 - t^2) - t^2 + t
   *        = h11(t) - t^2 + t
   *
   * h01(t) = -2*t^3 + 3*t^2
   *        = -2*t^3 + 2*t^2 + t^2
   *        = -2*(t^3 - t^2) + t^2
   *        = -2*h11(t) + t^2
   *        = -(2*h11(t) - t^2)
   *
   * h11(t) = t^3 - t^2
   *
   * H(t) = h00(t) * p0 + h10(t) * m0 + h01(t) * p1 + h11(t) * m1
   */
  float t2 = t * t;
  float t3 = t2 * t;

  float h11 = t3 - t2;

  float dh11 = h11 + h11;
  float dh11mt2 = dh11 - t2;

  float h00 = dh11mt2 + 1;
  float h10 = h11 - t2 + t;
  float h01 = -dh11mt2;

  poly->h00 = h00;
  poly->h10 = h10;
  poly->h01 = h01;
  poly->h11 = h11;
}

/*
 * Spline implementation.
 */

static void DeleteSpline(SplineT *self) {
  MemUnref(self->knots);
}

TYPEDECL(SplineT, (FreeFuncT)DeleteSpline);

SplineT *NewSpline(size_t knots, bool closed) {
  SplineT *spline = NewInstance(SplineT);

  spline->knots = NewTable(SplineKnotT, knots);
  spline->closed = closed;

  return spline;
}

__regargs static
SplineKnotT *SplineGetKnot(SplineT *spline, ssize_t knot) {
  size_t knots = TableSize(spline->knots);

  if (knot < 0) {
    knot = spline->closed ? (knot + knots) : 0;
  } else if (knot >= knots) {
    knot = spline->closed ? (knot - knots) : (knots - 1);
  }

  ASSERT((knot >= 0) && (knot < knots), "Knot number (%d) out of range.", (int)knot);

  return &spline->knots[knot];
}

__regargs static
size_t SplineKnots(SplineT *spline) {
  return TableSize(spline->knots) + (spline->closed ? 1 : 0);
}

__regargs static
float SplineEvalWithinInterval(SplineT *spline, float t, size_t knot) {
  SplineKnotT *p0 = SplineGetKnot(spline, knot);
  SplineKnotT *p1 = SplineGetKnot(spline, knot + 1);
  CubicPolynomialT poly;

  HermiteCubicPolynomial(t, &poly);

  return poly.h00 * p0->value + poly.h10 * p0->tangent +
         poly.h01 * p1->value + poly.h11 * p1->tangent;
}

float SplineEval(SplineT *spline asm("a0"), float t asm("fp0")) {
  float interval;

  /* Check if ping-pong. */
  if (!spline->closed) {
    /* Limit to [0.0, 2.0f). Reverse direction iff t > 1.0f. */
    t = modff(t * 0.5f, &interval) * 2.0f;
    if (t > 1.0f)
      t = 2.0f - t;
  }

  t = modff(t * (int)(SplineKnots(spline) - 1), &interval);

  return SplineEvalWithinInterval(spline, t, (int)interval);
}

void SplineInterpolate(SplineT *spline, size_t steps, PtrT array, SetItemFuncT writer) {
  float t, value, stride, knotf;
  size_t step, knot;

  stride = (float)(int)SplineKnots(spline) / (int)steps;

  for (step = 0, knot = 0, t = 0.0f; step < steps; step++) {
    value = SplineEvalWithinInterval(spline, t, knot);

    writer(array, step, &value);

    t = modff(t + stride, &knotf);
    knot += lroundf(knotf);
  }
}

void SplineAttachCatmullRomTangents(SplineT *spline) {
  size_t knot;
  size_t first = spline->closed ? 0 : 1;
  size_t last = TableSize(spline->knots) - first;

  for (knot = first; knot < last; knot++) {
    SplineKnotT *pA = SplineGetKnot(spline, knot - 1);
    SplineKnotT *pB = SplineGetKnot(spline, knot);
    SplineKnotT *pC = SplineGetKnot(spline, knot + 1);

    pB->tangent = 0.5f * (pA->value + pC->value);
  }
}
