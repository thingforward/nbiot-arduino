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

#pragma once

#include "commandadapter.h"

class Stream;

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

