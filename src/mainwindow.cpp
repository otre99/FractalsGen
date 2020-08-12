#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include "selectoutputimagesizedialog.h"
#include "juliatset.h"
#include <QtConcurrent/QtConcurrent>
#include <QFuture>



QMap<QString, QCPColorGradient::GradientPreset> name2gp;

struct FullImageRender {
    bool abort=false;
    QCPColorGradient cb;
    QCPRange r;
    QImage *img;
    bool log_cmap;
    FractalParameters *params;

    void Render(){
        JuliaSet jset;
        jset.Init(params->c, params->q.real(), params->max_norm, params->max_iterations, params->n);
        jset.SetOrbitPt(params->orbit_pt, params->orbit_mode);

        const long double dx = params->area.width()/img->width();
        const long double dy = params->area.height()/img->height();
        const long double x0 = params->area.x()+0.5*dx;
        const long double y0 = params->area.y()+0.5*dy;
        const size_t N =  img->width()*img->height();
        auto data = reinterpret_cast<QRgb*>(img->bits());


        std::function<float (cmplx z)> f;
        switch (params->couloring_model) {
        case 0:
            f = params->cplane ? std::bind(&JuliaSet::CalcEscapeMandelbrot, &jset, std::placeholders::_1) : std::bind(&JuliaSet::CalcEscapeJulia, &jset, std::placeholders::_1);
            break;
        case 1:
            f = params->cplane ? std::bind(&JuliaSet::CalcEscapeMandelbrotSmoth, &jset, std::placeholders::_1) : std::bind(&JuliaSet::CalcEscapeJuliaSmoth, &jset, std::placeholders::_1);
            break;
        case 2:
            f = params->cplane ? std::bind(&JuliaSet::CalcFinalNormMandelbrot, &jset, std::placeholders::_1) : std::bind(&JuliaSet::CalcFinalNormJulia, &jset, std::placeholders::_1);
            break;
        default:
            qDebug() << "Unknow couloring mode";
            return;
        }


#pragma omp parallel for
        for (size_t i=0; i<N; ++i){

            if (abort){
                continue;
            }
            const int rows = i/img->width();
            const int cols = i%img->width();
            long double y = y0 + rows*dy;
            long double x = x0 + cols*dx;

            cmplx z(x,y);
            const float niters = f(z);
            data[i] = cb.color(niters,r, log_cmap);
        }
    }
};


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setCentralWidget(m_custom_plot = new QCustomPlot);
    SetupColorMapPlot(m_custom_plot);

    name2gp["Grayscale"] = QCPColorGradient::gpGrayscale;
    name2gp["Hot"] = QCPColorGradient::gpHot;
    name2gp["Cold"] = QCPColorGradient::gpCold;
    name2gp["Night"] = QCPColorGradient::gpNight;
    name2gp["Candy"] = QCPColorGradient::gpCandy;
    name2gp["Geography"] = QCPColorGradient::gpGeography;
    name2gp["Ion"] = QCPColorGradient::gpIon;
    name2gp["Thermal"] = QCPColorGradient::gpThermal;
    name2gp["Polar"] = QCPColorGradient::gpPolar;
    name2gp["Spectrum"] = QCPColorGradient::gpSpectrum;
    name2gp["Jet"] = QCPColorGradient::gpJet;
    name2gp["Hues"] = QCPColorGradient::gpHues;


    fparams_.couloring_model=ui->comboBoxCouloring->currentIndex();
    fparams_.max_iterations = ui->spinBoxMaxIters->value();
    fparams_.n = ui->spinBoxN->value();
    fparams_.max_norm = ui->doubleSpinBoxMaxNorm->value();
    fparams_.cplane   = ui->checkBoxMandelbrot->isChecked();

    on_lineEditCx_textEdited(ui->lineEditCx->text());
    on_lineEditCy_textEdited(ui->lineEditCy->text());
    on_lineEditQx_textEdited(ui->lineEditQx->text());
    on_comboBoxCMaps_activated(ui->comboBoxCMaps->currentText());


    QObject::connect(m_custom_plot, &QCustomPlot::afterReplot, this, &MainWindow::Render);
    QObject::connect(&rthread_, &RenderThread::renderedImage, this, &MainWindow::RenderedData);
    update_from_saved_data_ = false;

    ui->horizontalSliderHi->SetRange(0,1000);
    ui->horizontalSliderHi->SetValueDouble(1000);
    ui->horizontalSliderLo->SetRange(0,1000);
}

