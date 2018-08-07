#include <Arduino.h>
#include "serialcmds.h"
#include "narrowbandcore.h"


void setup() {
    delay(1500);

    Serial.begin(115200);


    // TEKMODUL BC68-DEVKIT         9600,echo
    Serial1.begin(9600);

    ArduinoSerialCommandAdapter ca(Serial1, 1000);

    Serial.println("Initializing..");
    while(!ca.send_cmd_waitfor_reply("AT", "OK\r\n")) {
        //ca.send_cmd("EASY+Mode");
        delay(1000);
    }
    Serial.println("Done.");



    NarrowbandCore nbc(ca);
    nbc.setEcho(false);

    Serial.println(nbc.getManufacturerIdentification());
    Serial.println(nbc.getModelIdentification());
    
    Serial.println(nbc.getIMEI());
    if ( nbc.hasError()) {
        Serial.println(nbc.getLastError());
    }


/*

    // enable if not yet so
    bool b;
    nbc.getModuleFunctionality(b);
    if (!b) {
        nbc.setModuleFunctionality(true);
    }

    // auto mode, can only be set when CFUN=0
    //nbc.setOperatorSelection(OperatorSelectMode::Automatic);

    delay(3000);
*/
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

/*
    int counter = 0;

    if ( nbc.setNetworkRegistration(1)) {
        int mode, status = 0;
        while( status == 0 && ++counter < 10) {
            if ( nbc.getNetworkRegistration(mode, status)) {
                char buf[64];
                sprintf(buf, "mode=%d, status=%d", mode, status);
                Serial.println(buf);
            }
            delay(2000);
        }
    }
    if ( nbc.setConnectionStatus(true)) {
        int urc; bool connected = false;
        while ( !connected  && ++counter < 10) {
            if ( nbc.getConnectionStatus(urc, connected)) {
                char buf[64];
                sprintf(buf, "URC_Enabled=%d, Connected=%d", urc, connected);
                Serial.println(buf);
            }
            delay(2000);
        }
    }
*/
/*
    if ( nbc.setAttachStatus(true)) {
        bool attached = false;
        while ( !attached && ++counter < 10) {
            if ( nbc.getAttachStatus(attached)) {
                Serial.println(attached?"ATTACH":"DETACH");
            }
        }
    }
*/
    delay(2000);

    int rssi,ber;
    if ( nbc.getSignalQuality(rssi,ber)) {
        char buf[64];
        sprintf(buf, "RSSI=%d, BIT Error rate=%d", rssi, ber);
        Serial.println(buf);
    }
}



void loop() {

    if (Serial1.available()) {
        int c = Serial1.read();
        Serial.write(c);
    }
    
    if ( Serial.available()) {
        int c = Serial.read();

        // echo
        Serial.write(c);
        Serial1.write(c);
    }
}

