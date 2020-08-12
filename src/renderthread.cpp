/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "renderthread.h"

#include <QImage>
#include <cmath>
#include <complex>
#include <QDebug>
#include <iostream>
#include "juliatset.h"
#include <functional>

RenderThread::RenderThread(QObject *parent)
    :  QThread(parent)
{

}

RenderThread::~RenderThread()
{
    mutex.lock();
    abort = true;
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
    JuliaSet jset;
    jset.Init(params.c, params.q.real(), params.max_norm, params.max_iterations, params.n);



    jset.SetOrbitPt(params.orbit_pt, params.orbit_mode);


    const double dx = params.area.width()/params.image_size.width();
    const double dy = params.area.height()/params.image_size.height();
    const double x0 = params.area.x()+0.5*dx;
    const double y0 = params.area.y()+0.5*dy;
    const size_t N = params.image_size.width()*params.image_size.height();
    QVector<float> data(N);


    std::function<float (cmplx z)> f;
    switch (params.couloring_model) {
    case 0:
        f = params.cplane ? std::bind(&JuliaSet::CalcEscapeMandelbrot, &jset, std::placeholders::_1) : std::bind(&JuliaSet::CalcEscapeJulia, &jset, std::placeholders::_1);
        break;
    case 1:
        f = params.cplane ? std::bind(&JuliaSet::CalcEscapeMandelbrotSmoth, &jset, std::placeholders::_1) : std::bind(&JuliaSet::CalcEscapeJuliaSmoth, &jset, std::placeholders::_1);
        break;
    case 2:
        f = params.cplane ? std::bind(&JuliaSet::CalcFinalNormMandelbrot, &jset, std::placeholders::_1) : std::bind(&JuliaSet::CalcFinalNormJulia, &jset, std::placeholders::_1);
        break;
    default:
        qDebug() << "Unknow couloring mode";
        return data;
    }


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


