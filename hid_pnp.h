#ifndef HID_PNP_H
#define HID_PNP_H

#include <QObject>
#include <QTimer>
#include "../HIDAPI/hidapi.h"

#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "telemetry.h"

#define MAX_STR 65

class HID_PnP : public QObject
{
    Q_OBJECT
public:
    explicit HID_PnP(QObject *parent = 0);
    ~HID_PnP();

signals:
    void hid_comm_update(bool isConnected, bool isPressed, int potentiometerValue, int potentiometerValue2, Telemetry tele);

public slots:
    void toggle_leds();
    void toggle_motors();
    void PollUSB();

private:
    bool isConnected;
    bool pushbuttonStatus;
    bool toggleLeds;
    bool toggleMotors;
    qint16 potentiometerValue;
    qint16 potentiometerValue2;
    Telemetry tele;
    int t;
    int tt;
    hid_device *device;
    QTimer *timer;
    unsigned char buf[MAX_STR];

    void CloseDevice();
};

#endif // HID_PNP_H
