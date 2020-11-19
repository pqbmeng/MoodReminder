#include <chrono>

#include <QPainter>
#include <QTimer>
#include <QDesktopWidget>
#include <QApplication>
#include <QScreen>
#include <QList>
#include <QPropertyAnimation>
#include <QSystemTrayIcon>
#include <QMenu>

#include "ReminderWidget.h"

namespace
{
    // 窗口抖动
    void shakeWindow(QWidget* w)
    {
        QPropertyAnimation*pAnimation = new QPropertyAnimation(w, "pos");
        pAnimation->setDuration(500);
        pAnimation->setLoopCount(3);
        const auto wGeom = w->geometry();
        pAnimation->setKeyValueAt(0, QPoint(wGeom.x() - 3, wGeom.y() - 3));
        pAnimation->setKeyValueAt(0.1, QPoint(wGeom.x() + 6, wGeom.y() + 6));
        pAnimation->setKeyValueAt(0.2, QPoint(wGeom.x() - 6, wGeom.y() + 6));
        pAnimation->setKeyValueAt(0.3, QPoint(wGeom.x() + 6, wGeom.y() - 6));
        pAnimation->setKeyValueAt(0.4, QPoint(wGeom.x() - 6, wGeom.y() - 6));
        pAnimation->setKeyValueAt(0.5, QPoint(wGeom.x() + 6, wGeom.y() + 6));
        pAnimation->setKeyValueAt(0.6, QPoint(wGeom.x() - 6, wGeom.y() + 6));
        pAnimation->setKeyValueAt(0.7, QPoint(wGeom.x() + 6, wGeom.y() - 6));
        pAnimation->setKeyValueAt(0.8, QPoint(wGeom.x() - 6, wGeom.y() - 6));
        pAnimation->setKeyValueAt(0.9, QPoint(wGeom.x() + 6, wGeom.y() + 6));
        pAnimation->setKeyValueAt(1, QPoint(wGeom.x() - 3, wGeom.y() - 3));
        pAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }

    // 窗口渐渐透明
    void opacityWindow(QWidget* w)
    {
        QPropertyAnimation *pAnimation = new QPropertyAnimation(w, "windowOpacity");
        pAnimation->setDuration(3000);
        pAnimation->setKeyValueAt(0, 1);
        pAnimation->setKeyValueAt(0.5, 0);
        pAnimation->setKeyValueAt(1, 1);
        pAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }

    // 窗口下坠
    void dropWindow(QWidget* w)
    {
        QPropertyAnimation *pAnimation = new QPropertyAnimation(w, "geometry");
        QDesktopWidget *pDesktopWidget = QApplication::desktop();
        int x = (pDesktopWidget->availableGeometry().width() - w->width()) / 2;
        int y = (pDesktopWidget->availableGeometry().height() - w->height()) / 2;
        pAnimation->setDuration(1000);
        pAnimation->setStartValue(QRect(x, 0, w->width(), w->height()));
        pAnimation->setEndValue(QRect(x, y, w->width(), w->height()));
        pAnimation->setEasingCurve(QEasingCurve::OutElastic);
        pAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

ReminderWidget::ReminderWidget()
{
    setFixedSize(150, 150);
    setWindowFlags(Qt:: FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    initSystemTray();
}

ReminderWidget::~ReminderWidget()
{

}

void ReminderWidget::mousePressEvent(QMouseEvent *event)
{
    hide();

    QWidget::mousePressEvent(event);
}

void ReminderWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QPixmap mypixmap(":/face/mypic/smilingface.png");
    mypixmap = mypixmap.scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.fillRect(rect(), mypixmap);

    QWidget::paintEvent(event);
}

void ReminderWidget::hideEvent(QHideEvent *event)
{
    using namespace std::literals::chrono_literals;
    QTimer::singleShot(std::chrono::milliseconds(15min).count(), this, &QWidget::show);

    QWidget::hideEvent(event);
}

void ReminderWidget::showEvent(QShowEvent *event)
{
    auto myrect = rect();
    myrect.moveCenter(QGuiApplication::screens().at(0)->geometry().center());
    myrect.moveTop(0);
    setGeometry(myrect);

    using namespace std::literals::chrono_literals;
    QTimer::singleShot(std::chrono::milliseconds(3s).count(), this, &QWidget::hide);

    //::shakeWindow(this);
    ::opacityWindow(this);
    ::dropWindow(this);

    QWidget::showEvent(event);
}

void ReminderWidget::initSystemTray()
{
    auto sysTray = new QSystemTrayIcon;
    sysTray->setIcon(QIcon(":/face/mypic/smilingface.png"));
    sysTray->show();

    {
        auto menu = new QMenu;
        auto exitAction = new QAction("exit");
        connect(exitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
        menu->addAction(exitAction);
        sysTray->setContextMenu(menu);
    }
}
