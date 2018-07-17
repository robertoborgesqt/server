#include "config.h"
#include "ui_config.h"

Config::Config(QWidget *parent) : QDialog(parent),  ui(new Ui::Config)
{
    ui->setupUi(this);
    setWindowFlags( Qt::WindowStaysOnTopHint);
}

Config::~Config()
{
    delete ui;
}

int Config::getFrames()
{
    return  ui->spinFrames->value();
}

int Config::getBuffer()
{
    return ui->spinBuffer->value();
}

QString Config::getAddr()
{
    QString ret = ui->txtIp->text().split(":").at(0);
    return ret;
}

void Config::setFrames(int framesPerSec )
{
    ui->spinFrames->setValue(framesPerSec);
}

void Config::setBuffer(int sizeBuffer)
{
    ui->spinBuffer->setValue(sizeBuffer);
}

void Config::setAddr(const QString &address)
{
   QString s_addr = QString( "%1:%2" ).arg(address).arg(port);
   ui->txtIp->setText( s_addr );
}

void Config::setPort(int port)
{
    this->port = port;
}

void Config::setLocal(bool local)
{
    ui->chLocal->setChecked(local);
}

void Config::setRemote(bool remote)
{
    ui->chRemota->setChecked(remote);
}

void Config::setSenha(const QString &senha)
{
    ui->txtSenha->setText(senha);
}

void Config::setPNG(bool png)
{
    ui->rdPNG->setChecked(png);
}

void Config::setJPG(bool jpg)
{
    ui->rdJPG->setChecked(jpg);
}

int Config::getPort()
{
    QString ret = ui->txtIp->text().split(":").at(1);
    return ret.toInt();
}

bool Config::getRemote()
{
    return ui->chRemota->isChecked();
}

bool Config::getLocal()
{
    return ui->chLocal->isChecked();
}

bool Config::getPNG()
{
    return ui->rdPNG->isChecked();
}

bool Config::getJPG()
{
    return ui->rdJPG->isChecked();
}

QString Config::getSenha()
{
    return ui->txtSenha->text();
}

