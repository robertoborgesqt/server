#include "camera.h"
#include "config.h"
#include "ui_camera.h"

#include <QBuffer>
#include <qevent.h>
#include <qmenu.h>
#include <qmessagebox.h>

Camera::Camera(QWidget *parent) : QWidget(parent), ui(new Ui::Camera)
{
    ui->setupUi(this);

    // To do: Ler dados do banco local

    listscr =  QGuiApplication::screens();

    createActions();
    createTrayIcon();

    connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &Camera::messageClicked);

    trayIcon->setIcon(QIcon(":/imagens/camera.png"));
    setWindowIcon(QIcon(":/imagens/camera.png"));

    trayIcon->show();

    if ( isRemoto ) // Verifica se o proxy aceita conexões..
    {
        proxy = new CProxyTcp( this, this->address, port );
        for( ;; ) {
            if ( !proxy || !proxy->read() )
            {
                QMessageBox msgBox;
                msgBox.setText(tr("Endereço/porta rede indisponível " )+ address);
                msgBox.setInformativeText("Do you want to save your changes?");
                msgBox.setStandardButtons(QMessageBox::Abort | QMessageBox::Ok | QMessageBox::Retry);
                msgBox.setDefaultButton(QMessageBox::Save);
                int ret = msgBox.exec();

                if ( ret == QMessageBox::Retry )
                    continue;
                else
                    if ( ret == QMessageBox::Abort )
                        exit(0);
            }
            else
                break;
        }
    }

    setWindowTitle(tr(APP_TITLE));
    setStyleSheet("background:transparent;");
    setAttribute(Qt::WA_TranslucentBackground);

    QPoint pos = this->mapToGlobal(this->pos());

    ui->lblX->setText(QString("%1").arg( pos.x()));
    ui->lblY->setText(QString("%1").arg( pos.y()));

    //this->connect(this, SIGNAL())

   // setWindowFlags( Qt::WindowStaysOnTopHint);
   // setWindowFlags(Qt::FramelessWindowHint);
}

Camera::~Camera()
{
    delete ui;
}

void Camera::configurar()
{
    Config config;
    config.setFrames( framesPerSec );
    config.setBuffer( sizeBuffer   );
    config.setAddr  ( address  );
    config.setPort  ( port     );
    config.setSenha ( senha    );
    config.setLocal ( isLocal  );
    config.setRemote( isRemoto );
    config.setPNG   ( isPNG    );
    config.setJPG   ( isJPG    );

    config.exec();

    framesPerSec    = config.getFrames();
    sizeBuffer      = config.getBuffer();
    address         = config.getAddr();
    port            = config.getPort();
    senha           = config.getSenha();
    isLocal         = config.getLocal();
    isRemoto        = config.getRemote();
    isPNG           = config.getPNG();
    isJPG           = config.getJPG();

    // To do: Salvar database

}

void Camera::showCV()
{
    cvNamedWindow( "KillCam", CV_WINDOW_AUTOSIZE );

    CvCapture* capture = cvCreateCameraCapture(0);
    if ( capture == NULL )
    {
        QMessageBox::information(this, tr(APP_TITLE), tr("Erro acessando dispositivo" ));
        return;
    }
    IplImage* frame;

    while(1){
        frame = cvQueryFrame( capture );
        if( !frame ) break;
        cvShowImage( "KillCam", frame );
        char c = cvWaitKey(33);
        if( c == 30 ) break;
    }
    cvReleaseCapture( &capture );
    cvDestroyWindow( "KillCam" );
}

void Camera::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_OSX
    if (!event->spontaneous() || !isVisible()) {
        return;
    }
#endif
    if (trayIcon->isVisible()) {
       // QMessageBox::information(this, tr(APP_TITLE), tr("O SDS continuará rodando no fundo " ));
        hide();
        event->ignore();
    }
}

void Camera::showMessage()
{

}

void Camera::messageClicked()
{

}

