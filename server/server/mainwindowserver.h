#ifndef MAINWINDOWSERVER_H
#define MAINWINDOWSERVER_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QUdpSocket>
#include <QTcpServer>
#include <QLineEdit>

#include <memory>

class NetworkSettingsWidget: public QWidget
{
    Q_OBJECT

public:
    explicit NetworkSettingsWidget(QWidget* parent = nullptr);

private:
    std::unique_ptr<QUdpSocket> udp;
    std::unique_ptr<QTcpServer> tcp;

    QLineEdit* le_ip;
    QLineEdit* le_port;

    QPushButton* pbtn_activateListening;
    QPushButton* pbtn_disactivateListening;

    QRadioButton* rbtn_udp;
    QRadioButton* rbtn_tcp;

public slots:
    void pbtn_activateListeningClicked();
    void pbtn_disactivateListeningClicked();

    void slotReadData();

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
