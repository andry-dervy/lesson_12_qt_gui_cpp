#include "mainwindowclient.h"
#include <QDockWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QMenuBar>
#include <QNetworkDatagram>
#include <QDateTime>

#include "QApplication"
#include "widgetsfactory.h"

void MainWindowClient::setMenuFile()
{
    auto menubar = new QMenuBar(this);
    setMenuBar(menubar);

    auto menuFile = WidgetsFactory<QMenu>::create("menuFile",this);
    Q_ASSERT(menuFile != nullptr);
    menubar->addMenu(*menuFile);
    WidgetsFactory<QMenu>::setText(this,"menuFile",tr("&Файл"));
    auto act = WidgetsFactory<QAction>::create("actQuit",this);
    Q_ASSERT(act != nullptr);
    (*menuFile)->addAction(*act);
    WidgetsFactory<QAction>::setText(this,"actQuit",tr("&Выход"));
    connect(*act, &QAction::triggered, this, &MainWindowClient::quit);

    auto menuSettting = WidgetsFactory<QMenu>::create("menuSettings",this);
    Q_ASSERT(menuSettting != nullptr);
    menubar->addMenu(*menuSettting);
    WidgetsFactory<QMenu>::setText(this,"menuSettings",tr("&Настройки"));
    act = WidgetsFactory<QAction>::create("actNetworkSetting",this,true);
    Q_ASSERT(act != nullptr);
    (*menuSettting)->addAction(*act);
    WidgetsFactory<QAction>::setText(this,"actNetworkSetting",tr("&Сетевые настройки"));
    connect(*act, &QAction::triggered, this, &MainWindowClient::networkSettings);
}

void MainWindowClient::setCentralWidget_()
{
    auto wgt = new QWidget(this);
    auto layout = new QVBoxLayout();
    wgt->setLayout(layout);
    layout->addWidget(te);
    auto hlayout = new QHBoxLayout();
    layout->addLayout(hlayout);
    le_message = new QLineEdit(wgt);
    pbtn_send = new QPushButton(tr("Отправить"),wgt);
    pbtn_send->setEnabled(false);
    connect(pbtn_send,&QPushButton::clicked,this,&MainWindowClient::send);
    hlayout->addWidget(le_message);
    hlayout->addWidget(pbtn_send);

    setCentralWidget(wgt);
}

void MainWindowClient::quit()
{
    qApp->quit();
}

void MainWindowClient::networkSettings()
{
    auto act =  findChild<QAction*>("actNetworkSetting");
    Q_ASSERT(act != nullptr);
    if(act->isChecked()) {
        setDockNetworkSettings();
    }
    else {
        dockNetworkSettings->close();
    }
}

void MainWindowClient::loging(QString msg)
{
    te->append(msg);
}

void MainWindowClient::open_port(std::shared_ptr<Network> net_)
{
    net = net_;
    connect(net.get(),&Network::recieve,this,&MainWindowClient::loging);
    pbtn_send->setEnabled(true);
}

void MainWindowClient::close_port()
{
    pbtn_send->setEnabled(false);
    disconnect(net.get(),&Network::recieve,this,&MainWindowClient::loging);
    net = nullptr;
}

void MainWindowClient::send()
{
    net->send(le_message->text());
    le_message->clear();
}

MainWindowClient::MainWindowClient(QWidget *parent)
    : QMainWindow(parent)
    , te(new QTextEdit(this))
    , networkSettingsWgt(new NetworkSettingsWidget(this))
    , dockNetworkSettings(nullptr)
{
    resize(300,500);
    setWindowTitle(tr("Клиент"));
    setMenuFile();

    setCentralWidget_();

    connect(networkSettingsWgt,&NetworkSettingsWidget::loging,this,&MainWindowClient::loging);
    connect(networkSettingsWgt,&NetworkSettingsWidget::open_port,this,&MainWindowClient::open_port);
    connect(networkSettingsWgt,&NetworkSettingsWidget::close_port,this,&MainWindowClient::close_port);
}

MainWindowClient::~MainWindowClient()
{}

void MainWindowClient::setDockNetworkSettings()
{
    dockNetworkSettings = new QDockWidget(tr("Сетевые настройки"),this);
    dockNetworkSettings->installEventFilter(new DockWidgetEventFilter());
    dockNetworkSettings->setWidget(networkSettingsWgt);
    addDockWidget(Qt::TopDockWidgetArea,dockNetworkSettings);
}