void Camera::createActions()
{
    transmitirAction = new QAction(tr("Capturar"), this);
    connect(transmitirAction, &QAction::triggered, this, &Camera::transmitir);

    configAction = new QAction(tr("Configuração"), this);
    connect(configAction, &QAction::triggered, this, &Camera::configurar);

    restoreAction = new QAction(tr("&Mostrar câmera"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    cvAction = new QAction(tr("&Captura CV"), this);
    connect(cvAction, &QAction::triggered, this, &Camera::showCV);

    quitAction = new QAction(tr("&Sair"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void Camera::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(transmitirAction);
    trayIconMenu->addAction(configAction);
    trayIconMenu->addAction(cvAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void Camera::add( CSocketDevice *sk )
{
    qDebug() << QString("New access %1:%2").arg( sk->getHost() ).arg( sk->getPort() );

    if ( tcpsender )  {
        tcpsender->finaliza();
        DESTROY( tcpsender );
    }

    tcpsender = new tcpSender( sk );
    connect ( tcpsender, SIGNAL( disconnected()), this, SLOT(stopRecord()));
    connect ( this, SIGNAL( sendFrame(const QImage &)), tcpsender, SLOT(addFrame(const QImage &)));
    transmitirAction->triggered();

}

void Camera::stopRecord()
{
    transmitirAction->setText("Capturar");
    disconnect( &tm, SIGNAL( timeout() ), this, SLOT(initRecord()));
    tm.stop();
    writeVideo();
    if ( video )
       DESTROY( video );
}

void Camera::transmitir()
{
    if ( tm.isActive() )
    {
        stopRecord();
        return;
    }

    listscr =  QGuiApplication::screens();
    screen = listscr.at(n_desktop); // QGuiApplication::primaryScreen();

    wd = windowHandle();
    screen = wd->screen();

    if ( isLocal )
       initVideo(screen->geometry().width(), screen->geometry().height());

    transmitirAction->setText("Parar");
    // Para 60 fps - 1000 / 60
    tm.setInterval(1000 / framesPerSec);
    qDebug() << "FrameTime = " << 1000 / framesPerSec;
    connect( &tm, SIGNAL( timeout() ), this, SLOT(initRecord()));
    tm.start();
    // getElapsed(true);
}

QString Camera::getElapsed(bool reset)
{
    if ( reset )
        et.start();
    float ret = et.elapsed() / 1000.000;
    et.restart();
    return QString().sprintf( "%.3fsec", ret );
}

void Camera::initRecord()
{
   // qDebug() << "Captura " << getElapsed();
    screen = wd->screen();

    if (!screen)
        return;

   // qDebug() << "grabWindow " << getElapsed();

    originalPixmap = screen->grabWindow(0);

   // qDebug() << "Take " << getElapsed();

    if ( isRemoto && tcpsender )
    {
       emit sendFrame( QImage(originalPixmap.toImage() ));
       return;
    }

    qImageSingle = originalPixmap.toImage(); //Convert QPicmap to QImage

    //Get QImage data to Open-cv Mat
    Mat frame = Mat(qImageSingle.height(),qImageSingle.width(),CV_8SC4,qImageSingle.bits(),qImageSingle.bytesPerLine()).clone();
    frames.append(frame);

    if ( frames.count() > sizeBuffer )
    {
            writeVideo();
    }
}


void Camera::writeVideo()
{
    for( int i=0; i < frames.count(); i++ )
    {
        Mat frame = frames.at(i);
        try
        {
             *video << frames.at(i);
        }
        catch (std::runtime_error& ex) {
            fprintf(stderr, "Exceção convertendo imagem para formato PNG %s\n", ex.what());
        }
    }
    frames.clear();
}

void Camera::initVideo(int width, int height)
{
    qDebug() << "Iniciando video " << width << "x"<< height;
    if ( video )
    {
        DESTROY(video);
    }

    std::vector<int> compression_params;
    int framerate = framesPerSec;
    if ( isPNG )
    {
        compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
        compression_params.push_back(9);
        video = new VideoWriter("/Users/rborges67/home/Samsung/desktop.avi",
                                CV_FOURCC('M','J','P','G'),
                                framerate,
                                Size( width, height ),
                                true);
    }
    else
    {
        compression_params.push_back(CV_IMWRITE_JPEG_OPTIMIZE);
        compression_params.push_back(9);
        video = new VideoWriter("/Users/rborges67/home/Samsung/desktop.avi",
                                CV_FOURCC('M','J','P','G'),
                                framerate,
                                Size( width, height ),
                                true);
    }
}

void Camera::moveEvent ( QMoveEvent *   )
{
    QPoint pos = this->mapToGlobal(this->pos());
    ui->lblX->setText(QString("%1").arg( pos.x()));
    ui->lblY->setText(QString("%1").arg( pos.y()));

    QDesktopWidget *dsk = QApplication::desktop();
    bool isvirtual = dsk->isVirtualDesktop();

    int i = dsk->numScreens();
    ui->lblInfo->setText( QString( "%1").arg(isvirtual));

}


