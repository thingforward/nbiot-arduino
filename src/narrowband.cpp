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
#include "narrowband.h"
#include "narrowbandcore.h"

namespace Narrowband {

Narrowband::Narrowband(NarrowbandCore& nbc_, FunctionConfig& config_) : core_driver(nbc_), config(config_) {
    if (config.b_reboot_at_init) {
        core_driver.reboot();
    }
}

void Narrowband::begin() {
    while (!core_driver.ready()) {
        delay(1000);
    }
    core_driver.setEcho(false);
    if (config.b_disable_enable_at_begin) {
        core_driver.setModuleFunctionality(false);
    }
    core_driver.setModuleFunctionality(true);
}

void Narrowband::end() {
    core_driver.setModuleFunctionality(false);
}

Narrowband::operator bool() {
    if ( !core_driver.ready()) {
        return false;
    }
    bool enabled = false;
    if ( !core_driver.getModuleFunctionality((bool&)enabled)) {
        return false;
    }
    return enabled;
}

bool Narrowband::ensureAutomaticOperatorSelection(void) {
    OperatorSelectMode m; int f; String op;
    if ( core_driver.getOperatorSelection(m,f,op)) {
        if (m != OperatorSelectMode::Automatic) {
            return core_driver.setOperatorSelection(OperatorSelectMode::Automatic, "");
            delay(10*1000);
        }
    }
    return false;
}

bool Narrowband::ensureOperatorSelected(String op) {
    OperatorSelectMode  m;
    int f;
    String opq;

    //
    core_driver.setNetworkRegistration(1);
    delay(5000);

    // Start by querying the current mode
    if (!core_driver.getOperatorSelection(m,f,opq)) {
        return false;
    }

    // If we're deregistered, we directly set the operator selection manually,
    // ask again and continue
    if ( m == OperatorSelectMode::Deregister) {
        //
        if ( !core_driver.setOperatorSelection(OperatorSelectMode::Manual, op)) {
            return false;
        }
        // ask again
        if (!core_driver.getOperatorSelection(m,f,opq)) {
            return false;
        }

        core_driver.setModuleFunctionality(false);
        core_driver.setModuleFunctionality(true);
    }

    // Manual or Automatic, and operator is correct? Fine.
    if ( (m == OperatorSelectMode::Manual || m == OperatorSelectMode::Automatic) && op == opq) {
        return true;
    }

    // Wrong operator? The deregister and register again. Ask & check.
    if ( (m == OperatorSelectMode::Manual || m == OperatorSelectMode::Automatic) && op != opq) {
        // we need to switch AND deregister
        if ( !core_driver.setOperatorSelection(OperatorSelectMode::Deregister, op)) {
            return false;
        }
        if ( !core_driver.setOperatorSelection(OperatorSelectMode::Manual, op)) {
            return false;
        }

        // ask again
        if (!core_driver.getOperatorSelection(m,f,opq)) {
            return false;
        }

        delay(10000);

        if ( (m == OperatorSelectMode::Manual || m == OperatorSelectMode::Automatic) && op == opq) {
            core_driver.setModuleFunctionality(false);
            core_driver.setModuleFunctionality(true);

            return true;
        }
    }

    return false;
}

void _cb_attach_messages(const char *p, const void *ctx) {
    if ( ctx != NULL) {
        Narrowband *x = (Narrowband*)ctx;
        x->notify_status(p);
    }
}

void Narrowband::notify_status(const char *l) {
    // @TODO: refactor. checking for at commands here breaks
    // idea of abstraction between Driver and here.

    // copy l to own buffer because we're manipulating
    // the buffer afterwards.
    char buf[256];
    memset(buf,0,sizeof(buf));
    size_t n = strlen(l);
    if ( n >= sizeof(buf)) {
        n = sizeof(buf)-1;
    }
    memcpy(buf, l, n);

    if (strstr(buf,"+CEREG:") != 0) {
        char *q[3];
        int j = core_driver._split_csv_line(buf, strlen(buf),q, 3, "+CEREG");
        if ( j >= 1) {
            this->lastnotified_cereg = atoi(q[0]);
        }
    }
    if (strstr(buf,"+CSCON:") != 0) {
        char *q[3];
        int j = core_driver._split_csv_line(buf, strlen(buf),q, 3, "+CSCON");
        if ( j >= 1) {
            this->lastnotified_cscon = atoi(q[0]);
        }
    }
    if (strstr(buf,"+CGATT:") != 0) {
        char *q[3];
        int j = core_driver._split_csv_line(buf, strlen(buf),q, 3, "+CGATT");
        if ( j >= 1) {
            this->lastnotified_cgatt = atoi(q[0]);
        }
    }
}

bool Narrowband::attach( unsigned long timeout_msec, unsigned long wait_time_msec) {
    delay(wait_time_msec);
    if ( !core_driver.setNetworkRegistration(1)) {
        return false;
    }
    if ( !core_driver.setConnectionStatus(true)) {
        return false;
    }

    bool res = false;
    res = core_driver.setAttachStatus(true);
    if ( timeout_msec <= 0) {
        return res;
    }

    lastnotified_cereg = lastnotified_cscon = lastnotified_cgatt = 0;
    (void)core_driver.waitForResponse(timeout_msec, _cb_attach_messages, this);

    bool attached = false;
    if ( lastnotified_cereg > 0 && lastnotified_cscon > 0) {
        unsigned long now = millis();
        while ( !attached && (millis()-now) < timeout_msec) {
            delay(wait_time_msec);
            core_driver.getAttachStatus((bool&)attached);
        }
    } else {
        // we did not receive unsolicited responses from modem. 
        // Ask sequentially
        int reg_mode = 0, reg_status = 0;
        bool connected = false;
        int urcEnabled = -1;
        if (core_driver.getNetworkRegistration(reg_mode, reg_status)) {
            if (reg_mode == 1 && reg_status == 5) {
                unsigned long now = millis();
                while ( !attached && !connected && (millis()-now) < timeout_msec) {
                    delay(wait_time_msec);
                    core_driver.getConnectionStatus(urcEnabled, connected);
                    core_driver.getAttachStatus((bool&)attached);
                }
            }
        }
    }

    return attached;
}

bool Narrowband::isAttached() {
    bool attached = false;
    if(core_driver.getAttachStatus((bool&)attached)) {
        return attached;
    }
    return false;
}

bool Narrowband::detach(long timeout_msec) {
    bool res = core_driver.setAttachStatus(false);
    if ( timeout_msec <= 0) {
        return res;
    }

    core_driver.waitForResponse(timeout_msec, _cb_attach_messages);

    bool attached = false;
    res = core_driver.getAttachStatus((bool&)attached);

    return attached;
}

bool Narrowband::sendUDP( const char *ip, const int port, const uint8_t *p_data, const size_t sz_data) {
    int lport = random(32768,65535);
    int socket = core_driver.createSocket(SocketType::Datagram, 17, lport, false);
    if ( socket <= 0) {
        return false;
    }
    bool b = core_driver.sendTo(socket, ip, port, sz_data, p_data);
    core_driver.closeSocket(socket);
    return b;
}

bool Narrowband::sendUDP( const char *ip, const int port, String content) {
    return this->sendUDP(ip,port,(const uint8_t*)content.c_str(), content.length());
}

bool Narrowband::sendReceiveUDP( const char *ip, const int port, 
    const uint8_t *p_data, const size_t sz_data,
    uint8_t *p_response_data, const size_t sz_response_data,
    const long timeout_msec) {

    
    unsigned int lport = random(32768,65535);
    int socket = core_driver.createSocket(SocketType::Datagram, 17, lport, true);
    if ( socket <= 0) {
        return false;
    }
    bool b = core_driver.sendTo(socket, ip, port, sz_data, p_data);
    if ( !b) {
        core_driver.closeSocket(socket);
        return false;
    }

    int n = core_driver.waitForMessageIndication(socket, timeout_msec);
    if ( n > 0) {
        b = core_driver.recv(socket, p_response_data, n, timeout_msec);
    }
    core_driver.closeSocket(socket);

    return b;
}

bool Narrowband::sendReceiveUDP( const char *ip, const int port, 
    String request, String& response,
    const size_t sz_receive_bufsize,
    const long timeout_msec) {

    uint8_t *buf = (uint8_t*)calloc(1,sz_receive_bufsize);

    bool b = this->sendReceiveUDP(ip,port,(const uint8_t*)request.c_str(), request.length(), buf, sz_receive_bufsize, timeout_msec);
    if (b) {
        response.concat((const char *)buf);
    }
    free(buf);
    return b;
}


}
