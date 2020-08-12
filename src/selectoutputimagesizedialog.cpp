#include "selectoutputimagesizedialog.h"
#include "ui_selectoutputimagesizedialog.h"

SelectOutputImageSizeDialog::SelectOutputImageSizeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectOutputImageSizeDialog)
{
    ui->setupUi(this);
}

SelectOutputImageSizeDialog::~SelectOutputImageSizeDialog()
{
    delete ui;
}

QSize SelectOutputImageSizeDialog::GetImgSize() const
{
    return {ui->spinBoxW->value(), ui->spinBoxH->value()};
}

void  SelectOutputImageSizeDialog::SelectOutputImageSizeDialog::SetImgSize(const QSize &s)
{
    ui->spinBoxW->setValue(s.width());
    ui->spinBoxH->setValue(s.height());
}
