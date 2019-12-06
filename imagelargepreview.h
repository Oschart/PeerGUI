#ifndef IMAGELARGEPREVIEW_H
#define IMAGELARGEPREVIEW_H

#include <QDialog>

namespace Ui {
class ImageLargePreview;
}

class ImageLargePreview : public QDialog
{
    Q_OBJECT

public:
    explicit ImageLargePreview(std::string path, QWidget *parent = nullptr);
    ~ImageLargePreview();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ImageLargePreview *ui;
};

#endif // IMAGELARGEPREVIEW_H