NetworkSettingsWidget::NetworkSettingsWidget(QWidget* parent)
    : QWidget(parent)
{
    auto layout = new QGridLayout();
    setLayout(layout);
    auto  groupBox = new QGroupBox(tr("Транспортный протокол"));
    layout->addWidget(groupBox,0,0,1,2);

    auto gb_layout = new QHBoxLayout();
    groupBox->setLayout(gb_layout);
    rbtn_udp = new QRadioButton(tr("UDP"),this);
    rbtn_udp->setChecked(true);
    gb_layout->addWidget(rbtn_udp);
    rbtn_tcp = new QRadioButton(tr("TCP"),this);
    gb_layout->addWidget(rbtn_tcp);

    le_ip = new QLineEdit(this);
    le_ip->setText("127.0.0.1");
    layout->addWidget(le_ip,1,0,1,1);
    layout->addWidget(new QLabel(tr("IP-адрес")),1,1,1,1);
    le_port = new QLineEdit(this);
    le_port->setText("12001");
    layout->addWidget(le_port,2,0,1,1);
    layout->addWidget(new QLabel(tr("Порт")),2,1,1,1);
    pbtn_activateListening = new QPushButton(tr("Открыть порт"),this);
    connect(pbtn_activateListening,&QPushButton::clicked,
            this, &NetworkSettingsWidget::pbtn_activateListeningClicked);
    layout->addWidget(pbtn_activateListening,3,0,1,1);
    pbtn_disactivateListening = new QPushButton(tr("Закрыть порт"),this);
    pbtn_disactivateListening->setEnabled(false);
    connect(pbtn_disactivateListening,&QPushButton::clicked,
            this, &NetworkSettingsWidget::pbtn_disactivateListeningClicked);
    layout->addWidget(pbtn_disactivateListening,3,1,1,1);

    layout->setRowStretch(4,1);
}

void NetworkSettingsWidget::pbtn_activateListeningClicked()
{
    if(rbtn_udp->isChecked()) {
        emit open_port(std::make_shared<NetworkUDP>(
                           QHostAddress(le_ip->text()),
                           le_port->text().toUInt()));
        emit loging("The binding executed.");
    }
    else if(rbtn_tcp->isChecked()) {
        emit open_port(std::make_shared<NetworkTCP>(
                           QHostAddress(le_ip->text()),
                           le_port->text().toUInt()));
        emit loging("The binding executed.");
    }

    rbtn_udp->setEnabled(false);
    rbtn_tcp->setEnabled(false);
    le_ip->setEnabled(false);
    le_port->setEnabled(false);
    pbtn_activateListening->setEnabled(false);
    pbtn_disactivateListening->setEnabled(true);
}

void NetworkSettingsWidget::pbtn_disactivateListeningClicked()
{
    if(rbtn_udp->isChecked()) {
        emit loging("The binding is broken.");
        emit close_port();
    }
    else if(rbtn_tcp->isChecked()) {
        emit loging("The binding is broken.");
        emit close_port();
    }

    rbtn_udp->setEnabled(true);
    rbtn_tcp->setEnabled(true);
    le_ip->setEnabled(true);
    le_port->setEnabled(true);
    pbtn_activateListening->setEnabled(true);
    pbtn_disactivateListening->setEnabled(false);
}

bool DockWidgetEventFilter::eventFilter(QObject* object, QEvent* event)
{
    auto dock = qobject_cast<QDockWidget*>(object);
    if(!dock) return false;
    if(event->type() == QEvent::Type::Close) {
        WidgetsFactory<QAction>::setChecked(dock->parent(),
                                            "actNetworkSetting",
                                            false);
        return true;
    }
    return false;
}

NetworkUDP::NetworkUDP(QHostAddress addr_serv, uint16_t port_serv)
    : Network(SoketType::UDP)
    , udp(new QUdpSocket())
    , addr_server(addr_serv)
    , port_server(port_serv)
{
    udp->bind(QHostAddress::AnyIPv4);
    connect(udp.get(),&QUdpSocket::readyRead,this,&NetworkUDP::slotReadData);
}

NetworkUDP::~NetworkUDP()
{
    disconnect(udp.get(),&QUdpSocket::readyRead,this,&NetworkUDP::slotReadData);
}

void NetworkUDP::send(QString msg)
{
    udp->writeDatagram(msg.toUtf8(),addr_server,port_server);
}

void NetworkUDP::slotReadData()
{
    while(udp->hasPendingDatagrams()) {
        QNetworkDatagram datagrm = udp->receiveDatagram();
        QString msg;
        msg += "(from " + datagrm.senderAddress().toString() + "/" +
               QString::number(datagrm.senderPort()) + ") ";
        msg += QDateTime::currentDateTime().toString("yyyy.MM.dd : hh.mm.ss") + ": ";
        msg += datagrm.data();
        emit recieve(msg);
    }
}

NetworkTCP::NetworkTCP(QHostAddress addr_serv, uint16_t port_serv)
    : Network(SoketType::TCP)
    , tcp(new QTcpSocket())
    , addr_server(addr_serv)
    , port_server(port_serv)
{
    tcp->connectToHost(addr_server,port_server);
    if(!tcp->waitForConnected()) {
        qDebug() << "The connection is not established.";
    }
    connect(tcp.get(),&QTcpSocket::readyRead,this,&NetworkTCP::slotReadData);
}

NetworkTCP::~NetworkTCP()
{
    disconnect(tcp.get(),&QTcpSocket::readyRead,this,&NetworkTCP::slotReadData);
}

void NetworkTCP::send(QString msg)
{
    if(tcp->state() == QAbstractSocket::ConnectedState) {
        tcp->write(msg.toUtf8());
        qDebug() << msg;
    }
    else
        qDebug() << "The connection is not established.";
}

void NetworkTCP::slotReadData()
{
    while(tcp->bytesAvailable()>0) {
        QByteArray array = tcp->readAll();
        QString msg;
        msg += "(from " + tcp->peerAddress().toString() + "/" +
               QString::number(tcp->peerPort()) + ") ";
        msg += QDateTime::currentDateTime().toString("yyyy.MM.dd : hh.mm.ss") + ": ";
        msg += array;
        emit recieve(msg);
    }
}
