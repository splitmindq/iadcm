#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QMovie>
#include <QPushButton>
#include <QVector>
#include "../battery/batteryinfo.h"
#include <QProgressBar>
#include <QGraphicsDropShadowEffect>

class CustomButton : public QPushButton {
Q_OBJECT
public:
    explicit CustomButton(const QString &text, QWidget *parent = nullptr);
    ~CustomButton();

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QGraphicsDropShadowEffect *m_shadowEffect;
};

class NewWindow : public QWidget {
Q_OBJECT
public:
    explicit NewWindow(int labNumber, QWidget *parent = nullptr);

private slots:
    void updatePowerInfo();
    void enterSleepMode();
    void enterHibernateMode();

private:
    BatteryInfo *batteryInfo;
    QLabel *powerTypeLabel;
    QLabel *batteryTypeLabel;
    QLabel *batteryLevelLabel;
    QLabel *powerSchemeLabel;
    QLabel *fullBatteryTimeLabel;
    QLabel *remainingBatteryTimeLabel;
    QProgressBar *batteryProgressBar;
};

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLabButtonClicked();

private:
    QLabel *gifLabel;
    QMovie *movie;
    QVector<CustomButton*> labButtons;
};

#endif // MAINWINDOW_H