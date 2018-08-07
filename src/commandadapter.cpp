
#include <Arduino.h>
#include "commandadapter.h"

void CommandAdapter::dbg_out(char prefix, const char *p, char flag) {
    String res = "";
    Serial.print("["); 
    Serial.print(prefix); 
    Serial.print(flag); 
    int i = 0;
    int n = strlen(p);
    char const *x = p;
    while(i < n) {
        char buf[4];
        sprintf(buf, "%0.2X", *x);
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
}

void CommandAdapter::dbg_out0(const char *p, bool nl) {
    Serial.print("("); 
    Serial.print(p); 
    Serial.print(")"); 
    if ( nl) {
        Serial.println();
    }
}

void CommandAdapter::setTimeout(long timeout) {
    if (timeout > 0) {
        this->timeout = timeout;
    }
}

long CommandAdapter::getTmeout() const {
    return timeout;
}