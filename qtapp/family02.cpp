#include <functional>
#include <limits>

#include "fractals.h"

void Family02::FastPow2Inline(dbltype &x, dbltype &y) {
  dbltype x2 = x * x;
  dbltype y2 = y * y;
  y *= 2 * x;
  x = x2 - y2;
}

void Family02::FastPow3Inline(dbltype &x, dbltype &y) {
  dbltype xx = x * x;
  dbltype yy = y * y;
  x *= (xx - 3 * yy);
  y *= (3 * xx - yy);
}

void Family02::FastPow4Inline(dbltype &x, dbltype &y) {
  dbltype xx = x * x;
  dbltype yy = y * y;
  y *= 4 * x * (xx - yy);
  x = xx * xx - 6 * xx * yy + yy * yy;
}

void Family02::FastPow5Inline(dbltype &x, dbltype &y) {
  dbltype x2 = x * x;
  dbltype x4 = x2 * x2;
  dbltype y2 = y * y;
  dbltype y4 = y2 * y2;
  dbltype cm = -10 * x2 * y2;
  x *= (x4 + 5 * y4 + cm);
  y *= (y4 + 5 * x4 + cm);
}

void Family02::FastPow6Inline(dbltype &x, dbltype &y) {
  dbltype x2 = x * x;
  dbltype x4 = x2 * x2;
  dbltype y2 = y * y;
  dbltype y4 = y2 * y2;
  y *= x * (6 * x4 - 20.0 * x2 * y2 + 6 * y4);
  x = x4 * (x2 - 15 * y2) + y4 * (15 * x2 - y2);
}

void Family02::FastPowNInline(dbltype &x, dbltype &y, const int n) {
  cmplx r(x, y);
  r = std::pow(r, n);
  x = r.real();
  y = r.imag();
}

void Family02::Init(const FractalParameters &p) {
  Fractal::Init(p);
  c_.real(p.c.real());
  c_.imag(p.c.imag());
  n_ = p.n;

  switch (n_) {
    case 3:
      funct_ = std::bind(Family02::FastPow3Inline, std::placeholders::_1,
                         std::placeholders::_2);
      break;
    case 4:
      funct_ = std::bind(Family02::FastPow4Inline, std::placeholders::_1,
                         std::placeholders::_2);
      break;
    case 5:
      funct_ = std::bind(Family02::FastPow5Inline, std::placeholders::_1,
                         std::placeholders::_2);
      break;
    case 6:
      funct_ = std::bind(Family02::FastPow6Inline, std::placeholders::_1,
                         std::placeholders::_2);
      break;
    default:
      funct_ = std::bind(Family02::FastPowNInline, std::placeholders::_1,
                         std::placeholders::_2, n_);
  }
}

double Family02::CalcFinalNormMandelbrot(const cmplx &c) const {
  int iter = 1;
  dbltype x = c_.real();
  dbltype y = c_.imag();
  dbltype dist = std::numeric_limits<double>::max();
  for (; iter < max_iter_; ++iter) {
    dist = std::min(orbit_metric_funct_(x, y), dist);
    if (x * x + y * y >= th_norm_) break;
    funct_(x, y);
    x += c.real();
    y += c.imag();
  }
  return dist;
}

double Family02::CalcFinalNormJulia(const cmplx &z) const {
  int iter = 0;
  dbltype x = z.real();
  dbltype y = z.imag();
  dbltype dist = std::numeric_limits<dbltype>::max();
  for (; iter < max_iter_; ++iter) {
    dist = std::min(orbit_metric_funct_(x, y), dist);
    if (x * x + y * y >= th_norm_) break;

    funct_(x, y);
    x += c_.real();
    y += c_.imag();
  }
  return dist;
}

//////////////////////////////////////////
double Family02::CalcEscapeJulia(const cmplx &z) const {
  int iter = 0;
  dbltype x = z.real();
  dbltype y = z.imag();

  for (;;) {
    if (x * x + y * y >= th_norm_ || iter >= max_iter_) break;
    funct_(x, y);
    x += c_.real();
    y += c_.imag();
    ++iter;
  }
  return iter;
}

double Family02::CalcEscapeMandelbrot(const cmplx &c) const {
  int iter = 1;
  dbltype x = c_.real();
  dbltype y = c_.imag();
  for (;;) {
    if (x * x + y * y >= th_norm_ || iter >= max_iter_) break;
    funct_(x, y);
    x += c.real();
    y += c.imag();
    ++iter;
  }
  return iter;
}
