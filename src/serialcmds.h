
#pragma once

#include <Arduino.h>
#include "commandadapter.h"

namespace Narrowband {

class NarrowbandCore;

#define SERIALCMDS_DEFAULT_TIMEOUT_MSEC 1000

/**
 * `ArduinoSerialCommandAdapter` is a CommandAdapter that communicates
 * with an underlying Arduino `Stream` object, which can be a SoftwareSerial or
 * HardwareSerial. 
 */
class ArduinoSerialCommandAdapter : public CommandAdapter {
    friend class NarrowbandCore;
public:
    ArduinoSerialCommandAdapter(Stream& modem_serial_,long timeout_ = SERIALCMDS_DEFAULT_TIMEOUT_MSEC ) : 
        modem_serial(modem_serial_) { setTimeout(timeout_); }

    void send_cmd(const char *cmd);

    bool send_cmd_waitfor_reply(const char *cmd, const char *reply);

    size_t send_cmd_recv_reply(const char *cmd, char *replybuffer, size_t sz_replybuffer);

    size_t send_cmd_recv_reply_stop(const char *cmd, char *replybuffer, size_t sz_replybuffer, const char *stopWord);

private:
    Stream&   modem_serial;

    void clear();
};

}

