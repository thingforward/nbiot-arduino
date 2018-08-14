#include <narrowband.h>
#include <Arduino.h>

namespace Narrowband {


Narrowband::Narrowband(CommandAdapter& ca_, boolean b_reboot) : NarrowbandCore(ca_) {
    if (b_reboot) {
        this->reboot();
    }
}

void Narrowband::begin() {
    while (!this->ready()) {
        delay(1000);
    }
    this->setEcho(false);
    this->setModuleFunctionality(false);
    this->setModuleFunctionality(true);
}

void Narrowband::end() {
    this->setModuleFunctionality(false);
}

Narrowband::operator bool() {
    if ( !this->ready()) {
        return false;
    }
    bool enabled = false;
    if ( !this->getModuleFunctionality((bool&)enabled)) {
        return false;
    }
    return enabled;
}

void _cb_attach_messages(const char *p, const void *ctx) {
    Narrowband *nb = (Narrowband*)ctx;
    nb->notify_status(p);
}

void Narrowband::notify_status(const char *l) {
    dbg_out1(l,true);

    // copy l to own buffer because we're manipulating
    // the buffer afterwards.
    char buf[256];
    strcpy(buf,l);

    if (strstr(buf,"+CEREG:") != 0) {
        char *q[3];
        int j = _split_csv_line(buf, strlen(buf),q, 3, "+CEREG");
        if ( j >= 1) {
            lastnotified_cereg = atoi(q[0]);
        }
    }
    if (strstr(buf,"+CSCON:") != 0) {
        char *q[3];
        int j = _split_csv_line(buf, strlen(buf),q, 3, "+CSCON");
        if ( j >= 1) {
            lastnotified_cscon = atoi(q[0]);
        }
    }
    if (strstr(buf,"+CGATT:") != 0) {
        char *q[3];
        int j = _split_csv_line(buf, strlen(buf),q, 3, "+CGATT");
        if ( j >= 1) {
            lastnotified_cgatt = atoi(q[0]);
        }
    }
}

bool Narrowband::attach( long timeout_msec, long wait_time_msec) {
    delay(wait_time_msec);
    this->setNetworkRegistration(1);
    delay(wait_time_msec);
    this->setConnectionStatus(true);
    delay(wait_time_msec);

    bool res = false;
    res = this->setAttachStatus(true);
    if ( timeout_msec <= 0) {
        return res;
    }

    lastnotified_cereg = lastnotified_cscon = lastnotified_cgatt = 0;
    int n = waitForResponse(timeout_msec, _cb_attach_messages);
    
    bool attached = false;
    if ( lastnotified_cereg > 0 && lastnotified_cscon > 0) {
        unsigned long now = millis();
        while ( !attached && (millis()-now) < timeout_msec) {
            delay(wait_time_msec*3);
            this->getAttachStatus((bool&)attached);
        }
    }

    return attached;
}

bool Narrowband::isAttached() {
    bool attached = false;
    if(this->getAttachStatus((bool&)attached)) {
        return attached;
    }
    return false;
}

bool Narrowband::detach(long timeout_msec) {
    bool res = this->setAttachStatus(false);
    if ( timeout_msec <= 0) {
        return res;
    }

    int n = waitForResponse(timeout_msec, _cb_attach_messages);

    bool attached = false;
    res = this->getAttachStatus((bool&)attached);

    return attached;
}

bool Narrowband::sendUDP( const char *ip, const int port, const uint8_t *p_data, const size_t sz_data) {
    int lport = random(32768,65535);
    int socket = this->createSocket(SocketType::Datagram, 17, lport, false);
    if ( socket <= 0) {
        return false;
    }
    bool b = this->sendTo(socket, ip, port, sz_data, p_data);
    this->closeSocket(socket);

}

bool Narrowband::sendUDP( const char *ip, const int port, String content) {
    return this->sendUDP(ip,port,(const uint8_t*)content.c_str(), content.length());
}

bool Narrowband::sendReceiveUDP( const char *ip, const int port, 
    const uint8_t *p_data, const size_t sz_data,
    uint8_t *p_response_data, const size_t sz_response_data,
    const long timeout_msec) {

    int lport = random(32768,65535);
    int socket = this->createSocket(SocketType::Datagram, 17, lport, true);
    if ( socket <= 0) {
        return false;
    }
    bool b = this->sendTo(socket, ip, port, sz_data, p_data);
    if ( !b) {
        dbg_out1("Error sending UPD");
        this->closeSocket(socket);
        return false;
    }

    int n = this->waitForMessageIndication(socket, timeout_msec);
    if ( n > 0) {
        b = this->recv(socket, p_response_data, n, timeout_msec);
    }
    this->closeSocket(socket);

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