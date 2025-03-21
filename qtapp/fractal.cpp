#include <QDebug>

#include "fractals.h"

dbltype Fractal::kEps = 1e-7;

std::unique_ptr<Fractal> Fractal::Create(const FractalParameters *params) {
  std::unique_ptr<Fractal> result;
  switch (params->fractal_family) {
    case 0:
      result.reset(new Family00);
      break;
    case 1:
      result.reset(new Family01);
      break;
    case 2:
      result.reset(new Family02);
      break;
    case 3:
      result.reset(new Family03);
      break;
    case 4:
      result.reset(new Family04);
      break;
    default:
      qErrnoWarning(
          "Wrong fractal family number. Expected [0,1,2,3,4], but get %d",
          params->fractal_family);
      break;
  }
  result->Init(*params);
  return result;
}

std::function<double(const cmplx &z)> Fractal::GetCouloringFunction(
    bool mandelbrot, int orbit_trap) {
  switch (orbit_trap) {
    case 0:
      return mandelbrot ? std::bind(&Fractal::CalcEscapeMandelbrot, this,
                                    std::placeholders::_1)
                        : std::bind(&Fractal::CalcEscapeJulia, this,
                                    std::placeholders::_1);
    case 1:
      return mandelbrot ? std::bind(&Fractal::CalcFinalNormMandelbrot, this,
                                    std::placeholders::_1)
                        : std::bind(&Fractal::CalcFinalNormJulia, this,
                                    std::placeholders::_1);
      break;
    default:
      qDebug() << "Error: Returing empty Couloring function.";
      qDebug() << "orbit_trap: " << orbit_trap << "mandelbrot: " << mandelbrot;
      return {};
  }
};

void Fractal::Init(const FractalParameters &p) {
  orbit_pt_.real(p.orbit_pt.real());
  orbit_pt_.imag(p.orbit_pt.imag());
  radius_ = p.max_norm;
  th_norm_ = radius_ * radius_;
  max_iter_ = p.max_iterations;
  orbit_tangle_ = std::tan(std::arg(orbit_pt_));
  orbit_metric_funct_ = GetOrbitMetric(p);
}

std::function<dbltype(const dbltype &, const dbltype &)>
Fractal::GetOrbitMetric(const FractalParameters &f) const {
  switch (f.orbit_mode) {
    case 0:
      return std::bind(&Fractal::OrbitDiscance0, this, std::placeholders::_1,
                       std::placeholders::_2);
    case 1:
      return std::bind(&Fractal::OrbitDiscance1, this, std::placeholders::_1,
                       std::placeholders::_2);
    case 2:
      return std::bind(&Fractal::OrbitDiscance2, this, std::placeholders::_1,
                       std::placeholders::_2);
    case 3:
      return std::bind(&Fractal::OrbitDiscance3, this, std::placeholders::_1,
                       std::placeholders::_2);
    case 4:
      return std::bind(&Fractal::OrbitDiscance4, this, std::placeholders::_1,
                       std::placeholders::_2);
    case 5:
      return std::bind(&Fractal::OrbitDiscance5, this, std::placeholders::_1,
                       std::placeholders::_2);
    case 6:
      return std::bind(&Fractal::OrbitDiscance6, this, std::placeholders::_1,
                       std::placeholders::_2);
    case 7:
      return std::bind(&Fractal::OrbitDiscance7, this, std::placeholders::_1,
                       std::placeholders::_2);
    case 8:
      return std::bind(&Fractal::OrbitDiscance8, this, std::placeholders::_1,
                       std::placeholders::_2);
    case 9:
      return std::bind(&Fractal::OrbitDiscance9, this, std::placeholders::_1,
                       std::placeholders::_2);
    case 10:
      return std::bind(&Fractal::OrbitDiscance10, this, std::placeholders::_1,
                       std::placeholders::_2);
    case 11:
      return std::bind(&Fractal::OrbitDiscance11, this, std::placeholders::_1,
                       std::placeholders::_2);
    case 12:
      return std::bind(&Fractal::OrbitDiscance12, this, std::placeholders::_1,
                       std::placeholders::_2);
    case 13:
      return std::bind(&Fractal::OrbitDiscance13, this, std::placeholders::_1,
                       std::placeholders::_2);
    case 14:
      return std::bind(&Fractal::OrbitDiscance14, this, std::placeholders::_1,
                       std::placeholders::_2);
    default:
      return std::bind(&Fractal::OrbitDiscance0, this, std::placeholders::_1,
                       std::placeholders::_2);
  }
}
