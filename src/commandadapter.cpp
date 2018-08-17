
#include <Arduino.h>
#include "commandadapter.h"

namespace Narrowband {

void CommandAdapter::dbg_out(char prefix, const char *p, char flag) {
#ifdef NBIOT_DEBUG0
    String res = "";
    Serial.print("["); 
    Serial.print(prefix); 
    Serial.print(flag); 
    int i = 0;
    int n = strlen(p);
    char const *x = p;
    while(i < n) {
        char buf[4];
        sprintf(buf, "%.2X", *x);
        Serial.print(buf); Serial.print(" ");

        if (*x < 32) {
            res.concat(".");
        } else {
            res.concat(*x);
        }
        x++; i++;
    }
    Serial.print("] [");
    Serial.print(res); Serial.println("]");
#endif
}

void CommandAdapter::dbg_outs(char prefix, const char *p, size_t n, char flag) {
#ifdef NBIOT_DEBUG0
    String res = "";
    Serial.print("["); 
    Serial.print(prefix); 
    Serial.print(flag); 
    int i = 0;
    char const *x = p;
    while(i < n) {
        char buf[4];
        sprintf(buf, "%.2X", *x);
        Serial.print(buf); Serial.print(" ");

        if (*x < 32) {
            res.concat(".");
        } else {
            res.concat(*x);
        }
        x++; i++;
    }
    Serial.print("] [");
    Serial.print(res); Serial.println("]");
#endif
}

void CommandAdapter::dbg_out0(const char *p, bool nl) {
#ifdef NBIOT_DEBUG0
    Serial.print("("); 
    Serial.print(p); 
    Serial.print(")"); 
    if ( nl) {
        Serial.println();
    }
#endif
}

void CommandAdapter::setTimeout(unsigned long timeout) {
    if (timeout > 0) {
        this->timeout = timeout;
    }
}

unsigned long CommandAdapter::getTmeout() const {
    return timeout;
}


}