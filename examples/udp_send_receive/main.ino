/*
 *  Copyright (C) 2018  Digital Incubation & Growth GmbH
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *  This software is dual-licensed. For commercial licensing options, please
 *  contact the authors (see README).
 */

#include <Arduino.h>
#include <serialcmds.h>
#include <narrowbandcore.h>
#include <narrowband.h>


// Select serial object for modem communication
// Please look at the specs of your board and locate
// the Serial object with a pin mapping compatible to
// the shield you're using.
HardwareSerial& modem_serial = Serial1;

void setup() {
    // connection speed to your terminal (e.g. via USB)
    Serial.begin(115200);

    Serial.print("Hit [ENTER] or wait 10s ");
    Serial.setTimeout(10000);
    Serial.readStringUntil('\n');

    // Begin modem serial communication with correct speed (check shield specs!)
    // TEKMODUL BC68-DEVKIT         9600,echo
    modem_serial.begin(9600);

    delay(3000);

    // Instantiate command adapter as the connection via serial
    Narrowband::ArduinoSerialCommandAdapter ca(modem_serial);

    // Driver class
    Narrowband::NarrowbandCore nbc(ca);

    Narrowband::FunctionConfig cfg;

    // instantiate NB object. 
    Narrowband::Narrowband nb(nbc, cfg);
 
    // Quectel: wait at least 10 seconds
    delay(10*1000);

    // begin session
    nb.begin();
    if (!nb) {
        Serial.println("Error initializing NB module.");
        return;
    } else {
        Serial.println("NB module Initialized.");
    }

    nb.getCore().setReportError(true);
    nb.ensureAutomaticOperatorSelection();

    // try to attach within 15 secs
    if (nb.attach(15000)) {
        Serial.println("Attached.");

        String ip;
        if (nb.getCore().getPDPAddress(ip)) {
            Serial.println(ip);
        }

        // request something. Put in your IP address and 
        // request data.
        String req("hello world"), resp;
        if ( nb.sendReceiveUDP("192.168.0.1", 9876,req,resp)) {
            Serial.println(resp);
        }

        nb.detach();
        
    } else {
        Serial.println("unable to attach to NB network.");
    }

    nb.end();
}

void loop() {
}

