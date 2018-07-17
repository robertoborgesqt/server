#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) : QWidget(parent), ui(new Ui::Form)
{
    ui->setupUi(this);

    listscr =  QGuiApplication::screens();

    QDesktopWidget *desktop = QApplication::desktop();

    int desktops =  desktop->screenCount();
    QRect sz1 = desktop->screenGeometry (0);
    QRect sz2 = desktop->screenGeometry (1);

    ui->spinBox->setMaximum(listscr.count());

    connect( ui->spinBox,
             SIGNAL(valueChanged(int)),
             this,
             SLOT(setDesktop(int))
             );

    connect( ui->btRecord, SIGNAL(clicked(bool)), this, SLOT(startTimer()));
    connect( ui->btPlay, SIGNAL(clicked(bool)), this, SLOT( initPlay()));

}

Form::~Form()
{
    delete ui;
}

void Form::setDesktop(int p_desktop )
{
    n_desktop = p_desktop;
}

void Form::updateScreen()
{
    ui->lblPicure->setPixmap(originalPixmap.scaled( ui->lblPicure->size(),
                             Qt::KeepAspectRatio,
                             Qt::SmoothTransformation));
}

void Form::initRecord()
{
    QScreen *screen = listscr.at(n_desktop); // QGuiApplication::primaryScreen();

    if (const QWindow *window = windowHandle())
        screen = window->screen();

    if (!screen)
        return;

    originalPixmap = screen->grabWindow(0);
    qImageSingle = originalPixmap.toImage(); //Convert QPicmap to QImage

    //Get QImage data to Open-cv Mat
    Mat frame = Mat(qImageSingle.height(),qImageSingle.width(),CV_8UC4,qImageSingle.bits(),qImageSingle.bytesPerLine()).clone();
    frames.append(frame);

    if ( frames.count() > 100 )
        writeVideo();

    updateScreen();

    show();
}

void Form::initPlay()
{

}

void Form::writeVideo()
{
    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    if ( video == NULL )
        video = new VideoWriter("/Users/rborges67/home/Samsung/desktop.avi",
                                CV_FOURCC('M','J','P','G'),
                                10,
                                Size(qImageSingle.width(),
                                     qImageSingle.height()),
                                true);

    for( int i=0; i < frames.count(); i++ )
    {
        Mat frame = frames.at(i);
        try
        {
             *video << frames.at(i);
        }
        catch (std::runtime_error& ex) {
            fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
        }
    }
    frames.clear();
}

void Form::startTimer()
{
    if ( tm.isActive() )
    {
       ui->btRecord->setText("Start");
       disconnect( &tm, SIGNAL( timeout() ), this, SLOT(initRecord()));
       tm.stop();
       writeVideo();
       delete video;
       return;
    }

    ui->btRecord->setText("Stop");
    // Para 60 fps - 1000 / 60
    tm.setInterval(1000 / 60);
    connect( &tm, SIGNAL( timeout() ), this, SLOT(initRecord()));
    tm.start();
}
