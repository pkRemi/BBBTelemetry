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
    
    void setupRealtimeDataDemo(QCustomPlot *customPlot,
                               QCustomPlot *customPlotAccel,
                               QCustomPlot *customPlotGyro,
                               QCustomPlot *customPlotCompass);
private:
    Ui::BBBTelemetry *ui;
    HID_PnP *plugNPlay;
    QString demoName;
    QTimer dataTimer;

private slots:
    void realtimeDataSlot(int value0, int value1, Telemetry tele);

public slots:
    void update_gui(bool isConnected, bool isPressed, int potentiometerValue, int potentiometerValue2, Telemetry tele);

signals:
    void toggle_leds_button_pressed();
    void toggle_EnableMotorsButton_pressed();
    void send_set_values(SetValues);

private slots:
    void on_pushButton_clicked();
    void on_pushButtonEnableMotors_clicked();
    void on_pushButtonStartLogging_clicked();
    void on_pushButtonSendValues_clicked();
};

#endif // BBBTELEMETRY_H
