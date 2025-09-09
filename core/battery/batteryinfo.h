    #ifndef BATTERYINFO_H
    #define BATTERYINFO_H

    #include <QObject>
    #include <QString>
    #include <windows.h>
    #include <winnt.h>

    class BatteryInfo : public QObject {
    Q_OBJECT
    public:
        explicit BatteryInfo(QObject *parent = nullptr);
        QString getBatteryOperatingTime(); // НОВАЯ ФУНКЦИЯ
        QString getPowerType();
        QString getBatteryChemistry();
        QString getBatteryLevel();
        QString getPowerSavingMode();
        QString getBatteryTime();
        QString getEstimatedTime();
        bool enterSleepMode();
        bool enterHibernateMode();

    private:
        bool enablePrivilege(LPCTSTR privilegeName);
        SYSTEM_POWER_STATUS powerStatus;
        qint64 batteryStartTime; // Время начала работы от батареи (в мс)
        bool wasOnBattery;       // Флаг, что работали от батареи
        void updatePowerStatus();
    };

    #endif // BATTERYINFO_H