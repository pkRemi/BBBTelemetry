#include "hid_pnp.h"

HID_PnP::HID_PnP(QObject *parent) :
    QObject(parent)
{
    isConnected = false;
    pushbuttonStatus = false;
    potentiometerValue = 0;
    potentiometerValue = 10;
    toggleLeds = 0;
    toggleMotors = false;
    newSetData = false;

    device = NULL;
    buf[0] = 0x00;
    buf[1] = 0x37;
    memset((void*)&buf[2], 0x00, sizeof(buf) - 2);

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(PollUSB()));

    timer->start(250);
}

HID_PnP::~HID_PnP() {
    disconnect(timer, SIGNAL(timeout()), this, SLOT(PollUSB()));
}


void HID_PnP::PollUSB()
{
    buf[0] = 0x00;
    memset((void*)&buf[2], 0x00, sizeof(buf) - 2);

    if (isConnected == false) {
        device = hid_open(0x04d8, 0x003f, NULL);

        if (device) {
            isConnected = true;
            hid_set_nonblocking(device, true);
            timer->start(15);
        }
    }
    else {
        /** USB codes *************************************************************
         *  Send buf[1] - Receive buf[0]
         *
         *  0x80 Toggle LED's (no data from remote expected)
         *  0x37 Pot values
         *  0x81 PushButton status
         *  0x82 Send Motor enable toggle
         *  0x83 Send control values to remote
         *  0x42 Telemetry data
         *  0x41 No telemetry to send (only from remote)
         **************************************************************************/
        if(toggleLeds == true) {
            toggleLeds = false;

            buf[1] = 0x80;

            if (hid_write(device, buf, sizeof(buf)) == -1)  //Write buffer to USB
            {
                CloseDevice();
                return;
            }

            buf[0] = 0x00;
            buf[1] = 0x37;
            memset((void*)&buf[2], 0x00, sizeof(buf) - 2);  // Clear buffer exept 0 and 1
        }
        if(toggleMotors == true) {
            toggleMotors = false;

            buf[1] = 0x82;                                  // Toggle motors enable

            if (hid_write(device, buf, sizeof(buf)) == -1)  //Write buffer to USB
            {
                CloseDevice();
                return;
            }

            buf[0] = 0x00;
            buf[1] = 0x37;
            memset((void*)&buf[2], 0x00, sizeof(buf) - 2);  // Clear buffer exept 0 and 1
        }
        if(newSetData == true) {
            newSetData = false;
            buf[1] = 0x83;
            tt=2;
            for (int dd = 0;dd<6;dd++) {
                buf[tt++] = setData.code[dd];
                buf[tt++] = setData.value[dd] >> 8;   // MSB
                buf[tt++] = setData.value[dd] & 0xff; // LSB
            }
            if (hid_write(device, buf, sizeof(buf)) == -1)  //Write buffer to USB
            {
                CloseDevice();
                return;
            }

            buf[0] = 0x00;
            buf[1] = 0x37;
            memset((void*)&buf[2], 0x00, sizeof(buf) - 2);  // Clear buffer exept 0 and 1
        }


        if (hid_write(device, buf, sizeof(buf)) == -1)      // Write buffer to request data
        {
            CloseDevice();
            return;
        }
        if(hid_read(device, buf, sizeof(buf)) == -1)        // Read data from USB
        {
            CloseDevice();
            return;
        }
        if(buf[0] == 0x37) {
            potentiometerValue = (buf[2]<<8) + buf[1];
            potentiometerValue2 = (buf[4]<<8) + buf[3];
            buf[1] = 0x81;
        }
        else if(buf[0] == 0x81) {
            pushbuttonStatus = (buf[1] == 0x00);
            buf[1] = 0x42;
        }

        else if(buf[0] == 0x42) {
            //**** Telemetry string format for command=0x42 ************************/
            //** command, axh, axl, ayh, ayl, azh, azl, th, tl,             8byte **/
            //** gxh, gxl, gyh, gyl, gzh, gzl, cxh, cxl, cyh, cyl, czh, czl,12byte**/
            //**   --Total 20byte                                                 **/
            tt = 1;
            tele.ax = (buf[tt]<<8) + buf[tt+1];
            tt = tt + 2;
            tele.ay = (buf[tt]<<8) + buf[tt+1];
            tt = tt + 2;
            tele.az = (buf[tt]<<8) + buf[tt+1];
            tt = tt + 2;
            tele.temp = (buf[tt]<<8) + buf[tt+1];
            tt = tt + 2;
            tele.gx = (buf[tt]<<8) + buf[tt+1];
            tt = tt + 2;
            tele.gy = (buf[tt]<<8) + buf[tt+1];
            tt = tt + 2;
            tele.gz = (buf[tt]<<8) + buf[tt+1];
            tt = tt + 2;
            tele.cx = (buf[tt]<<8) + buf[tt+1];
            tt = tt + 2;
            tele.cy = (buf[tt]<<8) + buf[tt+1];
            tt = tt + 2;
            tele.cz = (buf[tt]<<8) + buf[tt+1];
            tt = tt + 2;
            tele.motorStatus = buf[tt];
            buf[1] = 0x37;
            hid_comm_update(isConnected, pushbuttonStatus, potentiometerValue, potentiometerValue2, tele);

        }
        else if(buf[0] == 0x41) {
            //**** No new telemetry, do nothing. ****/
            buf[1] = 0x37;
        }
    }

    //hid_comm_update(isConnected, pushbuttonStatus, potentiometerValue, potentiometerValue2, tele);
}

void HID_PnP::toggle_leds() {
    toggleLeds = true;
}
void HID_PnP::toggle_motors(){
    toggleMotors = true;
}
void HID_PnP::send_set_data(SetValues data){
    setData = data;
    newSetData = true;
}

void HID_PnP::CloseDevice() {
    hid_close(device);
    device = NULL;
    isConnected = false;
    pushbuttonStatus = false;
    potentiometerValue = 0;
    potentiometerValue2 = 100;
    toggleLeds = 0;
    hid_comm_update(isConnected, pushbuttonStatus, potentiometerValue, potentiometerValue2, tele);
    timer->start(250);
}
