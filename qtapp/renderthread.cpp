#include <execution> //NOTE(otre99): There seems to be a problem when this header is not the first one
#include "renderthread.h"

#include <QDebug>
#include <algorithm>
#include <cmath>
#include <complex>
#include <numeric>
#include <vector>

RenderThread::RenderThread(QObject *parent) : QThread(parent) {}

RenderThread::~RenderThread() {
  restart = true;
  abort = true;
  mutex.lock();
  condition.wakeOne();
  mutex.unlock();
  wait();
}

void RenderThread::render(const FractalParameters &params) {
  QMutexLocker locker(&mutex);
  fractal_parameters_ = params;

  if (!isRunning()) {
    start();
  } else {
    restart = true;
    condition.wakeOne();
  }
}

void RenderThread::run() {
  forever {
    if (abort) break;

    mutex.lock();
    FractalParameters local_params = fractal_parameters_;
    mutex.unlock();

    std::function<float(const cmplx &z)> f;
    switch (local_params.fractal_family) {
      case 0:
        family00.Init(local_params);
        f = family00.GetCouloringFunction(local_params.mandelbrot,
                                          local_params.orbit_trap);
        break;
      case 1:
        family01.Init(local_params);
        f = family01.GetCouloringFunction(local_params.mandelbrot,
                                          local_params.orbit_trap);
        break;
      case 2:
        family02.Init(local_params);
        f = family02.GetCouloringFunction(local_params.mandelbrot,
                                          local_params.orbit_trap);
        break;
      case 3:
        family03.Init(local_params);
        f = family03.GetCouloringFunction(local_params.mandelbrot,
                                          local_params.orbit_trap);
        break;
      case 4:
        family04.Init(local_params);
        f = family04.GetCouloringFunction(local_params.mandelbrot,
                                          local_params.orbit_trap);
        break;
      default:
        qErrnoWarning(
            "Wrong fractal family number. Expected [0,1,2,3,4], but get %d",
            local_params.fractal_family);
        continue;
        break;
    }

    //        qDebug() << "Start rendering ...";
    //        qDebug() << "Family     = " << local_params.fractal_family;
    //        qDebug() << "Q          = " << local_params.q.real() <<
    //        local_params.q.imag(); qDebug() << "C          = " <<
    //        local_params.c.real() << local_params.c.imag(); qDebug() << "n = "
    //        << local_params.n; qDebug() << "Orbit      = " <<
    //        local_params.orbit_pt.real() << local_params.orbit_pt.imag();
    //        qDebug() << "Orbit Trap = " << local_params.orbit_trap;
    //        qDebug() << "Mandelbrot = " << local_params.mandelbrot;
    //        qDebug() << "Max. Norm  = " << local_params.max_norm;
    //        qDebug() << "Max. Iters = " << local_params.max_iterations;
    //        qDebug() << "Orbit mode = " << local_params.orbit_mode;

    QVector<double> data;
    const size_t N =
        local_params.image_size.width() * local_params.image_size.height();
    const dbltype centerX = local_params.centerX;
    const dbltype centerY = local_params.centerY;
    const dbltype scaleFactor = local_params.scale;
    const int H = local_params.image_size.height();
    const int W = local_params.image_size.width();
    data.resize(N);
    if (indexs.size() != H) {
      indexs.resize(H);
      std::iota(indexs.begin(), indexs.end(), 0);
    }

    std::for_each(std::execution::par_unseq, indexs.begin(), indexs.end(),
                  [&](int i) {
                    if (this->restart) return;
                    double *d = &data[i * W];
                    dbltype yy = centerY + (i - H / 2) * scaleFactor;
                    for (int k = -W / 2; k < W / 2; ++k) {
                      d[k + W / 2] = f({centerX + k * scaleFactor, yy});
                    }
                  });

    if (!restart) {
      emit renderedImage(data, local_params.image_size, local_params.scale);
    }
    mutex.lock();
    if (!restart) condition.wait(&mutex);
    restart = false;
    mutex.unlock();
  }
}
