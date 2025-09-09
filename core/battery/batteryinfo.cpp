#include "batteryinfo.h"
#include <windows.h>
#include <powrprof.h>
#include <setupapi.h>
#include <batclass.h>
#include <devguid.h>
#include <winnt.h>
#include <QDateTime>

BatteryInfo::BatteryInfo(QObject *parent) : QObject(parent) {
    updatePowerStatus();

    if (powerStatus.ACLineStatus == 0) {
        batteryStartTime = QDateTime::currentMSecsSinceEpoch();
        wasOnBattery = true;
    }
}

void BatteryInfo::updatePowerStatus() {
    if (!GetSystemPowerStatus(&powerStatus)) {
        qWarning("Ошибка получения статуса питания");
    }

    // Автоматически сбрасываем таймер при подключении к зарядке
    if (powerStatus.ACLineStatus == 1 && wasOnBattery) {
        batteryStartTime = 0;
        wasOnBattery = false;
    }

    // Автоматически запускаем таймер при отключении от зарядки
    if (powerStatus.ACLineStatus == 0 && !wasOnBattery) {
        batteryStartTime = QDateTime::currentMSecsSinceEpoch();
        wasOnBattery = true;
    }
}
    bool BatteryInfo::enablePrivilege(LPCTSTR privilegeName) {
        HANDLE hToken;
        TOKEN_PRIVILEGES tokenPriv;
        LUID luidPriv;
        BOOL bRet;

        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
            return false;
        }

        if (!LookupPrivilegeValue(NULL, privilegeName, &luidPriv)) {
            CloseHandle(hToken);
            return false;
        }

        tokenPriv.PrivilegeCount = 1;
        tokenPriv.Privileges[0].Luid = luidPriv;
        tokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        bRet = AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, 0, NULL, NULL);
        CloseHandle(hToken);
        return bRet;
    }

    QString BatteryInfo::getPowerType() {
        updatePowerStatus();
        if (powerStatus.ACLineStatus == 1) {
            return "Подключен к сети";
        } else if (powerStatus.ACLineStatus == 0) {
            return "Работа от батареи";
        }
        return "Неизвестно";
    }

    QString BatteryInfo::getBatteryLevel() {
        updatePowerStatus();
        if (powerStatus.BatteryLifePercent == 255) {
            return "N/A";
        }
        return QString("%1%").arg(powerStatus.BatteryLifePercent);
    }


//powerStatus.BatteryLifeTime обновляется не сразу после отключения от зарядки
QString BatteryInfo::getBatteryTime() {
    updatePowerStatus();

    // Если подключено к зарядке
    if (powerStatus.ACLineStatus == 1) {
        return "00:00:00 (на зарядке)";
    }

    // Если время неизвестно, но мы на батарее - показываем 00:00
    if (powerStatus.BatteryLifeTime == (DWORD)-1) {
        return "00:00 (только отключились)";
    }

    int seconds = powerStatus.BatteryLifeTime;
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;

    return QString("%1:%2")
            .arg(hours, 2, 10, QLatin1Char('0'))
            .arg(minutes, 2, 10, QLatin1Char('0'));
}

//время работы аккумулятора с момента отключения от зарядки !!??
    QString BatteryInfo::getEstimatedTime() {
    updatePowerStatus();

    // Если подключено к зарядке - сбрасываем таймер
    if (powerStatus.ACLineStatus == 1) {
        if (wasOnBattery) {
            batteryStartTime = 0;
            wasOnBattery = false;
        }
        return "00:00:00 (на зарядке)";
    }

    // Если работаем от батареи
    if (powerStatus.ACLineStatus == 0) {
        // Запускаем таймер, если он еще не запущен
        if (batteryStartTime == 0) {
            batteryStartTime = QDateTime::currentMSecsSinceEpoch();
            wasOnBattery = true;
        }

        // Вычисляем прошедшее время
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        qint64 elapsedMillis = currentTime - batteryStartTime;

        // Конвертируем в часы, минуты, секунды
        int totalSeconds = elapsedMillis / 1000;
        int hours = totalSeconds / 3600;
        int minutes = (totalSeconds % 3600) / 60;
        int seconds = totalSeconds % 60;

        return QString("%1:%2:%3")
                .arg(hours, 2, 10, QLatin1Char('0'))
                .arg(minutes, 2, 10, QLatin1Char('0'))
                .arg(seconds, 2, 10, QLatin1Char('0'));
    }

    return "Неизвестно";
    }

    QString BatteryInfo::getBatteryChemistry() {
        HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_BATTERY, NULL, NULL,
                                                DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
        if (hDevInfo == INVALID_HANDLE_VALUE) {
            return "N/A";
        }

        SP_DEVICE_INTERFACE_DATA devInterfaceData;
        devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        if (!SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVCLASS_BATTERY, 0, &devInterfaceData)) {
            SetupDiDestroyDeviceInfoList(hDevInfo);
            return "N/A";
        }

        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInterfaceData, NULL, 0, &requiredSize, NULL);
        PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc(requiredSize);
        detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInterfaceData, detailData, requiredSize, NULL, NULL)) {
            free(detailData);
            SetupDiDestroyDeviceInfoList(hDevInfo);
            return "N/A";
        }

        HANDLE hBattery = CreateFile(detailData->DevicePath, GENERIC_READ | GENERIC_WRITE,
                                     FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        free(detailData);
        SetupDiDestroyDeviceInfoList(hDevInfo);
        if (hBattery == INVALID_HANDLE_VALUE) {
            return "N/A";
        }

        DWORD batteryTag;
        if (!DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_TAG, NULL, 0, &batteryTag, sizeof(batteryTag), NULL, NULL)) {
            CloseHandle(hBattery);
            return "N/A";
        }

        BATTERY_QUERY_INFORMATION bqi;
        bqi.BatteryTag = batteryTag;
        bqi.InformationLevel = BatteryInformation;
        BATTERY_INFORMATION bi;
        DWORD bytesReturned;
        if (DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_INFORMATION, &bqi, sizeof(bqi), &bi, sizeof(bi),
                            &bytesReturned, NULL)) {
            CloseHandle(hBattery);
            QString chemStr = QString::fromLatin1((char *) bi.Chemistry, 4);
            if (chemStr == "LION") return "Lithium-Ion";
            if (chemStr == "PbAc") return "Lead Acid";
            if (chemStr == "NiCd") return "Nickel Cadmium";
            if (chemStr == "NiMH") return "Nickel Metal Hydride";
            return chemStr;
        }

        CloseHandle(hBattery);
        return "N/A";
    }

    QString BatteryInfo::getPowerSavingMode() {
        switch (powerStatus.SystemStatusFlag) {
            case 0:
                return "Режим энергосбережения выключен";
            case 1:
                return "Режим энергосбережения включен";
            default:
                return "Неизвестно";
        }
    }

    bool BatteryInfo::enterSleepMode() {
        if (!enablePrivilege(SE_SHUTDOWN_NAME)) {
            return false;
        }
        return SetSuspendState(FALSE, FALSE, FALSE);
    }

    bool BatteryInfo::enterHibernateMode() {
        if (!enablePrivilege(SE_SHUTDOWN_NAME)) {
            return false;
        }
        return SetSuspendState(TRUE, FALSE, FALSE);
    }
