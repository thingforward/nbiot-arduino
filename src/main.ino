#ifndef UNIT_TEST

#include <Arduino.h>
#include "serialcmds.h"
#include "narrowbandcore.h"


// Select serial object for modem communication
HardwareSerial& modem_serial = Serial1;

void mydelay(int t) {
    if ( t <= 1000) {
        delay(t);
        return;
    }
    int x = t/500;
    int v = 0;
    char sym[4] = { '-', '\\', '|', '/'};

    for ( int i = 0; i < x; i++) {
        Serial.print(sym[v]);

        v = (v + 1 ) % sizeof(sym);
        delay(500);
        Serial.print('\b');
    }
}

void setup() {
    Serial.begin(115200);

    mydelay(5000);

    // TEKMODUL BC68-DEVKIT         9600,echo
    modem_serial.begin(9600);

    ArduinoSerialCommandAdapter ca(modem_serial, 1000);

    Serial.println("Initializing..");
    while(!ca.send_cmd_waitfor_reply("AT", "OK\r\n")) {
        mydelay(1000);
    }
    Serial.println("Done.");

    NarrowbandCore nbc(ca);
    nbc.reboot();

    mydelay(15*1000);

    nbc.setEcho(false);

    Serial.println(nbc.getManufacturerIdentification());
    Serial.println(nbc.getModelIdentification());
    
    Serial.println(nbc.getIMEI());
    Serial.println(nbc.getIMSI());

    nbc.setModuleFunctionality(false);
    //mydelay(1000);
    nbc.setCDPServer("40.114.225.189");
    //mydelay(1000);
    nbc.setModuleFunctionality(true);
    //mydelay(1000);


/*

    // auto mode, can only be set when CFUN=0
    //nbc.setOperatorSelection(OperatorSelectMode::Automatic);

    mydelay(3000);
    int m,f;
    String opName;
    if ( nbc.getOperatorSelection(m,f,opName)) {
        if ( m == 0) {
            Serial.println("Operator selection: AUTOMATIC.");
            Serial.println(f);
            Serial.println(opName);
        }
        if ( m == 1) {
            Serial.println("Operator selection: MANUAL.");
            Serial.println(f);
            Serial.println(opName);
        }
        if ( m == 2) {
            Serial.println("Operator selection: DISABLED.");
        }
    }
    PDPContext  ctx_1nce = { -1, "IP", "iot.1nce.net" };
    bool b_ctx_ok = false;

    PDPContext  ctx[10];
    int num_contexts = nbc.getPDPContexts(&ctx[0], 10);
    if ( num_contexts > 0) {
        for ( int i = 0; i < num_contexts; i++) {
            char buf[128];
            sprintf(buf,"%d | %s | %s", ctx[i].cid, ctx[i].type, ctx[i].APN);
            Serial.println(buf);

            if ( ctx[i] == ctx_1nce) {
                b_ctx_ok = true;
            }
        }
    }

    if (b_ctx_ok) {
        Serial.println("Target context present.");
    } else {
        Serial.println("Target context NOT present, please configure.");

    }
*/

    mydelay(1000);
    nbc.setNetworkRegistration(1);
    mydelay(1000);
    nbc.setConnectionStatus(true);
    mydelay(1000);

    if ( nbc.setAttachStatus(true)) {
        bool attached = false;
        int counter = 0;
        while ( !attached && ++counter < 10) {
            mydelay(3000);
            if ( nbc.getAttachStatus(attached)) {
            }
        }
    }

    int rssi,ber;
    if ( nbc.getSignalQuality(rssi,ber)) {
        char buf[64];
        sprintf(buf, "RSSI=%d, BIT Error rate=%d", rssi, ber);
        Serial.println(buf);
    }

    mydelay(1000);

    if ( nbc.ready()) {
        int socket = nbc.createSocket(SocketType::Datagram, 17, 48762, true);
        if ( socket >= 0) { 
            mydelay(1000);

            String s("FF871D6BBDC5F86B?hello world");

            nbc.sendTo(socket, "40.114.225.189", 9876, s.length(), (const uint8_t*)s.c_str());
           
            int n = nbc.waitForMessageIndication(socket, 5000);
            if ( n > 0) {
                char buf[128];
                memset(buf,0,sizeof(buf));
                if ( nbc.recv(socket, buf, n, 5000)) {
                    Serial.println(buf);
                }
            }
            mydelay(1000);
            nbc.closeSocket(socket);
        }
    }
}



void loop() {


    if (modem_serial.available()) {
        int c = modem_serial.read();
        Serial.write(c);
    }
    
    if ( Serial.available()) {
        int c = Serial.read();

        // echo
        Serial.write(c);
        modem_serial.write(c);
    }
}


#endif