#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QWindow>
#include <QPixmap>
#include <QScreen>
#include <QTimer>
#include <QDesktopWidget>
#include "opencv2/videoio.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include "mat.hpp"
using namespace cv;

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form();

private:
    Ui::Form *ui;

    int n_desktop=0;
    QPixmap originalPixmap;
    QImage qImageSingle;
    QTimer tm;
    QList<QScreen *>listscr;
    VideoWriter *video = NULL;
    QVector<Mat> frames;
    void  writeVideo();

public slots:
    void setDesktop(int p_desktop);
    void updateScreen();
    void initRecord();
    void initPlay();
    void startTimer();
};

#endif // FORM_H