void MainWindow::Render()
{
    auto xR = m_custom_plot->xAxis->range();
    auto yR = m_custom_plot->yAxis->range();

    if (xR != last_XR_ || yR != last_YR_) {
        fparams_.area = QRectF(QPointF{xR.lower,yR.lower}, QPointF{xR.upper, yR.upper});
        fparams_.image_size = GetRenderImageSize();
        RenderCommand(9);
        last_XR_ = xR;
        last_YR_ = yR;
    }
}

QSize MainWindow::GetRenderImageSize()
{
    switch (ui->comboBoxRanderModel->currentIndex()) {
    case 0:
        return m_custom_plot->axisRect(0)->size();
        break;
    case 1:
        return {300,300};
        break;
    case 2:
        return {100,100};
        break;
    default:
        return m_custom_plot->axisRect(0)->size();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::RenderCommand(int tt)
{
    elapsed_timer_.restart();
    statusBar()->showMessage("Rendering...");
    rthread_.render(fparams_);
}

void MainWindow::on_spinBoxMaxIters_valueChanged(int arg1)
{
    fparams_.max_iterations = arg1;
    ui->horizontalSliderHi->SetMaximunDouble(arg1);
    ui->horizontalSliderLo->SetMaximunDouble(arg1);
    RenderCommand(3);
}

void MainWindow::on_doubleSpinBoxMaxNorm_valueChanged(double arg1)
{
    fparams_.max_norm = arg1;
    RenderCommand(4);
}

void MainWindow::on_spinBoxN_valueChanged(int arg1)
{
    fparams_.n = arg1;
}

void MainWindow::on_lineEditCx_textEdited(const QString &arg1)
{
    double xx = arg1.toDouble();
    fparams_.c.real(xx);
    RenderCommand(5);
}


void MainWindow::on_lineEditCy_textEdited(const QString &arg1)
{
    double yy = arg1.toDouble();
    fparams_.c.imag(yy);
    RenderCommand(5);
}

void MainWindow::SetupColorMapPlot(QCustomPlot *customPlot)
{
    customPlot->setInteractions(
                QCP::iRangeDrag | QCP::iRangeZoom); // this will also allow rescaling the
    // color scale by dragging/zooming
    customPlot->axisRect()->setupFullAxesBox(true);
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");

    m_color_map = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
    m_color_map->data()->setRange(QCPRange(-2, 2), QCPRange(-2, 2));

    auto *colorScale = new QCPColorScale(customPlot);
    customPlot->plotLayout()->addElement(
                0, 1, colorScale); // add it to the right of the main axis rect
    colorScale->setType(
                QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels
    // right (actually atRight is already the default)
    m_color_map->setColorScale(
                colorScale); // associate the color map with the color scale

    m_color_map->setGradient(QCPColorGradient::gpGrayscale);
    auto *marginGroup = new QCPMarginGroup(customPlot);
    customPlot->axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop,
                                           marginGroup);
    colorScale->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);
    m_color_map->setDataRange(QCPRange(1, 500));
    m_custom_plot->rescaleAxes();
}

void MainWindow::FillColormap(const QVector<float> &dd, const QRectF &area, int cols) {
    m_color_map->data()->setSize(cols, dd.size()/cols);
    m_color_map->data()->setRange(QCPRange(area.left(), area.right()), QCPRange(area.bottom(), area.top()));

    auto cd = m_color_map->data();
    for (size_t i=0; i<dd.size(); ++i){
        cd->setCell(i%cols,i/cols,dd[i]);
    }
    if (update_from_saved_data_){

        m_custom_plot->rescaleAxes();
        last_XR_ = m_custom_plot->xAxis->range();
        last_YR_ = m_custom_plot->yAxis->range();

        on_comboBoxCMaps_activated(ui->comboBoxCMaps->currentText());
        update_from_saved_data_ = false;
    }
    if (ui->checkBoxAutoRes->isChecked()){
        on_pushButtonRescale_clicked();
    } else {
        m_custom_plot->replot();
    }
}

void MainWindow::RenderedData(const QVector<float> &data, const QRectF &ss, int cols)
{
    double zf = qRound((4./ss.width())*10)/10.0;
    statusBar()->showMessage( QString("Render time -> %1 ms | Zoom in -> x%2").arg(elapsed_timer_.elapsed()).arg(zf));
    FillColormap(data,ss, cols);
}

void MainWindow::on_pushButtonRescale_clicked()
{
    m_color_map->rescaleDataRange(true);
    m_custom_plot->replot();
    const auto r = m_color_map->dataRange();
    ui->horizontalSliderLo->SetRange(r.lower, r.upper);
    ui->horizontalSliderHi->SetRange(r.lower, r.upper);

}


void MainWindow::on_lineEditQx_textEdited(const QString &arg1)
{
    double xx = arg1.toDouble();
    fparams_.q.real(xx);
    RenderCommand(6);
}



void MainWindow::on_checkBoxLog_clicked()
{
    on_comboBoxCMaps_activated(ui->comboBoxCMaps->currentText());
}

void MainWindow::on_checkBoxPeriod_clicked()
{
    on_comboBoxCMaps_activated(ui->comboBoxCMaps->currentText());
}



void MainWindow::on_pushButtonSaveParams_clicked()
{
    QString path = QFileDialog::getSaveFileName(this);
    if (path.isEmpty()) return;
    SaveRelevantParameters(path);

}

void MainWindow::on_comboBoxCMaps_activated(const QString &arg1)
{
    QCPColorGradient grad(name2gp[arg1]);
    if (ui->checkBoxInverted->isChecked()){
        grad = grad.inverted();
    }
    grad.setPeriodic(ui->checkBoxPeriod->isChecked());
    m_color_map->setGradient(grad);
    m_color_map->colorScale()->setDataScaleType( ui->checkBoxLog->isChecked() ? QCPAxis::stLogarithmic : QCPAxis::stLinear);
    m_custom_plot->replot();
}

void MainWindow::on_comboBoxRanderModel_activated(int index)
{
    fparams_.image_size = GetRenderImageSize();
    if (index==0){
        RenderCommand(1);
    }
}

void MainWindow::on_pushButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this);
    if (path.isEmpty()) return;
    update_from_saved_data_ = true;
    LoadRelevantParameters(path);
    RenderCommand(0);
}

