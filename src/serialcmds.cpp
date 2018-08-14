#include "serialcmds.h"

namespace Narrowband {

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

    uint8_t idx = 0;
    unsigned long timer = millis();
    bool b_reply_match = false;
    char replybuffer[256]; 
    
    memset(replybuffer, 0, sizeof(replybuffer));

    while (!b_reply_match && millis() - timer < timeout && idx < sizeof(replybuffer)) {
        if (modem_serial.available()) {
            int c = modem_serial.read();
            replybuffer[idx++] = c;

            b_reply_match = strstr(replybuffer, reply);
        }
    }

    dbg_out('<', replybuffer, ((b_reply_match)?'!':' ') );

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

    uint8_t idx = 0;
    unsigned long timer = millis();
    bool b_reply_match = false;

    while (!b_reply_match && millis() - timer < timeout && idx < sz_replybuffer) {
        if (modem_serial.available()) {
            int c = modem_serial.read();
            replybuffer[idx++] = c;
        }
    }
    replybuffer[idx] = '\0';

    dbg_out('<', replybuffer, ' ');

    return idx;
}

size_t ArduinoSerialCommandAdapter::send_cmd_recv_reply_stop(const char *cmd, char *replybuffer, size_t sz_replybuffer, const char *stopWord) {
    modem_serial.println(cmd);
    dbg_out('>', cmd);

    uint8_t idx = 0;
    unsigned long timer = millis();
    bool b_reply_match = false;

    while (!b_reply_match && millis() - timer < timeout && idx < sz_replybuffer) {
        if (modem_serial.available()) {
            int c = modem_serial.read();
            replybuffer[idx++] = c;

            if ( strstr(replybuffer, stopWord) != NULL) {
                replybuffer[idx] = '\0';
                dbg_out('<', replybuffer, ' ');
                return idx;
            }
        }
    }
    replybuffer[idx] = '\0';
    dbg_out('<', replybuffer, ' ');
    return idx;
}

}