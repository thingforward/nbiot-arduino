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

#include "serialcmds.h"
#include "nbdbg.h"
#include <Arduino.h>

namespace Narrowband {

#define DEBUG_WS    16

/**
 * Sends a command via modem_serial
 * Does not wait for a response nor capture the response
 */
void ArduinoSerialCommandAdapter::send_cmd(const char *cmd) {
    dbg_out('>', cmd);
    clear();
    modem_serial.println(cmd);
}

/**
 * Sends `cmd` to `modem_serial`. Waits for up to `timeout` msecs for a response
 * If response contains `reply`, returns true, false otherwise.
 * Max buffer size if 256 here!
 */
bool ArduinoSerialCommandAdapter::send_cmd_waitfor_reply(const char *cmd, const char *reply) {
    if ( cmd == NULL || reply == NULL) {
        return 0;
    }
    uint8_t idx = 0;
    int stopWordLen = strlen(reply);

    const int sz_replybuffer = 256;
    char replybuffer[sz_replybuffer];
    char *pw = replybuffer;
    char *pd = replybuffer;
    unsigned long timer = millis();
    unsigned long t0 = timer;

    // clear out reply buffer
    memset(replybuffer, 0, sz_replybuffer);

    if(strlen(cmd) > 0) {
        dbg_out('>', cmd);
        modem_serial.println(cmd);
    }

    // while we're below timeout, and did not reach end of output buffer ...
    while ( timer-t0 < timeout && idx < sz_replybuffer) {
        // read what's there...
        int av = modem_serial.available();
        if (av > 0) {
            for ( int i = 0; i < av; i++) {
                int c = modem_serial.read();
                if ( c >= 0) {
                    *pw = c;

                    idx++; pw++;

                    // check for stop word. return if found, debug.
                    if ( idx >= stopWordLen) {
                        char *ps = pw-stopWordLen;

                        if (memcmp(ps, reply, stopWordLen) == 0) {
                            dbg_outs('>', (const char*)pd, (pw-pd), '!' );
                            return true;
                        }
                    }

#if !defined(ARDUINO_ARCH_AVR)
                    // debug every 16 bytes read UNLESS we're on AVR (not fast enough,
                    // will miss out bytes when read from a software serial)
                    if ( idx % DEBUG_WS == 0) {
                        dbg_outs('<', (const char*)pd, DEBUG_WS, ' ' );
                        pd += DEBUG_WS;
                    }
#endif
                }
            }
        }

        timer = millis();
    }
    // debug the rest..
    dbg_outs('>', (const char*)pd, (pw-pd), ' ' );

    return false;

}

/**
 * Sends `cmd` to `modem_serial`. Waits for up to `timeout` msecs for a response. Response is captured
 * in `replybuffer` of size `sz_replybuffer.
 */
size_t ArduinoSerialCommandAdapter::send_cmd_recv_reply(const char *cmd, char *replybuffer, size_t sz_replybuffer) {
    if ( cmd == NULL || replybuffer == NULL || sz_replybuffer <= 0) {
        return 0;
    }
    uint8_t idx = 0;

    char *pw = replybuffer;
    char *pd = replybuffer;
    unsigned long timer = millis();
    unsigned long t0 = timer;

    // clear out reply buffer
    memset(replybuffer, 0, sz_replybuffer);

    if(strlen(cmd) > 0) {
        dbg_out('>', cmd);
        modem_serial.println(cmd);
    }

    // while we're below timeout, and did not reach end of output buffer ...
    while ( timer-t0 < timeout && idx < sz_replybuffer) {
        // read what's there...
        int av = modem_serial.available();
        if (av > 0) {
            for ( int i = 0; i < av; i++) {
                int c = modem_serial.read();
                if ( c >= 0) {
                    *pw = c;

                    idx++; pw++;

#if !defined(ARDUINO_ARCH_AVR)
                    // debug every 16 bytes read UNLESS we're on AVR (not fast enough,
                    // will miss out bytes when read from a software serial)
                    if ( idx % DEBUG_WS == 0) {
                        dbg_outs('<', (const char*)pd, DEBUG_WS, ' ' );
                        pd += DEBUG_WS;
                    }
#endif
                }
            }
        }

        timer = millis();
    }
    // debug the rest..
    dbg_outs('>', (const char*)pd, (pw-pd), ' ' );

    return idx;
}

size_t ArduinoSerialCommandAdapter::send_cmd_recv_reply_stop(const char *cmd, char *replybuffer, size_t sz_replybuffer, const char *stopWord) {
    if ( cmd == NULL || replybuffer == NULL || sz_replybuffer <= 0 || stopWord == NULL) {
        return 0;
    }
    size_t stopWordLen = strlen(stopWord);
    size_t idx = 0;

    char *pw = replybuffer;
    char *pd = replybuffer;
    unsigned long timer = millis();
    unsigned long t0 = timer;

    // clear out reply buffer
    memset(replybuffer, 0, sz_replybuffer);

    dbg_out('>', cmd);
    modem_serial.println(cmd);

    // while we're below timeout, and did not reach end of output buffer ...
    while ( timer-t0 < timeout && idx < sz_replybuffer) {
        // read what's there...
        int av = modem_serial.available();
        if (av > 0) {
            for ( int i = 0; i < av; i++) {
                int c = modem_serial.read();
                if ( c >= 0) {
                    *pw = c;

                    idx++; pw++;

                    // check for stop word. return if found, debug.
                    if ( idx >= stopWordLen) {
                        char *ps = pw-stopWordLen;

                        if (memcmp(ps, stopWord, stopWordLen) == 0) {
                            dbg_outs('>', (const char*)pd, (pw-pd), '!' );
                            return idx;
                        }
                    }

#if !defined(ARDUINO_ARCH_AVR)
                    // debug every 16 bytes read UNLESS we're on AVR (not fast enough,
                    // will miss out bytes when read from a software serial)
                    if ( idx % DEBUG_WS == 0) {
                        dbg_outs('<', (const char*)pd, DEBUG_WS, ' ' );
                        pd += DEBUG_WS;
                    }
#endif
                }
            }
        }

        timer = millis();
    }
    // debug the rest..
    dbg_outs('>', (const char*)pd, (pw-pd), ' ' );

    return idx;
}

void ArduinoSerialCommandAdapter::clear() {
    while(modem_serial.available()){modem_serial.read();}
}


}

