#ifndef MAINWINDOWCLIENT_H
#define MAINWINDOWCLIENT_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QLineEdit>

#include <memory>

class Network: public QObject
{
    Q_OBJECT

public:
    enum class SoketType
    {
        UDP,TCP
    };

public:
    Network(SoketType socket):socket_t(socket){}
    virtual ~Network(){}
    virtual void send(QString msg) = 0;
    SoketType getSoketType() const {return socket_t;}

protected:
    SoketType socket_t;

signals:
    void recieve(QString msg);
};

class NetworkUDP: public Network
{
    Q_OBJECT
public:
    NetworkUDP(QHostAddress addr_serv, uint16_t port_serv);
    ~NetworkUDP() override;
    void send(QString msg) override;
private:
    std::unique_ptr<QUdpSocket> udp;
    QHostAddress addr_server;
    uint16_t port_server;
private slots:
    void slotReadData();
};

class NetworkTCP: public Network
{
    Q_OBJECT
public:
    NetworkTCP(QHostAddress addr_serv, uint16_t port_serv);
    ~NetworkTCP() override;
    void send(QString msg) override;
private:
    std::unique_ptr<QTcpSocket> tcp;
    QHostAddress addr_server;
    uint16_t port_server;
private slots:
    void slotReadData();
};

class NetworkSettingsWidget: public QWidget
{
    Q_OBJECT

public:
    explicit NetworkSettingsWidget(QWidget* parent = nullptr);

private:
    QLineEdit* le_ip;
    QLineEdit* le_port;

    QPushButton* pbtn_activateListening;
    QPushButton* pbtn_disactivateListening;

    QRadioButton* rbtn_udp;
    QRadioButton* rbtn_tcp;

public slots:
    void pbtn_activateListeningClicked();
    void pbtn_disactivateListeningClicked();

signals:
    void loging(QString str);
    void open_port(std::shared_ptr<Network> net);
    void close_port();
};


class MainWindowClient : public QMainWindow
{
    Q_OBJECT

public:
    MainWindowClient(QWidget *parent = nullptr);
    ~MainWindowClient();

private:
    void setDockNetworkSettings();
    void setMenuFile();
    void setCentralWidget_();

private:
    QTextEdit* te;
    NetworkSettingsWidget* networkSettingsWgt;
    QDockWidget* dockNetworkSettings;

    QLineEdit* le_message;
    QPushButton* pbtn_send;

    std::shared_ptr<Network> net;

public slots:
    void quit();
    void networkSettings();

    void loging(QString msg);
    void open_port(std::shared_ptr<Network> net_);
    void close_port();

    void send();
};

class DockWidgetEventFilter: public QObject
{
protected:
    bool eventFilter(QObject *object, QEvent *event);
};

#endif // MAINWINDOWCLIENT_H
