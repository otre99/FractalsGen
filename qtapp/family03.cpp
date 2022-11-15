#include "fractals.h"
#include "iostream"
#include <functional>
#include <limits>
#include <numeric>

dbltype reps = 1e-7;

void Family03::Init(const FractalParameters &p) {
  Fractal::Init(p);
  c_.real(p.c.real());
  c_.imag(p.c.imag());
  n_ = p.n;
  alpha_ = static_cast<dbltype>(n_ - 1) / n_;
  switch (n_ - 1) {
  case 1:
    funct_ = std::bind(Family02::FastPow1Inline, std::placeholders::_1,
                       std::placeholders::_2);
    break;
  case 2:
    funct_ = std::bind(Family02::FastPow2Inline, std::placeholders::_1,
                       std::placeholders::_2);
    break;
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
                       std::placeholders::_2, n_ - 1);
  }
}

double Family03::CalcFinalNormMandelbrot(const cmplx &c) const {
  return CalcFinalNormJulia(c);
}

double Family03::CalcFinalNormJulia(const cmplx &z) const {
  int iter = 1;
  dbltype rdem;
  dbltype x1, y1, x2, y2;
  dbltype x = z.real();
  dbltype y = z.imag();
  dbltype dist = std::numeric_limits<double>::max();
  for (; iter < max_iter_; ++iter) {
    x1 = x;
    y1 = y;
    funct_(x, y);
    x *= n_;
    y *= n_;
    rdem = x * x + y * y;
    x2 = x1 * alpha_ - (c_.real() * x + c_.imag() * y) / rdem;
    y2 = y1 * alpha_ - (c_.imag() * x - c_.real() * y) / rdem;

    x = x2;
    y = y2;
    rdem = std::max(std::abs(x1 - x), std::abs(y1 - y));
    dist = std::min(dist, orbit_metric_funct_(x, y));
    if (rdem < reps)
      break;
  }
  return dist;
}

//////////////////////////////////////////
double Family03::CalcEscapeJulia(const cmplx &z) const {
  int iter = 1;
  dbltype rdem;
  dbltype x1, y1, x2, y2;
  dbltype x = z.real();
  dbltype y = z.imag();
  for (; iter < max_iter_; ++iter) {
    x1 = x;
    y1 = y;
    funct_(x, y);
    x *= n_;
    y *= n_;
    rdem = x * x + y * y;
    x2 = x1 * alpha_ - (c_.real() * x + c_.imag() * y) / rdem;
    y2 = y1 * alpha_ - (c_.imag() * x - c_.real() * y) / rdem;

    x = x2;
    y = y2;
    rdem = std::max(std::abs(x1 - x), std::abs(y1 - y));
    if (rdem < reps)
      break;
  }
  return iter;
}

double Family03::CalcEscapeMandelbrot(const cmplx &c) const {
  return CalcEscapeJulia(c);
}
