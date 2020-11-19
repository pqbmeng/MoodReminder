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
    QPropertyAnimation* shakeWindow(QWidget* w, const bool start = true)
    {
        QPropertyAnimation*pAnimation = new QPropertyAnimation(w, "pos");
        pAnimation->setDuration(500);
        pAnimation->setLoopCount(3);
        const auto wGeom = w->geometry();
        constexpr auto offset = 32;
        pAnimation->setKeyValueAt(0, QPoint(wGeom.x() - 3, wGeom.y() - 3));
        pAnimation->setKeyValueAt(0.1, QPoint(wGeom.x() + offset, wGeom.y() + offset));
        pAnimation->setKeyValueAt(0.2, QPoint(wGeom.x() - offset, wGeom.y() + offset));
        pAnimation->setKeyValueAt(0.3, QPoint(wGeom.x() + offset, wGeom.y() - offset));
        pAnimation->setKeyValueAt(0.4, QPoint(wGeom.x() - offset, wGeom.y() - offset));
        pAnimation->setKeyValueAt(0.5, QPoint(wGeom.x() + offset, wGeom.y() + offset));
        pAnimation->setKeyValueAt(0.6, QPoint(wGeom.x() - offset, wGeom.y() + offset));
        pAnimation->setKeyValueAt(0.7, QPoint(wGeom.x() + offset, wGeom.y() - offset));
        pAnimation->setKeyValueAt(0.8, QPoint(wGeom.x() - offset, wGeom.y() - offset));
        pAnimation->setKeyValueAt(0.9, QPoint(wGeom.x() + offset, wGeom.y() + offset));
        pAnimation->setKeyValueAt(1, QPoint(wGeom.x() - 3, wGeom.y() - 3));
        if (start)
        {
            pAnimation->start(QAbstractAnimation::DeleteWhenStopped);
        }
        return pAnimation;
    }

    // 窗口渐渐透明
    QPropertyAnimation* opacityWindow(QWidget* w, const bool start = true)
    {
        QPropertyAnimation *pAnimation = new QPropertyAnimation(w, "windowOpacity");
        pAnimation->setDuration(2000);
        pAnimation->setKeyValueAt(0, 1);
        pAnimation->setKeyValueAt(0.1, 0.7);
        pAnimation->setKeyValueAt(0.2, 0.5);
        pAnimation->setKeyValueAt(0.3, 0.7);
        pAnimation->setKeyValueAt(0.4, 1);
        pAnimation->setKeyValueAt(0.5, 0.7);
        pAnimation->setKeyValueAt(0.6, 0.5);
        pAnimation->setKeyValueAt(0.7, 0.7);
        pAnimation->setKeyValueAt(0.8, 1);
        pAnimation->setKeyValueAt(0.9, 0.5);
        pAnimation->setKeyValueAt(1, 0);
        if (start)
        {
            pAnimation->start(QAbstractAnimation::DeleteWhenStopped);
        }
        return pAnimation;
    }

    // 窗口下坠
    QPropertyAnimation* dropWindow(QWidget* w, const bool start = true)
    {
        QPropertyAnimation *pAnimation = new QPropertyAnimation(w, "geometry");
        QDesktopWidget *pDesktopWidget = QApplication::desktop();
        int x = (pDesktopWidget->availableGeometry().width() - w->width()) / 2;
        int y = (pDesktopWidget->availableGeometry().height() - w->height()) / 2;
        pAnimation->setDuration(1000);
        pAnimation->setStartValue(QRect(x, 0, w->width(), w->height()));
        pAnimation->setEndValue(QRect(x, y, w->width(), w->height()));
        pAnimation->setEasingCurve(QEasingCurve::OutElastic);
        if (start)
        {
            pAnimation->start(QAbstractAnimation::DeleteWhenStopped);
        }
        return pAnimation;
    }

    using namespace std::literals::chrono_literals;
    const auto intervalTime = 8min;
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
    QTimer::singleShot(std::chrono::milliseconds(::intervalTime).count(), this, &QWidget::show);

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

    const auto pAnimation = ::dropWindow(this, false);
    connect(pAnimation, &QAbstractAnimation::finished, [this]
    {
        ::shakeWindow(this);
        ::opacityWindow(this);
    });
    pAnimation->start(QAbstractAnimation::DeleteWhenStopped);

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
