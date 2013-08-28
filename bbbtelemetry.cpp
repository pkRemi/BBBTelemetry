#include "bbbtelemetry.h"
#include "ui_bbbtelemetry.h"

BBBTelemetry::BBBTelemetry(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BBBTelemetry)
{
    ui->setupUi(this);
    plugNPlay = new HID_PnP();
    setupRealtimeDataDemo(ui->customPlot, ui->customPlotAccel, ui->customPlotGyro,ui->customPlotCompass);
    connect(this, SIGNAL(toggle_leds_button_pressed()), plugNPlay, SLOT(toggle_leds()));
    connect(this, SIGNAL(toggle_EnableMotorsButton_pressed()), plugNPlay, SLOT(toggle_motors()));
    connect(plugNPlay, SIGNAL(hid_comm_update(bool, bool, int, int, Telemetry)), this, SLOT(update_gui(bool, bool, int, int, Telemetry)));
    connect(this, SIGNAL(send_set_values(SetValues)), plugNPlay, SLOT(send_set_data(SetValues)));
}

BBBTelemetry::~BBBTelemetry()
{
    disconnect(this, SIGNAL(toggle_leds_button_pressed()), plugNPlay, SLOT(toggle_leds()));
    disconnect(this, SIGNAL(toggle_EnableMotorsButton_pressed()), plugNPlay, SLOT(toggle_motors()));
    disconnect(plugNPlay, SIGNAL(hid_comm_update(bool, bool, int, int, Telemetry)), this, SLOT(update_gui(bool, bool, int, int, Telemetry)));
    disconnect(this, SIGNAL(send_set_values(SetValues)), plugNPlay, SLOT(send_set_data(SetValues)));
    delete ui;
    delete plugNPlay;
}

void BBBTelemetry::setupRealtimeDataDemo(QCustomPlot *customPlot,
                                         QCustomPlot *customPlotAccel,
                                         QCustomPlot *customPlotGyro,
                                         QCustomPlot *customPlotCompass)
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
  customPlot->xAxis->setTickStep(8);
  customPlot->setupFullAxesBox();

  // make left and bottom axes transfer their ranges to right and top axes:
  connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

  // setup a timer that repeatedly calls BBBTelemetry::realtimeDataSlot:
  //connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
  //dataTimer.start(25); // Interval 0 means to refresh as fast as possible

  //********** Setup Accel plot, no dot only line but 3 axis
  customPlotAccel->addGraph(); // blue line
  customPlotAccel->graph(0)->setPen(QPen(Qt::blue));
  customPlotAccel->addGraph(); // red line
  customPlotAccel->graph(1)->setPen(QPen(Qt::red));
  customPlotAccel->addGraph(); // green line
  customPlotAccel->graph(2)->setPen(QPen(Qt::green));
  customPlotAccel->xAxis->setTickLabelType(QCPAxis::ltDateTime);
  customPlotAccel->xAxis->setDateTimeFormat("hh:mm:ss");
  customPlotAccel->xAxis->setAutoTickStep(false);
  customPlotAccel->xAxis->setTickStep(8);
  customPlotAccel->setupFullAxesBox();
  // make left and bottom axes transfer their ranges to right and top axes:
  connect(customPlotAccel->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlotAccel->xAxis2, SLOT(setRange(QCPRange)));
  connect(customPlotAccel->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlotAccel->yAxis2, SLOT(setRange(QCPRange)));

  //********** Setup Gyro plot, no dot only line but 3 axis
  customPlotGyro->addGraph(); // blue line
  customPlotGyro->graph(0)->setPen(QPen(Qt::blue));
  customPlotGyro->addGraph(); // red line
  customPlotGyro->graph(1)->setPen(QPen(Qt::red));
  customPlotGyro->addGraph(); // green line
  customPlotGyro->graph(2)->setPen(QPen(Qt::green));
  customPlotGyro->xAxis->setTickLabelType(QCPAxis::ltDateTime);
  customPlotGyro->xAxis->setDateTimeFormat("hh:mm:ss");
  customPlotGyro->xAxis->setAutoTickStep(false);
  customPlotGyro->xAxis->setTickStep(8);
  customPlotGyro->setupFullAxesBox();
  // make left and bottom axes transfer their ranges to right and top axes:
  connect(customPlotGyro->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlotGyro->xAxis2, SLOT(setRange(QCPRange)));
  connect(customPlotGyro->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlotGyro->yAxis2, SLOT(setRange(QCPRange)));

  //********** Setup Compass plot, no dot only line but 3 axis
  customPlotCompass->addGraph(); // blue line
  customPlotCompass->graph(0)->setPen(QPen(Qt::blue));
  customPlotCompass->addGraph(); // red line
  customPlotCompass->graph(1)->setPen(QPen(Qt::red));
  customPlotCompass->addGraph(); // green line
  customPlotCompass->graph(2)->setPen(QPen(Qt::green));
  customPlotCompass->xAxis->setTickLabelType(QCPAxis::ltDateTime);
  customPlotCompass->xAxis->setDateTimeFormat("hh:mm:ss");
  customPlotCompass->xAxis->setAutoTickStep(false);
  customPlotCompass->xAxis->setTickStep(8);
  customPlotCompass->setupFullAxesBox();
  // make left and bottom axes transfer their ranges to right and top axes:
  connect(customPlotCompass->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlotCompass->xAxis2, SLOT(setRange(QCPRange)));
  connect(customPlotCompass->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlotCompass->yAxis2, SLOT(setRange(QCPRange)));
}

