#ifndef TCP_SENDER_H
#define TCP_SENDER_H

#include <QElapsedTimer>
#include <QObject>
#include <csocketdevice.h>
#include <qqueue.h>

enum states { TH_WAIT, TH_RUNNING, TH_FINISH };

class tcpSender : public QObject
{
    Q_OBJECT

public:
    tcpSender(CSocketDevice *sk);
    void finaliza();

protected:
    CSocketDevice *sk = NULL;
    QTimer tm;
    states state= TH_WAIT;
    QQueue <QByteArray> queue;
    bool erroState=false;
    QByteArray image_ba;
    QElapsedTimer et;
    QString getElapsed(bool reset=false );

public slots:
    void  scanList();
    void  sendpackage( const QByteArray & ba );
    void  addFrame(const QImage &img );

    void socketClosed();
    void connectionOk();
    void connectionError();
    void clientDisconnected() ;

    void receiveData(const QByteArray & ba) ;

signals:
    void  send        ( const QByteArray & );
    void  transmit    ( const QByteArray & );
    void  next        ();
    void  finalizado  ();
    void  disconnected();
};

#endif // TCP_SENDER_H
