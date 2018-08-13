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
    bool enabled = false;
    if ( this->getModuleFunctionality( (bool&)enabled )) {
        if (!enabled) {
            this->setModuleFunctionality(true);
        }
    }
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

bool Narrowband::attach(const long timeout_msec) {
    this->setNetworkRegistration(1);
    delay(100);
    this->setConnectionStatus(true);
    delay(100);

    bool res = false;

    res = this->setAttachStatus(true);

    if ( timeout_msec <= 0) {
        return res;
    }
    
    unsigned long now = millis();
    bool attached = false;
    while ( !attached && (millis()-now) < timeout_msec) {
        delay(1000);
        this->getAttachStatus(attached);
    }

    return attached;
}

bool Narrowband::isAttached() {
    bool attached = false;
    if(this->getAttachStatus(attached)) {
        return attached;
    }
    return false;
}

bool Narrowband::detach() {
    return this->setAttachStatus(false);
}

bool Narrowband::sendUDP( const char *ip, const int port, const uint8_t *p_data, const size_t sz_data) {
    return false;
}

bool Narrowband::sendReceiveUDP( const char *ip, const int port, 
    const uint8_t *p_data, const size_t sz_data,
    uint8_t *p_response_data, const size_t sz_response_data,
    const long timeout_msec) {
    return false;
}

bool Narrowband::ping(const char *ip, const long timeout_msec) {
    return false;

}

}