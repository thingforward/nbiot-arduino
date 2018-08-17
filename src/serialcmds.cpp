#include "serialcmds.h"

namespace Narrowband {

#define DEBUG_WS    16

/**
 * Sends a command via modem_serial
 * Does not wait for a response nor capture the response
 */
void ArduinoSerialCommandAdapter::send_cmd(const char *cmd) {
    modem_serial.println(cmd);
    dbg_out('>', cmd);
}

/**
 * Sends `cmd` to `modem_serial`. Waits for up to `timeout` msecs for a response
 * If response contains `reply`, returns true, false otherwise.
 */
bool ArduinoSerialCommandAdapter::send_cmd_waitfor_reply(const char *cmd, const char *reply) {
    modem_serial.println(cmd);
    dbg_out('>', cmd);

    uint8_t idx = 0, lidx = 0;
    unsigned long timer = millis();
    bool b_reply_match = false;
    
    char replybuffer[256]; 
    memset(replybuffer, 0, sizeof(replybuffer));

    while (!b_reply_match && (millis() - timer < timeout) && (idx < sizeof(replybuffer))) {
        if (modem_serial.available()) {
            int c = modem_serial.read();
            if ( c <= 0) {
                c = 255;
            }
            replybuffer[idx++] = c;

            if ( idx % DEBUG_WS == 0) {
                dbg_outs('<', (const char*)&replybuffer[lidx], DEBUG_WS, ((b_reply_match)?'!':' ') );
                lidx = idx;
            }
        }
        if ( strstr(replybuffer, reply) != nullptr) {
            b_reply_match = true;
        }
    }

    dbg_outs('<', (const char*)&replybuffer[lidx], (idx-lidx), ((b_reply_match)?'!':' ') );

    return b_reply_match;
}

/**
 * Sends `cmd` to `modem_serial`. Waits for up to `timeout` msecs for a response. Response is captured
 * in `replybuffer` of size `sz_replybuffer.
 */
size_t ArduinoSerialCommandAdapter::send_cmd_recv_reply(const char *cmd, char *replybuffer, size_t sz_replybuffer) {
    if(strlen(cmd) > 0) {
        modem_serial.println(cmd);
        dbg_out('>', cmd);
    }

    uint8_t idx = 0, lidx = 0;
    unsigned long timer = millis();
    bool b_reply_match = false;

    while (!b_reply_match && millis() - timer < timeout && idx < sz_replybuffer) {
        if (modem_serial.available()) {
            int c = modem_serial.read();
            replybuffer[idx++] = c;

            if ( idx % DEBUG_WS == 0) {
                dbg_outs('<', (const char*)&replybuffer[lidx], DEBUG_WS, ' ' );
                lidx = idx;
            }
        }
    }
    replybuffer[idx] = '\0';

    dbg_outs('<', (const char*)&replybuffer[lidx], (idx-lidx), ' ' );
//    dbg_out('<', replybuffer, ' ');

    return idx;
}

size_t ArduinoSerialCommandAdapter::send_cmd_recv_reply_stop(const char *cmd, char *replybuffer, size_t sz_replybuffer, const char *stopWord) {
    modem_serial.println(cmd);
    dbg_out('>', cmd);

    uint8_t idx = 0, lidx = 0;
    unsigned long timer = millis();
    bool b_reply_match = false;

    while (!b_reply_match && millis() - timer < timeout && idx < sz_replybuffer) {
        if (modem_serial.available()) {
            int c = modem_serial.read();
            replybuffer[idx++] = c;

            if ( idx % DEBUG_WS == 0) {
                dbg_outs('<', (const char*)&replybuffer[lidx], DEBUG_WS, ((b_reply_match)?'!':' ') );
                lidx = idx;
            }

        }
        if ( strstr(replybuffer, stopWord) != nullptr) {
            replybuffer[idx] = '\0';
            dbg_outs('<', (const char*)&replybuffer[lidx], (idx-lidx), ' ' );
            return idx;
        }
    }
    replybuffer[idx] = '\0';
    dbg_outs('<', (const char*)&replybuffer[lidx], (idx-lidx), ' ' );
    return idx;
}

}