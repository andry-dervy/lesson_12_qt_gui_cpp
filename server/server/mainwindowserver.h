#ifndef MAINWINDOWSERVER_H
#define MAINWINDOWSERVER_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QLineEdit>

#include <memory>
#include <set>

class NetworkSettingsWidget: public QWidget
{
    Q_OBJECT

public:
    explicit NetworkSettingsWidget(QWidget* parent = nullptr);
    ~NetworkSettingsWidget() override;

private:
    std::unique_ptr<QUdpSocket> udp;
    std::unique_ptr<QTcpServer> tcp;

    QLineEdit* le_ip;
    QLineEdit* le_port;

    QPushButton* pbtn_activateListening;
    QPushButton* pbtn_disactivateListening;

    QRadioButton* rbtn_udp;
    QRadioButton* rbtn_tcp;

    const size_t MAX_SOCKETS = 10;
    std::set<std::unique_ptr<QTcpSocket>> tcpSockets;
    QTcpSocket* socket;
    void tcpDisconnectSockets();

public slots:
    void pbtn_activateListeningClicked();
    void pbtn_disactivateListeningClicked();

    void slotReadDataUDP();
    void slotNewTcpConnection();

    void slotReadDataTCP();
    void slotClientTcpDisconnected();

signals:
    void loging(QString str);
};

class MainWindowServer : public QMainWindow
{
    Q_OBJECT

public:
    MainWindowServer(QWidget *parent = nullptr);
    ~MainWindowServer();

private:
    void setDockNetworkSettings();
    void setMenuFile();

private:
    QTextEdit* te;
    NetworkSettingsWidget* networkSettingsWgt;
    QDockWidget* dockNetworkSettings;

public slots:
    void quit();
    void networkSettings();

    void loging(QString msg);
};

class DockWidgetEventFilter: public QObject
{
protected:
    bool eventFilter(QObject *object, QEvent *event);
};

#endif // MAINWINDOWSERVER_H