void BBBTelemetry::realtimeDataSlot(int value0, int value1, Telemetry tele)
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
      ui->customPlot->graph(0)->removeDataBefore(key-32);
      ui->customPlot->graph(1)->removeDataBefore(key-32);
      // rescale value (vertical) axis to fit the current data:
      ui->customPlot->graph(0)->rescaleValueAxis();
      ui->customPlot->graph(1)->rescaleValueAxis(true);
      //lastPointKey = key;
      //***** Accel Plot ****************************************************/
      // add data to lines:
      ui->customPlotAccel->graph(0)->addData(key, tele.ax);
      ui->customPlotAccel->graph(1)->addData(key, tele.ay);
      ui->customPlotAccel->graph(2)->addData(key, tele.az);
      // remove data of lines that's outside visible range:
      ui->customPlotAccel->graph(0)->removeDataBefore(key-32);
      ui->customPlotAccel->graph(1)->removeDataBefore(key-32);
      ui->customPlotAccel->graph(2)->removeDataBefore(key-32);
      // rescale value (vertical) axis to fit the current data:
      ui->customPlotAccel->graph(0)->rescaleValueAxis();
      ui->customPlotAccel->graph(1)->rescaleValueAxis(true);
      ui->customPlotAccel->graph(2)->rescaleValueAxis(true);
      //***** Gyro Plot ****************************************************/
      // add data to lines:
      ui->customPlotGyro->graph(0)->addData(key, tele.gx);
      ui->customPlotGyro->graph(1)->addData(key, tele.gy);
      ui->customPlotGyro->graph(2)->addData(key, tele.gz);
      // remove data of lines that's outside visible range:
      ui->customPlotGyro->graph(0)->removeDataBefore(key-32);
      ui->customPlotGyro->graph(1)->removeDataBefore(key-32);
      ui->customPlotGyro->graph(2)->removeDataBefore(key-32);
      // rescale value (vertical) axis to fit the current data:
      ui->customPlotGyro->graph(0)->rescaleValueAxis();
      ui->customPlotGyro->graph(1)->rescaleValueAxis(true);
      ui->customPlotGyro->graph(2)->rescaleValueAxis(true);
      //***** Compass Plot ****************************************************/
      // add data to lines:
      ui->customPlotCompass->graph(0)->addData(key, tele.cx);
      ui->customPlotCompass->graph(1)->addData(key, tele.cy);
      ui->customPlotCompass->graph(2)->addData(key, tele.cz);
      // remove data of lines that's outside visible range:
      ui->customPlotCompass->graph(0)->removeDataBefore(key-32);
      ui->customPlotCompass->graph(1)->removeDataBefore(key-32);
      ui->customPlotCompass->graph(2)->removeDataBefore(key-32);
      // rescale value (vertical) axis to fit the current data:
      ui->customPlotCompass->graph(0)->rescaleValueAxis();
      ui->customPlotCompass->graph(1)->rescaleValueAxis(true);
      ui->customPlotCompass->graph(2)->rescaleValueAxis(true);
      lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    ui->customPlot->xAxis->setRange(key+0.25, 32, Qt::AlignRight);
    ui->customPlot->replot();
    //***** Accel Plot ****************************************************/
    ui->customPlotAccel->xAxis->setRange(key+0.25, 32, Qt::AlignRight);
    ui->customPlotAccel->replot();
    //***** Gyro Plot ****************************************************/
    ui->customPlotGyro->xAxis->setRange(key+0.25, 32, Qt::AlignRight);
    ui->customPlotGyro->replot();
    //***** Compass Plot ****************************************************/
    ui->customPlotCompass->xAxis->setRange(key+0.25, 32, Qt::AlignRight);
    ui->customPlotCompass->replot();

    // calculate frames per second:
