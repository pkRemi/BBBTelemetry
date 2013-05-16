#ifndef BBBTELEMETRY_H
#define BBBTELEMETRY_H

#include <QMainWindow>
#include "hid_pnp.h"
#include "../QCustomPlot/qcustomplot.h" // the header file of QCustomPlot. Don't forget to add it to your project, if you use an IDE, so it gets compiled.
#include "telemetry.h"

namespace Ui {
class BBBTelemetry;
}

class BBBTelemetry : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit BBBTelemetry(QWidget *parent = 0);
    ~BBBTelemetry();
    
    void setupRealtimeDataDemo(QCustomPlot *customPlot);
private:
    Ui::BBBTelemetry *ui;
    HID_PnP *plugNPlay;
    QString demoName;
    QTimer dataTimer;

private slots:
    void realtimeDataSlot(int value0, int value1);

public slots:
    void update_gui(bool isConnected, bool isPressed, int potentiometerValue, int potentiometerValue2, Telemetry tele);

signals:
    void toggle_leds_button_pressed();

private slots:
    void on_pushButton_clicked();
};

#endif // BBBTELEMETRY_H
