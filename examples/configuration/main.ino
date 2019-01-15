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

HardwareSerial& modem_serial = Serial1;      // Serial1

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
 
    // wait at least 10 seconds
    delay(10*1000);

    nb.getCore().setEcho(false);


    int bands[10];
    size_t num_bands = 0;


    // query currently activate bands

    nb.getCore().getBands(bands, 10, num_bands);

    Serial.println("Active Bands:");
    for ( size_t i = 0; i < num_bands; i++) {
        Serial.print(bands[i]); Serial.print(" ");
    }
    Serial.println();
    delay(1000);

    // query all bands module supports

    nb.getCore().getSupportedBands(bands, 10, num_bands);

    Serial.println("Supported Bands:");
    for ( size_t i = 0; i < num_bands; i++) {
        Serial.print(bands[i]); Serial.print(" ");
    }
    Serial.println();
    delay(1000);

    // enable all bands
    nb.getCore().setBands(bands, num_bands);
    delay(1000);

    // query again
    nb.getCore().getBands(bands, 10, num_bands);

    Serial.println("Active Bands:");
    for ( size_t i = 0; i < num_bands; i++) {
        Serial.print(bands[i]); Serial.print(" ");
    }
    Serial.println();


    // get configuration values
    String key("AUTOCONNECT");
    String v;
    if ( nb.getCore().getConfigValue(key, v)) {
        Serial.print(key); Serial.print( " is set to: ");
        Serial.println(v);
    } else {
        Serial.print(key); Serial.print( " not found.");
    }





    Serial.println("----- AT Command mode now ------");
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