//    static double lastFpsKey;
//    static int frameCount;
//    ++frameCount;
//    if (key-lastFpsKey > 2) // average fps over 2 seconds
//    {
//      ui->statusBar->showMessage(
//            QString("%1 FPS, Total Data points: %2")
//            .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
//            .arg(ui->customPlot->graph(0)->data()->count()+ui->customPlot->graph(1)->data()->count())
//            , 0);
//      lastFpsKey = key;
//      frameCount = 0;
//    }
  }

void BBBTelemetry::on_pushButton_clicked()
{
    ui->MessageWindow->appendPlainText("-> Toggle LED's");
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
        //ui->label_2->setEnabled(true);
        //ui->label_3->setEnabled(true);
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
        if(tele.motorStatus == 1)
            ui->labelMotorStatus->setText("Motors Enabled");
        else if(tele.motorStatus == 0)
           ui->labelMotorStatus->setText("Motors Disabled");
        else if (tele.motorStatus == 2)
            ui->labelMotorStatus->setText("Enabling Motors...");
        ui->progressBar->setValue(potentiometerValue);
        ui->PotValue->setText(sPotValue);
        ui->progressBar_2->setValue(potentiometerValue2);
        ui->PotValue_2->setText(sPotValue2);
//        BBBTelemetry::realtimeDataSlot(potentiometerValue,potentiometerValue2);
        BBBTelemetry::realtimeDataSlot(potentiometerValue, potentiometerValue2, tele);

//        BBBTelemetry::realtimeDataSlot(10, 0 - 10);
    }
    else
    {
        //ui->label_2->setEnabled(false);
        //ui->label_3->setEnabled(false);
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

void BBBTelemetry::on_pushButtonEnableMotors_clicked()
{
    ui->MessageWindow->appendPlainText("-> Toggle Motors");
    emit toggle_EnableMotorsButton_pressed();
}

void BBBTelemetry::on_pushButtonStartLogging_clicked()
{
    QString a = "Start Logging. Using file: ";
    a.append(ui->lineEditLogFileName->text());
    ui->MessageWindow->appendPlainText(a);
}

void BBBTelemetry::on_pushButtonSendValues_clicked()
{
    ui->MessageWindow->appendPlainText("-> Send Set Values:");
    QString a;
    SetValues b;
    b.code[0] = ui->pCode_1->text().toInt();
    b.code[1] = ui->pCode_2->text().toInt();
    b.code[2] = ui->pCode_3->text().toInt();
    b.code[3] = ui->pCode_4->text().toInt();
    b.code[4] = ui->pCode_5->text().toInt();
    b.code[5] = ui->pCode_6->text().toInt();
    b.value[0] = ui->pValue_1->text().toInt();
    b.value[1] = ui->pValue_2->text().toInt();
    b.value[2] = ui->pValue_3->text().toInt();
    b.value[3] = ui->pValue_4->text().toInt();
    b.value[4] = ui->pValue_5->text().toInt();
    b.value[5] = ui->pValue_6->text().toInt();
    a = ui->pLabel_1->text();
    a.append(" = ");
    a.append(ui->pValue_1->text());
    ui->MessageWindow->appendPlainText(a);
    a = ui->pLabel_2->text();
    a.append(" = ");
    a.append(ui->pValue_2->text());
    ui->MessageWindow->appendPlainText(a);
    a = ui->pLabel_3->text();
    a.append(" = ");
    a.append(ui->pValue_3->text());
    ui->MessageWindow->appendPlainText(a);
    a = ui->pLabel_4->text();
    a.append(" = ");
    a.append(ui->pValue_4->text());
    ui->MessageWindow->appendPlainText(a);
    a = ui->pLabel_5->text();
    a.append(" = ");
    a.append(ui->pValue_5->text());
    ui->MessageWindow->appendPlainText(a);
    a = ui->pLabel_6->text();
    a.append(" = ");
    a.append(ui->pValue_6->text());
    ui->MessageWindow->appendPlainText(a);
    emit send_set_values(b);
}
