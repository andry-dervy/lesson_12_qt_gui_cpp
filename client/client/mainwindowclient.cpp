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

MainWindowClient::MainWindowClient(QWidget *parent)
    : QMainWindow(parent)
{
    resize(300,500);
    setWindowTitle(tr("Клиент"));
    setMenuFile();

    setCentralWidget(te);

    connect(networkSettingsWgt,&NetworkSettingsWidget::loging,this,&MainWindowClient::loging);
}

MainWindowClient::~MainWindowClient()
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

void NetworkSettingsWidget::pbtn_activateListeningClicked()
{
    if(rbtn_udp->isChecked()) {
        udp = std::make_unique<QUdpSocket>();
        if(!udp->bind(QHostAddress(le_ip->text()),le_port->text().toInt())) {
            emit loging("The binding didn't executed.");
            return;
        }
        emit loging("The binding executed.");

        connect(udp.get(),&QUdpSocket::readyRead,this,&NetworkSettingsWidget::slotReadData);
    }
    else if(rbtn_tcp->isChecked()) {

    }

    rbtn_udp->setEnabled(false);
    rbtn_tcp->setEnabled(false);
    pbtn_activateListening->setEnabled(false);
    pbtn_disactivateListening->setEnabled(true);
}

void NetworkSettingsWidget::pbtn_disactivateListeningClicked()
{
    if(rbtn_udp->isChecked()) {
        disconnect(udp.get(),&QUdpSocket::readyRead,this,&NetworkSettingsWidget::slotReadData);
        udp = nullptr;
        emit loging("The binding is broken.");
    }
    else if(rbtn_tcp->isChecked()) {

    }

    rbtn_udp->setEnabled(true);
    rbtn_tcp->setEnabled(true);
    pbtn_activateListening->setEnabled(true);
    pbtn_disactivateListening->setEnabled(false);
}

void NetworkSettingsWidget::slotReadData()
{
    while(udp->hasPendingDatagrams()) {
        QNetworkDatagram datagrm = udp->receiveDatagram();
        QString msg;
        msg += "(" + datagrm.senderAddress().toString() + "/" + datagrm.senderPort() + ") ";
        msg += QDateTime::currentDateTime().toString() + ": ";
        msg += datagrm.data();
        emit loging(msg);
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
