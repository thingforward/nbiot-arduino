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

#include <stddef.h>

namespace Narrowband {

/**
 * Abstract class `CommandAdapter`, used by upper layer classes to execute
 * commands, do i/o with underlying communication mechanisms (e.g. serial or spi)
 */
class CommandAdapter {

public:

    /**
     * Sends a command via modem_serial
     * Does not wait for a response nor capture the response
     */
    virtual void send_cmd(const char *cmd) = 0;

    /**
     * Sends `cmd` to `modem_serial`. Waits for up to `timeout` msecs for a response
     * If response contains `reply`, returns true, false otherwise.
     */
    virtual bool send_cmd_waitfor_reply(const char *cmd, const char *reply) = 0;
   
    /**
     * Sends `cmd` to `modem_serial`. Waits for up to `timeout` msecs for a response. Response is captured
     * in `replybuffer` of size `sz_replybuffer.
     */
    virtual size_t send_cmd_recv_reply(const char *cmd, char *replybuffer, size_t sz_replybuffer) = 0;
   
    /**
     * Sends `cmd` to `modem_serial`. Waits for up to `timeout` msecs for a response OR `stopWord` appears in response.
     * Response is captured in `replybuffer` of size `sz_replybuffer.
     */
    virtual size_t send_cmd_recv_reply_stop(const char *cmd, char *replybuffer, size_t sz_replybuffer, const char *stopWord) = 0;
   
    /** Sets the timeout [msec] */
    void setTimeout(unsigned long timeout);

    /** Returns current timeout [msec] */
    unsigned long getTimeout() const;

protected:
    unsigned long    timeout;
};

inline void CommandAdapter::setTimeout(unsigned long timeout) {
    if (timeout > 0) {
        this->timeout = timeout;
    }
}

inline unsigned long CommandAdapter::getTimeout() const {
    return timeout;
}


}
 