#include "renderthread.h"
#include <cmath>
#include <complex>


RenderThread::RenderThread(QObject *parent)
    :  QThread(parent)
{

}

RenderThread::~RenderThread()
{
    restart=true;
    abort = true;
    mutex.lock();
    condition.wakeOne();
    mutex.unlock();
    wait();
}


void RenderThread::render(const FractalParameters &params)
{
    QMutexLocker locker(&mutex);
    fractal_parameters_ = params;

    if (!isRunning()) {
        start();
    } else {
        restart = true;
        condition.wakeOne();
    }
}

void RenderThread::run()
{
    forever {
        if (abort) break;

        mutex.lock();
        FractalParameters local_params = fractal_parameters_;
        mutex.unlock();

        auto data = GenImage(local_params);

        if (!restart){
            emit renderedImage(data, local_params.area, local_params.image_size.width());
        }
        mutex.lock();
        if (!restart)
            condition.wait(&mutex);
        restart = false;
        mutex.unlock();
    }
}



QVector<float> RenderThread::GenImage(const FractalParameters &params)
{
    std::unique_ptr<Fractal> jset = SelectFractal(params);
    const dbltype dx = params.area.width()/params.image_size.width();
    const dbltype dy = params.area.height()/params.image_size.height();
    const dbltype x0 = params.area.x()+0.5*dx;
    const dbltype y0 = params.area.y()+0.5*dy;
    const size_t N = params.image_size.width()*params.image_size.height();
    QVector<float> data(N);
    std::function<float (const cmplx &z)> f = jset->GetCouloringFunction(params.cplane, params.couloring_mode);
    #pragma omp parallel for
    for (size_t i=0; i<N; ++i){
        if (restart){
            continue;
        }
        const int rows = i/params.image_size.width();
        const int cols = i%params.image_size.width();
        long double y = y0 + rows*dy;
        long double x = x0 + cols*dx;
        cmplx z(x,y);
        data[i] = f(z);
    }
    return data;
}


