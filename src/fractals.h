#ifndef FRACTALS_H
#define FRACTALS_H
#include <complex>
#include <QSize>
#include <QRectF>
#include <memory>
#include <functional>

using  dbltype = long double;
using  cmplx   = std::complex<dbltype>;

struct FractalParameters {
    int n;
    int max_iterations;
    double max_norm;
    QSize image_size;
    QRectF area;
    std::complex<double> c;
    std::complex<double> q;
    std::complex<double> orbit_pt;
    bool cplane;
    int couloring_mode;
    int orbit_mode;
    int fractal_family=0;
};

class Fractal {

protected:
    cmplx orbit_pt_;
    dbltype th_norm_;
    dbltype radius_;
    dbltype orbit_tangle_;
    int max_iter_;
    int orbit_mode_;
    std::function<dbltype(const dbltype &x, const dbltype &y)> orbit_metric_funct_;

    // x^2+y^2
    dbltype OrbitDiscance0(const dbltype &x, const dbltype &y) const {
        return (orbit_pt_.real()-x)*(orbit_pt_.real()-x)+(orbit_pt_.imag()-y)*(orbit_pt_.imag()-y);
    }
    // |x|
    dbltype OrbitDiscance1(const dbltype &x, const dbltype &y) const {
        return std::abs(orbit_pt_.real()-x);
    }
    // |y|
    dbltype OrbitDiscance2(const dbltype &x, const dbltype &y) const {
        return std::abs(orbit_pt_.imag()-y);
    }

    // |x*y|
    dbltype OrbitDiscance3(const dbltype &x, const dbltype &y) const {
        dbltype xx = orbit_pt_.real()-x;
        dbltype yy = orbit_pt_.imag()-y;
        return  std::abs(xx*yy);
    }

    // x^2+y^2-2*x*y
    dbltype OrbitDiscance4(const dbltype &x, const dbltype &y) const {
        dbltype xx = orbit_pt_.real()-x;
        dbltype yy = orbit_pt_.imag()-y;
        return  xx*xx+yy*yy - 2*xx*yy;
    }

    // x^2+y^2+2*x*y
    dbltype OrbitDiscance5(const dbltype &x, const dbltype &y) const {
        dbltype xx = orbit_pt_.real()-x;
        dbltype yy = orbit_pt_.imag()-y;
        return  xx*xx+yy*yy + 2*xx*yy;
    }

    // x^2+y^2+|2*x*y|
    dbltype OrbitDiscance6(const dbltype &x, const dbltype &y) const {
        dbltype xx = orbit_pt_.real()-x;
        dbltype yy = orbit_pt_.imag()-y;
        return  xx*xx+yy*yy + std::abs(2*xx*yy);
    }

    // x^2+y^2-|2*x*y|
    dbltype OrbitDiscance7(const dbltype &x, const dbltype &y) const {
        dbltype xx = orbit_pt_.real()-x;
        dbltype yy = orbit_pt_.imag()-y;
        return  xx*xx+yy*yy - std::abs(2*xx*yy);
    }

    // min(|x|,|y|)
    dbltype OrbitDiscance8(const dbltype &x, const dbltype &y) const {
        dbltype xx = std::abs(orbit_pt_.real()-x);
        dbltype yy = std::abs(orbit_pt_.imag()-y);
        return  std::min(xx, yy);
    }

    // max(|x|,|y|)
    dbltype OrbitDiscance9(const dbltype &x, const dbltype &y) const {
        dbltype xx = std::abs(orbit_pt_.real()-x);
        dbltype yy = std::abs(orbit_pt_.imag()-y);
        return  std::max(xx, yy);
    }

    //   |Line|
    dbltype OrbitDiscance10(const dbltype &x, const dbltype &y) const {
        return std::abs(y-orbit_tangle_*x);///orbit_dem_;
    }

    //   |yy-xx|
    dbltype OrbitDiscance11(const dbltype &x, const dbltype &y) const {
        dbltype xx = orbit_pt_.real()-x;
        dbltype yy = orbit_pt_.imag()-y;
        return std::abs(yy-xx);
    }

    //   |yy+xx|
    dbltype OrbitDiscance12(const dbltype &x, const dbltype &y) const {
        dbltype xx = orbit_pt_.real()-x;
        dbltype yy = orbit_pt_.imag()-y;
        return std::abs(yy+xx);
    }

