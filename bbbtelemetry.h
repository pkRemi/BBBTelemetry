#ifndef BBBTELEMETRY_H
#define BBBTELEMETRY_H

#include <QMainWindow>
#include "hid_pnp.h"

namespace Ui {
class BBBTelemetry;
}

class BBBTelemetry : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit BBBTelemetry(QWidget *parent = 0);
    ~BBBTelemetry();
    
private:
    Ui::BBBTelemetry *ui;
    HID_PnP *plugNPlay;

public slots:
    void update_gui(bool isConnected, bool isPressed, int potentiometerValue, int potentiometerValue2);

signals:
    void toggle_leds_button_pressed();

private slots:
    void on_pushButton_clicked();
};

#endif // BBBTELEMETRY_H
