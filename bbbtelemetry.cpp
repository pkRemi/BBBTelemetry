#include "bbbtelemetry.h"
#include "ui_bbbtelemetry.h"

BBBTelemetry::BBBTelemetry(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BBBTelemetry)
{
    ui->setupUi(this);
    plugNPlay = new HID_PnP();
    setupRealtimeDataDemo(ui->customPlot);
    connect(this, SIGNAL(toggle_leds_button_pressed()), plugNPlay, SLOT(toggle_leds()));
    connect(plugNPlay, SIGNAL(hid_comm_update(bool, bool, int, int, Telemetry)), this, SLOT(update_gui(bool, bool, int, int, Telemetry)));
}

BBBTelemetry::~BBBTelemetry()
{
    disconnect(this, SIGNAL(toggle_leds_button_pressed()), plugNPlay, SLOT(toggle_leds()));
    disconnect(plugNPlay, SIGNAL(hid_comm_update(bool, bool, int, int, Telemetry)), this, SLOT(update_gui(bool, bool, int, int, Telemetry)));
    delete ui;
    delete plugNPlay;
}

void BBBTelemetry::setupRealtimeDataDemo(QCustomPlot *customPlot)
{
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  QMessageBox::critical(this, "", "You're using Qt < 4.7, the realtime data demo needs functions that are available with Qt 4.7 to work properly");
#endif
  demoName = "Real Time Data Demo";

  // include this section to fully disable antialiasing for higher performance:
  /*
  customPlot->setNotAntialiasedElements(QCP::aeAll);
  QFont font;
  font.setStyleStrategy(QFont::NoAntialias);
  customPlot->xAxis->setTickLabelFont(font);
  customPlot->yAxis->setTickLabelFont(font);
  customPlot->legend->setFont(font);
  */
  customPlot->addGraph(); // blue line
  customPlot->graph(0)->setPen(QPen(Qt::blue));
  customPlot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
  customPlot->graph(0)->setAntialiasedFill(false);
  customPlot->addGraph(); // red line
  customPlot->graph(1)->setPen(QPen(Qt::red));
  customPlot->graph(0)->setChannelFillGraph(customPlot->graph(1));

  customPlot->addGraph(); // blue dot
  customPlot->graph(2)->setPen(QPen(Qt::blue));
  customPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(2)->setScatterStyle(QCP::ssDisc);
  customPlot->addGraph(); // red dot
  customPlot->graph(3)->setPen(QPen(Qt::red));
  customPlot->graph(3)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(3)->setScatterStyle(QCP::ssDisc);

  customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
  customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
  customPlot->xAxis->setAutoTickStep(false);
  customPlot->xAxis->setTickStep(2);
  customPlot->setupFullAxesBox();

  // make left and bottom axes transfer their ranges to right and top axes:
  connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

  // setup a timer that repeatedly calls BBBTelemetry::realtimeDataSlot:
  //connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
  //dataTimer.start(25); // Interval 0 means to refresh as fast as possible
}

void BBBTelemetry::realtimeDataSlot(int value0, int value1)
{
    // calculate two new data points:
  #if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
    double key = 0;
  #else
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
  #endif
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.01) // at most add point every 10 ms
    {
      //double value0 = sin(key*1.6+cos(key*1.7)*2)*10 + sin(key*1.2+0.56)*20 + 26;
      //double value1 = sin(key*1.3+cos(key*1.2)*1.2)*7 + sin(key*0.9+0.26)*24 + 26;
      // add data to lines:
      ui->customPlot->graph(0)->addData(key, value0);
      ui->customPlot->graph(1)->addData(key, value1);
      // set data of dots:
      ui->customPlot->graph(2)->clearData();
      ui->customPlot->graph(2)->addData(key, value0);
      ui->customPlot->graph(3)->clearData();
      ui->customPlot->graph(3)->addData(key, value1);
      // remove data of lines that's outside visible range:
      ui->customPlot->graph(0)->removeDataBefore(key-8);
      ui->customPlot->graph(1)->removeDataBefore(key-8);
      // rescale value (vertical) axis to fit the current data:
      ui->customPlot->graph(0)->rescaleValueAxis();
      ui->customPlot->graph(1)->rescaleValueAxis(true);
      lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    ui->customPlot->xAxis->setRange(key+0.25, 8, Qt::AlignRight);
    ui->customPlot->replot();

    // calculate frames per second:
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2) // average fps over 2 seconds
    {
      ui->statusBar->showMessage(
            QString("%1 FPS, Total Data points: %2")
            .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
            .arg(ui->customPlot->graph(0)->data()->count()+ui->customPlot->graph(1)->data()->count())
            , 0);
      lastFpsKey = key;
      frameCount = 0;
    }
  }

void BBBTelemetry::on_pushButton_clicked()
{
    emit toggle_leds_button_pressed();
}
void BBBTelemetry::update_gui(bool isConnected, bool isPressed, int potentiometerValue, int potentiometerValue2, Telemetry tele)
{
    QString sPotValue;
    QString sPotValue2;
    sPotValue = QString("%1").arg(potentiometerValue,4,16);
    sPotValue2 = QString("%1").arg(potentiometerValue2,4,16);
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
//        BBBTelemetry::realtimeDataSlot(potentiometerValue,potentiometerValue2);
        BBBTelemetry::realtimeDataSlot(tele.ax,tele.ay);

//        BBBTelemetry::realtimeDataSlot(10, 0 - 10);
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
