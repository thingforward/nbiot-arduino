#ifndef __SERIALCMDS_H
#define __SERIALCMDS_H

#include <Arduino.h>
#include "commandadapter.h"

#define SERIALCMDS_DEFAULT_TIMEOUT_MSEC 1000


class ArduinoSerialCommandAdapter : public CommandAdapter {
    friend class NarrowbandCore;
public:
    ArduinoSerialCommandAdapter(Stream& modem_serial_,long timeout_ = SERIALCMDS_DEFAULT_TIMEOUT_MSEC ) : 
        modem_serial(modem_serial_) { setTimeout(timeout_); }

    /** Send `cmd` to serial, fire&forget style. Return immediately */
    void send_cmd(const char *cmd);

    bool send_cmd_waitfor_reply(const char *cmd, const char *reply);

    size_t send_cmd_recv_reply(const char *cmd, char *replybuffer, size_t sz_replybuffer);

    size_t send_cmd_recv_reply_stop(const char *cmd, char *replybuffer, size_t sz_replybuffer, const char *stopWord);

private:
    Stream&   modem_serial;
};


#endif