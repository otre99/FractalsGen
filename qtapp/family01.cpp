#include <QDebug>
#include <functional>
#include <limits>

#include "fractals.h"

void Family01::Init(const FractalParameters &p) {
  Fractal::Init(p);
  c_.real(p.c.real());
  c_.imag(p.c.imag());
}

double Family01::CalcFinalNormMandelbrot(const cmplx &c) const {
  // qDebug() << " Family01::CalcFinalNormMandelbrot() "<<  double(c.real()) <<
  // double(c.imag());
  int iter = 1;
  dbltype x, y, tmp1, tmp2;
  x = c_.real();
  y = c_.imag();
  dbltype dist = std::numeric_limits<double>::max();
  for (; iter < max_iter_; ++iter) {
    tmp1 = x;
    tmp2 = y;
    x *= x;
    y *= y;
    dist = std::min(orbit_metric_funct_(tmp1, tmp2), dist);
    if (x + y >= th_norm_) break;
    x += -y + c.real();
    y = 2 * tmp1 * tmp2 + c.imag();
  }
  return dist;
}

double Family01::CalcFinalNormJulia(const cmplx &z) const {
  // qDebug() << " Family01::CalcFinalNormJulia() "<<  double(z.real()) <<
  // double(z.imag());
  int iter = 0;
  dbltype x, y;
  dbltype tmp1, tmp2;
  x = z.real();
  y = z.imag();
  dbltype dist = std::numeric_limits<dbltype>::max();
  for (; iter < max_iter_; ++iter) {
    tmp1 = x;
    tmp2 = y;
    x *= x;
    y *= y;
    dist = std::min(orbit_metric_funct_(tmp1, tmp2), dist);
    if (x + y >= th_norm_) break;

    x += -y + c_.real();
    y = 2 * tmp1 * tmp2 + c_.imag();
  }
  return dist;
}

//////////////////////////////////////////
double Family01::CalcEscapeJulia(const cmplx &z) const {
  // qDebug() << " Family01::CalcEscapeJulia() "<<  double(z.real()) <<
  // double(z.imag());
  int iter = 0;
  dbltype x, y, tmp1, tmp2;

  x = z.real();
  y = z.imag();
  for (;;) {
    tmp1 = x;
    tmp2 = y;
    x *= x;
    y *= y;
    if (x + y >= th_norm_ || iter >= max_iter_) break;
    x += -y + c_.real();
    y = 2 * tmp1 * tmp2 + c_.imag();
    ++iter;
  }
  return iter;
}

double Family01::CalcEscapeMandelbrot(const cmplx &c) const {
  // qDebug() << " Family01::CalcEscapeMandelbrot() "<<  double(c.real()) <<
  // double(c.imag());
  int iter = 1;
  dbltype x, y;
  dbltype tmp1, tmp2;
  x = c_.real();
  y = c_.imag();
  // qDebug() << iter << double(th_norm_) << max_iter_;
  for (;;) {
    tmp1 = x;
    tmp2 = y;
    x *= x;
    y *= y;
    // qDebug() << "AA" << double(x+y) << iter ;
    if (x + y >= th_norm_ || iter >= max_iter_) break;
    x += -y + c.real();
    y = 2 * tmp1 * tmp2 + c.imag();
    ++iter;
  }
  // qDebug() << "BB" <<  iter ;
  return iter;
}