void MainWindow::SaveRelevantParameters(QString &path)
{
    QFile file(path);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);   // we will serialize the data into the file
    out << fparams_.n;
    out << fparams_.max_iterations;
    out << fparams_.max_norm;
    out << fparams_.image_size;
    out << fparams_.area;
    out << fparams_.c.real() << fparams_.c.imag();
    out << fparams_.q.real() << fparams_.q.imag();
    out << fparams_.cplane;

    // colors
    out << ui->comboBoxCMaps->currentIndex() << ui->checkBoxLog->isChecked() << ui->checkBoxPeriod->isChecked();
    out << m_color_map->dataRange().lower << m_color_map->dataRange().upper;
    out << fparams_.couloring_model;

    out << fparams_.orbit_pt.real( ) << fparams_.orbit_pt.imag() << fparams_.orbit_mode;
}

void MainWindow::LoadRelevantParameters(QString &path)
{

    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);   // we will serialize the data into the file
    double cx,cy,qx,qy;
    in >> fparams_.n;
    in >> fparams_.max_iterations;
    in >> fparams_.max_norm;
    in >> fparams_.image_size;
    in >> fparams_.area;
    in >> cx >> cy >> qx >> qy >> fparams_.cplane;



    ui->checkBoxMandelbrot->setChecked(fparams_.cplane);
    fparams_.c.real(cx); ui->lineEditCx->setText(QString::number(cx));
    fparams_.c.imag(cy); ui->lineEditCy->setText(QString::number(cy));
    fparams_.q.real(qx); ui->lineEditQx->setText(QString::number(qx));



    int index; bool log_cmp, period_cmap;
    double minV, maxV;
    in >> index >> log_cmp >> period_cmap;
    in >> minV >> maxV;

    ui->comboBoxCMaps->setCurrentIndex(index);
    ui->checkBoxLog->setChecked(log_cmp);
    ui->checkBoxPeriod->setChecked(period_cmap);

    ui->horizontalSliderHi->SetMaximunDouble(ui->spinBoxMaxIters->value());
    ui->horizontalSliderLo->SetValueDouble(minV);
    ui->horizontalSliderHi->SetValueDouble(maxV);
    m_color_map->setDataRange(QCPRange(minV, maxV));

    in >> fparams_.couloring_model;
    ui->comboBoxCouloring->setCurrentIndex(fparams_.couloring_model);

    in >> cx >> cy;
    fparams_.orbit_pt.real(cx); ui->lineEditOrbitX->setText(QString::number(cx));
    fparams_.orbit_pt.imag(cy); ui->lineEditOrbitY->setText(QString::number(cy));

    in >> fparams_.orbit_mode;
    ui->comboBoxOrbitType->setCurrentIndex(fparams_.orbit_mode);
}

