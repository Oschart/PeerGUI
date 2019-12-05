#ifndef VISITPROFILE_H
#define VISITPROFILE_H

#include <QWidget>

namespace Ui {
class visitProfile;
}

class visitProfile : public QWidget
{
    Q_OBJECT

public:
    explicit visitProfile(QWidget *parent = nullptr);
    ~visitProfile();

private:
    Ui::visitProfile *ui;
};

#endif // VISITPROFILE_H
