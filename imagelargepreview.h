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

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::ImageLargePreview *ui;
    std::string imagePath;

};

#endif // IMAGELARGEPREVIEW_H
