#ifndef GRANTEDIMAGEPREVIEW_H
#define GRANTEDIMAGEPREVIEW_H

#include <QDialog>

namespace Ui {
class GrantedImagePreview;
}

class GrantedImagePreview : public QDialog
{
    Q_OBJECT

public:
    explicit GrantedImagePreview(std::string, QWidget *parent = nullptr);
    ~GrantedImagePreview();

private:
    Ui::GrantedImagePreview *ui;
    std::string path;
};

#endif // GRANTEDIMAGEPREVIEW_H
