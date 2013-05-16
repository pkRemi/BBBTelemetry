#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <QObject>

class Telemetry
{
public:
    Telemetry();
    qint16 ax;
    qint16 ay;
    qint16 az;
    qint16 temp;
    qint16 gx;
    qint16 gy;
    qint16 gz;
    qint16 cx;
    qint16 cy;
    qint16 cz;

};

#endif // TELEMETRY_H
