#include "fractals.h"

dbltype Fractal::kEps = 1e-7;

std::unique_ptr<Fractal> SelectFractal(const FractalParameters &p){
    std::unique_ptr<Fractal> ptr;
    switch (p.fractal_family) {
    case 0: ptr.reset(new Family00); break;
    case 1: ptr.reset(new Family01); break;
    case 2: ptr.reset(new Family02); break;
    case 3: ptr.reset(new Family03); break;
    case 4: ptr.reset(new Family04); break;
    default: ptr.reset(new Family00); break;
    }
    ptr->Init(p);
    return ptr;
}

std::function<float (const cmplx &z)> Fractal::GetCouloringFunction(bool cplane, int color_mode) {
    switch (color_mode) {
    case 0:
    case 1:
        return cplane ? std::bind(&Fractal::CalcEscapeMandelbrot, this, std::placeholders::_1) : std::bind(&Fractal::CalcEscapeJulia, this, std::placeholders::_1);
    case 2:
        return cplane ? std::bind(&Fractal::CalcFinalNormMandelbrot, this, std::placeholders::_1) : std::bind(&Fractal::CalcFinalNormJulia, this, std::placeholders::_1);
        break;
    default:
        return {};
    }
};

void Fractal::Init(const FractalParameters &p){
    orbit_pt_.real(p.orbit_pt.real());
    orbit_pt_.imag(p.orbit_pt.imag());
    radius_ = p.max_norm;
    th_norm_ = radius_*radius_;
    max_iter_ = p.max_iterations;
    orbit_tangle_ = std::tan(std::arg(orbit_pt_));
    orbit_metric_funct_ = GetOrbitMetric(p);
}

std::function<dbltype (const dbltype &, const dbltype &)> Fractal::GetOrbitMetric(const FractalParameters &f) const
{
    switch (f.orbit_mode) {
    case 0: return std::bind(&Fractal::OrbitDiscance0, this, std::placeholders::_1, std::placeholders::_2);
    case 1: return std::bind(&Fractal::OrbitDiscance1, this, std::placeholders::_1, std::placeholders::_2);
    case 2: return std::bind(&Fractal::OrbitDiscance2, this, std::placeholders::_1, std::placeholders::_2);
    case 3: return std::bind(&Fractal::OrbitDiscance3, this, std::placeholders::_1, std::placeholders::_2);
    case 4: return std::bind(&Fractal::OrbitDiscance4, this, std::placeholders::_1, std::placeholders::_2);
    case 5: return std::bind(&Fractal::OrbitDiscance5, this, std::placeholders::_1, std::placeholders::_2);
    case 6: return std::bind(&Fractal::OrbitDiscance6, this, std::placeholders::_1, std::placeholders::_2);
    case 7: return std::bind(&Fractal::OrbitDiscance7, this, std::placeholders::_1, std::placeholders::_2);
    case 8: return std::bind(&Fractal::OrbitDiscance8, this, std::placeholders::_1, std::placeholders::_2);
    case 9: return std::bind(&Fractal::OrbitDiscance9, this, std::placeholders::_1, std::placeholders::_2);
    case 10: return std::bind(&Fractal::OrbitDiscance10, this, std::placeholders::_1, std::placeholders::_2);
    case 11: return std::bind(&Fractal::OrbitDiscance11, this, std::placeholders::_1, std::placeholders::_2);
    case 12: return std::bind(&Fractal::OrbitDiscance12, this, std::placeholders::_1, std::placeholders::_2);
    case 13: return std::bind(&Fractal::OrbitDiscance13, this, std::placeholders::_1, std::placeholders::_2);
    case 14: return std::bind(&Fractal::OrbitDiscance14, this, std::placeholders::_1, std::placeholders::_2);
    default: return std::bind(&Fractal::OrbitDiscance0, this, std::placeholders::_1, std::placeholders::_2);
    }
}
