#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>

namespace Ui {
class Config;
}

class Config : public QDialog
{
    Q_OBJECT

public:
    explicit Config(QWidget *parent = 0);
    ~Config();

    int     getFrames();
    int     getBuffer();
    int     getPort  ();
    bool    getRemote();
    bool    getLocal ();
    bool    getPNG   ();
    bool    getJPG   ();
    QString getSenha ();
    QString getAddr  ();

    void setFrames ( int framesPerSec);
    void setBuffer ( int sizeBuffer  );
    void setAddr   ( const QString & address  );
    void setPort   ( int port );
    void setLocal  ( bool local );
    void setRemote ( bool remote );
    void setSenha  ( const QString &senha );
    void setPNG    ( bool png );
    void setJPG    ( bool jpg );

private:
    Ui::Config *ui;
    int port=6767;
};

#endif // CONFIG_H
