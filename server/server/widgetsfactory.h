#ifndef WIDGETSFACTORY_H
#define WIDGETSFACTORY_H

#include <QString>
#include <QAction>
#include <QMenu>
#include <QPixmap>
#include <QToolButton>
#include <QComboBox>
#include <QPushButton>

template<class T>
class WidgetsFactory
{
public:
    WidgetsFactory() = default;
    virtual ~WidgetsFactory() = default;
public:
    static std::optional<T *> create(
            const QString&& nameObject, QWidget *parent = nullptr, bool checkable = false, const QPixmap&& icon = QPixmap());

    static void setText(QObject* parent,const QString&& nameObject, const QString &&text);
    static void setChecked(QObject* parent,const QString&& nameObject, const bool b);
    static bool isChecked(QObject* parent,const QString&& nameObject);
};

//extern template void WidgetsFactory<QAction>::setText(QObject* obj,const QString&& nameObject,const QString&& text);
//extern template void WidgetsFactory<QMenu>::setText(QObject* obj,const QString&& nameObject,const QString&& text);
//extern template std::optional<QAction *> WidgetsFactory<QAction>::create(const QString &&nameObject, QWidget *parent, bool checkable, const QPixmap &&icon);
//extern template std::optional<QMenu *> WidgetsFactory<QMenu>::create(const QString &&nameObject, QWidget *parent, bool checkable, const QPixmap &&icon);
//extern template std::optional<QToolButton *> WidgetsFactory<QToolButton>::create(const QString &&nameObject, QWidget *parent, bool checkable, const QPixmap &&icon);
//extern template void WidgetsFactory<QToolButton>::setText(QObject* obj,const QString&& nameObject,const QString&& text);

#endif // WIDGETSFACTORY_H
