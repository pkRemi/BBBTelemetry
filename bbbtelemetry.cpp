#include "bbbtelemetry.h"
#include "ui_bbbtelemetry.h"

BBBTelemetry::BBBTelemetry(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BBBTelemetry)
{
    ui->setupUi(this);
    plugNPlay = new HID_PnP();

    connect(this, SIGNAL(toggle_leds_button_pressed()), plugNPlay, SLOT(toggle_leds()));
    connect(plugNPlay, SIGNAL(hid_comm_update(bool, bool, int, int)), this, SLOT(update_gui(bool, bool, int, int)));
}

BBBTelemetry::~BBBTelemetry()
{
    disconnect(this, SIGNAL(toggle_leds_button_pressed()), plugNPlay, SLOT(toggle_leds()));
    disconnect(plugNPlay, SIGNAL(hid_comm_update(bool, bool, int, int)), this, SLOT(update_gui(bool, bool, int, int)));
    delete ui;
    delete plugNPlay;
}

void BBBTelemetry::on_pushButton_clicked()
{
    emit toggle_leds_button_pressed();
}
void BBBTelemetry::update_gui(bool isConnected, bool isPressed, int potentiometerValue, int potentiometerValue2)
{
    QString sPotValue;
    QString sPotValue2;
    sPotValue = QString("%1").arg(potentiometerValue);
    sPotValue2 = QString("%1").arg(potentiometerValue2);
    if(isConnected)
    {
        ui->label_2->setEnabled(true);
        ui->label_3->setEnabled(true);
        ui->pushButton->setEnabled(true);
        ui->pushbuttonStatus->setEnabled(true);
        ui->progressBar->setEnabled(true);
        ui->progressBar_2->setEnabled(true);

        ui->deviceConnectedStatus->setText("Device Found: AttachedState = TRUE");
        ui->statusBar->showMessage("Device Found: AttachedState = TRUE");
        if(isPressed)
            ui->pushbuttonStatus->setText("Pushbutton State: Pressed");
        else
            ui->pushbuttonStatus->setText("Pushbutton State: Not Pressed");

        ui->progressBar->setValue(potentiometerValue);
        ui->PotValue->setText(sPotValue);
        ui->progressBar_2->setValue(potentiometerValue2);
        ui->PotValue_2->setText(sPotValue2);
    }
    else
    {
        ui->label_2->setEnabled(false);
        ui->label_3->setEnabled(false);
        ui->pushButton->setEnabled(false);
        ui->pushbuttonStatus->setEnabled(false);
        ui->progressBar->setEnabled(false);
        ui->progressBar_2->setEnabled(false);

        ui->PotValue->setText("N/A");
        ui->PotValue_2->setText("N/A");

        ui->deviceConnectedStatus->setText("Device Not Detected: Verify Connection/Correct Firmware");
        ui->statusBar->showMessage("Device Not Detected: Verify Connection/Correct Firmware");
        ui->pushbuttonStatus->setText("Pushbutton State: Unknown");
        ui->progressBar->setValue(0);
        ui->progressBar_2->setValue(0);
    }
}
