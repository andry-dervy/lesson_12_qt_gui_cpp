#include "widgetsfactory.h"

template<class T>
std::optional<T *> WidgetsFactory<T>::create(const QString &&nameObject, QWidget *parent, bool checkable, const QPixmap &&icon)
{
    Q_UNUSED(nameObject);
    Q_UNUSED(parent);
    Q_UNUSED(checkable);
    Q_UNUSED(icon);
    return std::nullopt;
}

template<>
std::optional<QAction *> WidgetsFactory<QAction>::create(const QString &&nameObject, QWidget *parent, bool checkable, const QPixmap &&icon)
{
    auto t = new QAction(parent);
    if(!t) return std::nullopt;
    t->setObjectName(nameObject);
    t->setCheckable(checkable);
    t->setIcon(icon);
    return std::make_optional<QAction*>(t);
}

template<>
std::optional<QMenu *> WidgetsFactory<QMenu>::create(const QString &&nameObject, QWidget *parent, bool checkable, const QPixmap &&icon)
{
    Q_UNUSED(checkable);
    auto t = new QMenu(parent);
    if(!t) return std::nullopt;
    t->setObjectName(nameObject);
    t->setIcon(icon);
    return std::make_optional<QMenu*>(t);
}

template<>
std::optional<QToolButton *> WidgetsFactory<QToolButton>::create(const QString &&nameObject, QWidget *parent, bool checkable, const QPixmap &&icon)
{
    Q_UNUSED(checkable);
    auto t = new QToolButton(parent);
    if(!t) return std::nullopt;
    t->setObjectName(nameObject);
    t->setIcon(icon);
    return std::make_optional<QToolButton*>(t);
}

template<>
std::optional<QComboBox *> WidgetsFactory<QComboBox>::create(const QString &&nameObject, QWidget *parent, bool checkable, const QPixmap &&icon)
{
    Q_UNUSED(checkable);
    Q_UNUSED(icon);
    auto t = new QComboBox(parent);
    if(!t) return std::nullopt;
    t->setObjectName(nameObject);
    return std::make_optional<QComboBox*>(t);
}

template<>
std::optional<QPushButton *> WidgetsFactory<QPushButton>::create(const QString &&nameObject, QWidget *parent, bool checkable, const QPixmap &&icon)
{
    Q_UNUSED(checkable);
    Q_UNUSED(icon);
    auto t = new QPushButton(parent);
    if(!t) return std::nullopt;
    t->setObjectName(nameObject);
    return std::make_optional<QPushButton*>(t);
}

template<class T>
void WidgetsFactory<T>::setText(QObject *parent, const QString &&nameObject,const QString &&text)
{
    Q_UNUSED(parent);
    Q_UNUSED(nameObject);
    Q_UNUSED(text);
}

template<>
void WidgetsFactory<QAction>::setText(QObject* parent,const QString&& nameObject,const QString&& text)
{
    Q_ASSERT(parent != nullptr);
    auto act = parent->findChild<QAction*>(nameObject);
    if(act != nullptr) act->setText(text);
}

template<>
void WidgetsFactory<QMenu>::setText(QObject* parent,const QString&& nameObject,const QString&& text)
{
    Q_ASSERT(parent != nullptr);
    auto menu = parent->findChild<QMenu*>(nameObject);
    if(menu != nullptr) menu->setTitle(text);
}

template<>
void WidgetsFactory<QComboBox>::setText(QObject* parent,const QString&& nameObject,const QString&& text)
{
    Q_ASSERT(parent != nullptr);
    auto combo = parent->findChild<QComboBox*>(nameObject);
    if(combo != nullptr) combo->setToolTip(text);
}

template<>
void WidgetsFactory<QToolButton>::setText(QObject* parent,const QString&& nameObject,const QString&& text)
{
    Q_ASSERT(parent != nullptr);
    auto toolButton = parent->findChild<QToolButton*>(nameObject);
    if(toolButton != nullptr) toolButton->setText(text);
}

template<>
void WidgetsFactory<QPushButton>::setText(QObject* parent,const QString&& nameObject,const QString&& text)
{
    Q_ASSERT(parent != nullptr);
    auto pushButton = parent->findChild<QPushButton*>(nameObject);
    if(pushButton != nullptr) pushButton->setText(text);//WindowTitle(text);
}

template<class T>
void WidgetsFactory<T>::setChecked(QObject* parent,const QString&& nameObject, const bool b)
{
    Q_UNUSED(parent);
    Q_UNUSED(nameObject);
    Q_UNUSED(b);
}

template<>
void WidgetsFactory<QAction>::setChecked(QObject* parent,const QString&& nameObject, const bool b)
{
    Q_ASSERT(parent != nullptr);
    auto act = parent->findChild<QAction*>(nameObject);
    if(act != nullptr) act->setChecked(b);
}

template<class T>
bool WidgetsFactory<T>::isChecked(QObject* parent,const QString&& nameObject)
{
    Q_UNUSED(parent);
    Q_UNUSED(nameObject);
    return false;
}

template<>
bool WidgetsFactory<QAction>::isChecked(QObject* parent,const QString&& nameObject)
{
    Q_ASSERT(parent != nullptr);
    auto act = parent->findChild<QAction*>(nameObject);
    if(act != nullptr) return act->isChecked();
    return false;
}
