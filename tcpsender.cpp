#include "tcpsender.h"

#include <QBuffer>
#include <QImage>
#include <ccrc.h>
#include <qeventloop.h>

tcpSender::tcpSender( CSocketDevice *sk )
{
    this->sk = sk;

    QObject::connect( this->sk,  SIGNAL( closed   ()), this, SLOT( socketClosed())  );
    QObject::connect( this->sk,  SIGNAL( opened   ()), this, SLOT( connectionOk())  );
    QObject::connect( this->sk,  SIGNAL( error    ()), this, SLOT( connectionError()) );

    connect( sk,   SIGNAL( hasData  (const QByteArray &) ), this, SLOT( receiveData(const QByteArray &)) );
    connect( this, SIGNAL( transmit (const QByteArray &) ), sk ,  SLOT( writedata(const QByteArray &)) );
    connect( this, SIGNAL( send     (const QByteArray &) ), this ,  SLOT( sendpackage(const QByteArray &)) );
    connect( this, SIGNAL( next() ) , SLOT(scanList()));

    tm.setInterval(50);
    connect( &tm, SIGNAL(timeout()), SLOT(scanList()));
    tm.start();

}

void tcpSender::finaliza()
{
    state = TH_FINISH;
    tm.stop();
}

void tcpSender::addFrame( const QImage &img  )
{
    QByteArray w_ba;
    QBuffer buffer(&w_ba);      // Construct a QBuffer object using the QbyteArray
    img.save(&buffer, "JPG");     // Save the QImage data into the QBuffer
    queue.enqueue(w_ba);
}

QString tcpSender::getElapsed(bool reset)
{
    if ( reset )
        et.start();
    float ret = et.elapsed() / 1000.000;
    et.restart();
    return QString().sprintf( "%.3fsec", ret );
}

void tcpSender::scanList()
{
    tm.stop();

    if ( queue.empty() ) {
        tm.start();
        return;
    }

    image_ba = queue.dequeue();
    qDebug() << "Sending "<< image_ba.size() << " Time = "<< getElapsed();

    emit transmit(QString( "<SDSPayload=%1>").arg(image_ba.size()).toLatin1());

}

void tcpSender::receiveData(const QByteArray &ba)
{
    if ( ba.contains("<SDSOk>")  )
    {
       CCrc crc( image_ba );
       QByteArray ret =  QString( "<SDSCRC=%1>").arg( crc.get().toHex().toUpper().data() ).toLatin1();
       emit transmit( ret  );
    }
    else
    if ( ba.contains("<SDSCRCOk>")  )
    {
       emit transmit(image_ba);
    }
    else
    if ( ba.contains("<SDSNext>")  )
        emit next();
}

void tcpSender::sendpackage( const QByteArray & ba )
{
    sk->writedata( QString("<SDSSend=%1>" ).arg( ba.size()));
    sk->waitForReadyRead();
    QByteArray resp = sk->readAll();
    if ( resp == "<SDSOK>" )
    {
        qDebug() << "OK Recebido";
        sk->writedata(ba);
    }
    qDebug() << "Aguardando finalizacao...";

    for( int i=0; i < 10; i ++ )
    {
        sk->waitForReadyRead();
        qDebug() << "recebeu .....";
        resp = sk->readAll();
        qDebug() << "Received " << resp;
        if ( resp == "<SDSFINISHED>" )
        {
            qDebug() << "SDSFINISHED";
            erroState = false;
            break;
        }
        else
        {
            qDebug() << "TCPThread Async error";
            erroState = true;
        }
    }
    emit finalizado();
}

void tcpSender::socketClosed()
{
    qDebug() << "Socket closed";
    tm.stop();
    queue.clear();
    emit disconnected();
}

void tcpSender::connectionOk()
{
    qDebug() << "Socket connectionOk";
}

void tcpSender::connectionError()
{
    qDebug() << "Socket connectionError";
}

void tcpSender::clientDisconnected()
{
    qDebug() << "Socket clientDisconnected";
}

