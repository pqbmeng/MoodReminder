#ifndef REMINDERWIDGET_H
#define REMINDERWIDGET_H
#include <QWidget>
class ReminderWidget : public QWidget
{
    Q_OBJECT

public:
    ReminderWidget();
    ~ReminderWidget();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    void initSystemTray();
};

#endif // REMINDERWIDGET_H