void MainWindow::on_horizontalSliderHi_doubleValueMoved(double val)
{
    const double lo = m_color_map->dataRange().lower;
    m_color_map->setDataRange(QCPRange(lo, std::max(static_cast<double>(val), lo)));
    m_custom_plot->replot();
}

void MainWindow::on_horizontalSliderLo_doubleValueMoved(double val)
{
    double hi = m_color_map->dataRange().upper;
    m_color_map->setDataRange(QCPRange(std::min(static_cast<double>(val), hi), hi));
    m_custom_plot->replot();
}

void MainWindow::on_pushButtonPNG_clicked()
{
    SelectOutputImageSizeDialog dlg(this);
    if (dlg.exec() == QDialog::Rejected){
        return;
    }

    const QString path = QFileDialog::getSaveFileName(this);
    if (path.isEmpty()) return;

    auto cb = m_color_map->colorScale()->gradient();
    auto r = m_color_map->dataRange();
    bool log_cmap = ui->checkBoxLog->isChecked();

    QImage image(dlg.GetImgSize(),QImage::Format_RGB32);
    QProgressDialog progress("Rendering...", "Cancel", 0, 0, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    FullImageRender img_render;
    img_render.r = r;
    img_render.cb = cb;
    img_render.img = &image;
    img_render.params = &fparams_;
    img_render.log_cmap = log_cmap;
    QFuture<void> f = QtConcurrent::run(&img_render, &FullImageRender::Render);
    while (f.isRunning()) {

        if (progress.wasCanceled()){
            img_render.abort = true;
        }
        qApp->processEvents();
        QThread::msleep(10);
    }
    if (img_render.abort){
        return;
    }
    image.save(path, "png");
}

void MainWindow::on_checkBoxMandelbrot_clicked()
{
    fparams_.cplane = ui->checkBoxMandelbrot->isChecked();
    RenderCommand(2);
}

void MainWindow::on_checkBoxInverted_clicked()
{
    on_comboBoxCMaps_activated(ui->comboBoxCMaps->currentText());
}

void MainWindow::on_comboBoxCouloring_activated(int index)
{
    fparams_.couloring_model = index;
    RenderCommand(0);
}

void MainWindow::on_lineEditOrbitX_textEdited(const QString &arg1)
{
    double xx = arg1.toDouble();
    fparams_.orbit_pt.real(xx);
    RenderCommand(6);
}

void MainWindow::on_lineEditOrbitY_textEdited(const QString &arg1)
{
    double xx = arg1.toDouble();
    fparams_.orbit_pt.imag(xx);
    RenderCommand(6);
}

void MainWindow::on_comboBoxOrbitType_activated(int index)
{
    fparams_.orbit_mode = index;
    RenderCommand(0);
}
