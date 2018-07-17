#ifndef CAMERA_H
#define CAMERA_H

#include <QWidget>
#include <QSystemTrayIcon>

#include "../SDSCommon/csocketdevice.h"
#include "../SDSCommon/cproxytcp.h"

#include "tcpsender.h"
class tcpSender;

QT_BEGIN_NAMESPACE
class QAction;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QPushButton;
class QSpinBox;
class QTextEdit;
QT_END_NAMESPACE

#include <QPixmap>
#include <QScreen>
#include <QTimer>
#include <QDesktopWidget>
#include <QWindow>
#include "opencv2/videoio.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "mat.hpp"

#define APP_TITLE "SDS Server Solution 1.0"

using namespace cv;

namespace Ui {
class Camera;
}

class Camera : public QWidget
{
    Q_OBJECT

public:
    explicit Camera(QWidget *parent = 0);
    ~Camera();
    void add( CSocketDevice *sk );
    tcpSender *tcpsender=NULL;

protected:
    void closeEvent(QCloseEvent *event) override;
    void writeVideo();
    void initVideo(int width, int height);
    bool isConnected();

    void moveEvent ( QMoveEvent *   ) override;

    int framesPerSec=30;
    int sizeBuffer=100;

    QString address="127.0.0.1"; // "0.0.0.0";
    int port=6767;
    QString senha="1234";
    bool isRemoto = true;
    bool isLocal = false;

    QScreen *screen=NULL;
    const QWindow *wd;
    QElapsedTimer et;
    QString getElapsed(bool reset=false);

    bool isPNG=true;
    bool isJPG=false;

signals:
    void disconnected ( );
    void sendFrame(const QImage &);

private slots:
    void showMessage();
    void messageClicked();
    void transmitir();
    void configurar();
    void showCV();
    void initRecord();
    void stopRecord();

private:
    Ui::Camera *ui;
    void createActions();
    void createTrayIcon();

    QAction *transmitirAction;
    QAction *configAction;
    QAction *restoreAction;
    QAction *quitAction;
    QAction *cvAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    int n_desktop=0;
    QPixmap originalPixmap;
    QImage qImageSingle;
    QTimer tm;
    QList<QScreen *>listscr;
    QVector<Mat> frames;
    QList<CSocketDevice *>sks;
    VideoWriter *video = NULL;

    CProxyTcp *proxy;
    QString m_path;
    QTimer tmRecover, tmEstable;

};

#endif // CAMERA_H