    // |yy|+|xx|
    dbltype OrbitDiscance13(const dbltype &x, const dbltype &y) const {
        dbltype xx = orbit_pt_.real()-x;
        dbltype yy = orbit_pt_.imag()-y;
        return std::abs(yy)+std::abs(xx);
    }

    // |yy|-|xx|
    dbltype OrbitDiscance14(const dbltype &x, const dbltype &y) const {
        dbltype xx = orbit_pt_.real()-x;
        dbltype yy = orbit_pt_.imag()-y;
        return std::abs(std::abs(yy)-std::abs(xx));
    }
    std::function<dbltype (const dbltype &, const dbltype &)> GetOrbitMetric(const FractalParameters &f) const;

public:
    virtual void Init(const FractalParameters &p);
    virtual float CalcEscapeJulia(const cmplx &z) const = 0;
    virtual float CalcEscapeMandelbrot(const cmplx &c) const = 0;
    virtual float CalcFinalNormJulia(const cmplx &z) const = 0;
    virtual float CalcFinalNormMandelbrot(const cmplx &c) const = 0;
    std::function<float (const cmplx &z)> GetCouloringFunction(bool cplane, int color_mode);
    static dbltype kEps;
};

class Family00 : public Fractal {
    cmplx c_;
    dbltype q_;
public:
    void Init(const FractalParameters &p) override final;
    virtual float CalcEscapeJulia(const cmplx &z) const override final;
    virtual float CalcEscapeMandelbrot(const cmplx &c) const override final;
    virtual float CalcFinalNormJulia(const cmplx &z) const override final;
    virtual float CalcFinalNormMandelbrot(const cmplx &c) const override final;
};

class Family01 : public Fractal {
    cmplx c_;
public:
    void Init(const FractalParameters &p) override final;
    virtual float CalcEscapeJulia(const cmplx &z) const override final;
    virtual float CalcFinalNormJulia(const cmplx &z) const override final;
    virtual float CalcEscapeMandelbrot(const cmplx &c) const override final;
    virtual float CalcFinalNormMandelbrot(const cmplx &c) const override final;
};


class Family02 : public Fractal {
    cmplx c_;
    int n_;
    std::function<void(dbltype &, dbltype &)> funct_;

public:
    void Init(const FractalParameters &p) override final;
    virtual float CalcEscapeJulia(const cmplx &z) const override final;
    virtual float CalcFinalNormJulia(const cmplx &z) const override final;
    virtual float CalcEscapeMandelbrot(const cmplx &c) const override final;
    virtual float CalcFinalNormMandelbrot(const cmplx &c) const override final;

    static void FastPow1Inline(dbltype &, dbltype &) {};
    static void FastPow2Inline(dbltype &x, dbltype &y);
    static void FastPow3Inline(dbltype &x, dbltype &y);
    static void FastPow4Inline(dbltype &x, dbltype &y);
    static void FastPow5Inline(dbltype &x, dbltype &y);
    static void FastPow6Inline(dbltype &x, dbltype &y);
    static void FastPowNInline(dbltype &x, dbltype &y, const int n);
};

class Family03 : public Fractal {
    cmplx c_;
    int n_;
    std::function<void(dbltype &, dbltype &)> funct_;
    dbltype alpha_;
public:
    void Init(const FractalParameters &p) override final;
    virtual float CalcEscapeJulia(const cmplx &z) const override final;
    virtual float CalcFinalNormJulia(const cmplx &z) const override final;
    virtual float CalcEscapeMandelbrot(const cmplx &c) const override final;
    virtual float CalcFinalNormMandelbrot(const cmplx &c) const override final;
};

class Family04 : public Fractal {
    cmplx c_, q_;
    int n_;
    std::function<void(dbltype &, dbltype &)> funct_;
    dbltype alpha_;
public:
    void Init(const FractalParameters &p) override final;
    virtual float CalcEscapeJulia(const cmplx &z) const override final;
    virtual float CalcFinalNormJulia(const cmplx &z) const override final;
    virtual float CalcEscapeMandelbrot(const cmplx &c) const override final;
    virtual float CalcFinalNormMandelbrot(const cmplx &c) const override final;
};

std::unique_ptr<Fractal> SelectFractal(const FractalParameters &p);

#endif // FRACTALS_H
