#include "mainwindowserver.h"
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
#include <QAbstractSocket>

#include "QApplication"
#include "widgetsfactory.h"

void MainWindowServer::setMenuFile()
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
    connect(*act, &QAction::triggered, this, &MainWindowServer::quit);

    auto menuSettting = WidgetsFactory<QMenu>::create("menuSettings",this);
    Q_ASSERT(menuSettting != nullptr);
    menubar->addMenu(*menuSettting);
    WidgetsFactory<QMenu>::setText(this,"menuSettings",tr("&Настройки"));
    act = WidgetsFactory<QAction>::create("actNetworkSetting",this,true);
    Q_ASSERT(act != nullptr);
    (*menuSettting)->addAction(*act);
    WidgetsFactory<QAction>::setText(this,"actNetworkSetting",tr("&Сетевые настройки"));
    connect(*act, &QAction::triggered, this, &MainWindowServer::networkSettings);
}

void MainWindowServer::quit()
{
    qApp->quit();
}

void MainWindowServer::networkSettings()
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

void MainWindowServer::loging(QString msg)
{
    te->append(msg);
}

MainWindowServer::MainWindowServer(QWidget *parent)
    : QMainWindow(parent)
    , te(new QTextEdit(this))
    , networkSettingsWgt(new NetworkSettingsWidget(this))
    , dockNetworkSettings(nullptr)
{
    resize(300,500);
    setWindowTitle(tr("Сервер"));
    setMenuFile();

    setCentralWidget(te);

    connect(networkSettingsWgt,&NetworkSettingsWidget::loging,this,&MainWindowServer::loging);
}

MainWindowServer::~MainWindowServer()
{}

void MainWindowServer::setDockNetworkSettings()
{
    dockNetworkSettings = new QDockWidget(tr("Сетевые настройки"),this);
    dockNetworkSettings->installEventFilter(new DockWidgetEventFilter());
    dockNetworkSettings->setWidget(networkSettingsWgt);
    addDockWidget(Qt::TopDockWidgetArea,dockNetworkSettings);
}

NetworkSettingsWidget::NetworkSettingsWidget(QWidget* parent)
    : QWidget(parent)
    , udp(nullptr)
    , tcp(nullptr)
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

NetworkSettingsWidget::~NetworkSettingsWidget()
{
    tcpDisconnectSockets();
}

void NetworkSettingsWidget::tcpDisconnectSockets()
{
    for(auto it = tcpSockets.begin(); it != tcpSockets.end();++it) {
        auto& sock = *it;
        disconnect(sock.get(), &QTcpSocket::readyRead, this, &NetworkSettingsWidget::slotReadDataTCP);
        disconnect(sock.get(), &QTcpSocket::disconnected, this, &NetworkSettingsWidget::slotClientTcpDisconnected);
    }
    tcpSockets.clear();
}

void NetworkSettingsWidget::pbtn_activateListeningClicked()
{
    if(rbtn_udp->isChecked()) {
        udp = std::make_unique<QUdpSocket>();
        if(!udp->bind(QHostAddress(le_ip->text()),le_port->text().toInt())) {
            emit loging("The binding didn't executed.");
            return;
        }
        emit loging("The binding executed.");

        connect(udp.get(),&QUdpSocket::readyRead,this,&NetworkSettingsWidget::slotReadDataUDP);
    }
    else if(rbtn_tcp->isChecked()) {
        tcp = std::make_unique<QTcpServer>();
        if(!tcp->listen(QHostAddress(le_ip->text()),le_port->text().toInt())) {
            emit loging("The listening didn't executed.");
            return;
        }
        emit loging("The listening is executed.");

        connect(tcp.get(),&QTcpServer::newConnection,this,&NetworkSettingsWidget::slotNewTcpConnection);
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
        disconnect(udp.get(),&QUdpSocket::readyRead,this,&NetworkSettingsWidget::slotReadDataUDP);
        udp = nullptr;
        emit loging("The binding is broken.");
    }
    else if(rbtn_tcp->isChecked()) {
        tcpDisconnectSockets();
        emit loging("The connection is broken.");
    }

    rbtn_udp->setEnabled(true);
    rbtn_tcp->setEnabled(true);
    le_ip->setEnabled(true);
    le_port->setEnabled(true);
    pbtn_activateListening->setEnabled(true);
    pbtn_disactivateListening->setEnabled(false);
}

void NetworkSettingsWidget::slotReadDataUDP()
{
    while(udp->hasPendingDatagrams()) {
        QNetworkDatagram datagrm = udp->receiveDatagram();
        QString msg;
        msg += "(from " + datagrm.senderAddress().toString() + "/" +
               QString::number(datagrm.senderPort()) + ") ";
        msg += QDateTime::currentDateTime().toString("yyyy.MM.dd : hh.mm.ss") + ": ";
        msg += datagrm.data();
        emit loging(msg);
        udp->writeDatagram(datagrm.data(),datagrm.senderAddress(),datagrm.senderPort());
    }
}

void NetworkSettingsWidget::slotNewTcpConnection()
{
    auto tcpSocket = tcp->nextPendingConnection();

    if(tcpSockets.size() == MAX_SOCKETS) {
        tcpSocket->disconnect();
        return;
    }

    tcpSocket->write("I am server!");
    connect(tcpSocket, &QTcpSocket::readyRead, this, &NetworkSettingsWidget::slotReadDataTCP);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &NetworkSettingsWidget::slotClientTcpDisconnected);
    std::unique_ptr<QTcpSocket> socket{tcpSocket};
    tcpSockets.insert(std::move(socket));
}

void NetworkSettingsWidget::slotReadDataTCP()
{
    qDebug() << "slotReadDataTCP: tcpSockets.size =" << tcpSockets.size();
    for(const auto& sock: tcpSockets) {
        qDebug() << "sock.bytesAvailable =" << sock->bytesAvailable();
        while(sock->bytesAvailable() > 0) {
            QByteArray array = sock->readAll();
            QString msg;
            msg += "(from " + sock->peerAddress().toString() + "/" +
                   QString::number(sock->peerPort()) + ") ";
            msg += QDateTime::currentDateTime().toString("yyyy.MM.dd : hh.mm.ss") + ": ";
            msg += array;
            emit loging(msg);

            sock->write(array);
        }
    }
}

void NetworkSettingsWidget::slotClientTcpDisconnected()
{
    for(auto it = tcpSockets.begin(); it != tcpSockets.end();)
    {
        auto& sock = *it;
        if(sock->state() == QAbstractSocket::UnconnectedState) {
            disconnect(sock.get(), &QTcpSocket::readyRead, this, &NetworkSettingsWidget::slotReadDataTCP);
            disconnect(sock.get(), &QTcpSocket::disconnected, this, &NetworkSettingsWidget::slotClientTcpDisconnected);
            it = tcpSockets.erase(it);
        }
        else ++it;
    }
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
