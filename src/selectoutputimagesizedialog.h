#ifndef SELECTOUTPUTIMAGESIZEDIALOG_H
#define SELECTOUTPUTIMAGESIZEDIALOG_H

#include <QDialog>

namespace Ui {
class SelectOutputImageSizeDialog;
}

class SelectOutputImageSizeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectOutputImageSizeDialog(QWidget *parent = nullptr);
    ~SelectOutputImageSizeDialog();
    QSize GetImgSize() const;
    void  SetImgSize(const QSize &s);
private:
    Ui::SelectOutputImageSizeDialog *ui;
};

#endif // SELECTOUTPUTIMAGESIZEDIALOG_H
