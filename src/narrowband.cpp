#include <narrowband.h>
#include <Arduino.h>

namespace Narrowband {


Narrowband::Narrowband(NarrowbandCore& nbc_, boolean b_reboot) : core_driver(nbc_) {
    if (b_reboot) {
        core_driver.reboot();
    }
}

void Narrowband::begin() {
    while (!core_driver.ready()) {
        delay(1000);
    }
    core_driver.setEcho(false);
    core_driver.setModuleFunctionality(false);
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
    int n = strlen(l);
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

bool Narrowband::attach( long timeout_msec, long wait_time_msec) {
    delay(wait_time_msec);
    core_driver.setNetworkRegistration(1);
    delay(wait_time_msec);
    core_driver.setConnectionStatus(true);
    delay(wait_time_msec);

    bool res = false;
    res = core_driver.setAttachStatus(true);
    if ( timeout_msec <= 0) {
        return res;
    }

    lastnotified_cereg = lastnotified_cscon = lastnotified_cgatt = 0;
    int n = core_driver.waitForResponse(timeout_msec, _cb_attach_messages, this);

    bool attached = false;
    if ( lastnotified_cereg > 0 && lastnotified_cscon > 0) {
        Serial.println("A");
        unsigned long now = millis();
        while ( !attached && (millis()-now) < timeout_msec) {
            Serial.println(attached);
            delay(wait_time_msec*3);
            core_driver.getAttachStatus((bool&)attached);
        }
    }
    Serial.println("B");

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

    int n = core_driver.waitForResponse(timeout_msec, _cb_attach_messages);

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

}

bool Narrowband::sendUDP( const char *ip, const int port, String content) {
    return this->sendUDP(ip,port,(const uint8_t*)content.c_str(), content.length());
}

bool Narrowband::sendReceiveUDP( const char *ip, const int port, 
    const uint8_t *p_data, const size_t sz_data,
    uint8_t *p_response_data, const size_t sz_response_data,
    const long timeout_msec) {

    int lport = random(32768,65535);
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
    const long timeout_msec) {

    uint8_t buf[2048];
    memset(buf,0,sizeof(buf));

    bool b = this->sendReceiveUDP(ip,port,(const uint8_t*)request.c_str(), request.length(), buf, sizeof(buf), timeout_msec);
    if (b) {
        response.concat((const char *)buf);
    }
    return b;
}


}