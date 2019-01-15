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

#include <stdint.h>

namespace Narrowband {

class NarrowbandCore;

/**
 * FunctionConfig captures flags and defaults for Narrowband class
 */
struct FunctionConfig {
    // Reboot module at initialization
    bool    b_reboot_at_init = false;

    // in Narrowband.begin(), disable functionality and enable directly.
    bool    b_disable_enable_at_begin = false;
};

/**
 * Narrowband offers high level methods to interact with 
 * a modem. Typical approach is to begin a session, attach
 * to the network, send/receive data.
 * It uses methods from Narrowband core.
 */
class Narrowband {
public:
    Narrowband(NarrowbandCore& nb_, FunctionConfig& config_);

    /**
     * Enables module functionality
     */
    void begin();

    /**
     * Disables module functionality
     */
    void end();

    /**
     * Checks if module is in attention and enabled
     */
    operator bool();

    /**
     * Attaches to the narrowband network, according to module/band setup. If timeout is > 0, this method
     * is synchronous, waits for successful attachment and returns within maximum timeout specified.
     * If timeout_msec <= 0, attach calls are triggered but not waited for successful completion (asynchronous)
     * @return successful attachment (sychronous) or error-free triggering (asynchronous).
     */
    bool attach(unsigned long timeout_msec = 5000, unsigned long wait_time_msec = 2000);

    /**
     * Checks for Registration, Connection and Attachment. Returns true, if all are valid, false otherwise.
     */
    bool isAttached();

    /**
     * detatches from the nb network.
     */
    bool detach(long timeout_msec = 5000);

    /**
     * Checks for current operator selection, switches if necessary.
     * Module must be registered for automatic provider selection.
     */
    bool ensureOperatorSelected(String op);

    /**
     * Checks for current operator selection, switches to automatic if necessary.
     */
    bool ensureAutomaticOperatorSelection(void);

    /**
     * Sends UDP packet
     */
    bool sendUDP( const char *ip, const int port, const uint8_t *p_data, const size_t sz_data);
    bool sendUDP( const char *ip, const int port, String content);

    /**
     * Sends UDP packet and waits for reply
     */
    bool sendReceiveUDP( const char *ip, const int port, 
        const uint8_t *p_data, const size_t sz_data,
        uint8_t *p_response_data, const size_t sz_response_data,
        const long timeout_msec = 5000);
    bool sendReceiveUDP( const char *ip, const int port, 
        String request, String& response,
        const size_t sz_receive_bufsize = 128,
        const long timeout_msec = 5000);

    /**
     */
    void notify_status(const char *l);

    NarrowbandCore& getCore() const { return core_driver; };

private:
    void cb_attach_messages(const char *p, const void *ctx);

protected:
    int lastnotified_cereg, lastnotified_cscon, lastnotified_cgatt; 
    NarrowbandCore& core_driver;
    FunctionConfig& config;   
};

}
