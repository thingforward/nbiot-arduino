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

#ifdef ARDUINO

#include <string.h>

#include <Arduino.h>

#define NBIOT_SERIAL    Serial

namespace Narrowband {

void btoh(char c, char *buf) {
    char hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    int8_t c_l = ((int8_t)c & 0x0F);
    int8_t c_u = (((int8_t)c & 0xF0) >> 4);

    buf[0] = hex[c_u];
    buf[1] = hex[c_l];
    buf[2] = 0;
}

void dbg_out(char prefix, const char *p, char flag) {
#ifdef NBIOT_DEBUG0
    String res = "";
    NBIOT_SERIAL.print("["); 
    NBIOT_SERIAL.print(prefix); 
    NBIOT_SERIAL.print(flag); 
    int i = 0;
    int n = strlen(p);
    char const *x = p;
    while(i < n) {
        char buf[3];
        btoh(*x, buf);

        NBIOT_SERIAL.print(buf); NBIOT_SERIAL.print(" ");

        if (*x < 32) {
            res.concat(".");
        } else {
            res.concat(*x);
        }
        x++; i++;
    }
    NBIOT_SERIAL.print("] [");
    NBIOT_SERIAL.print(res); NBIOT_SERIAL.println("]");
#endif
}

void dbg_outs16(char prefix, const char *p, size_t n, char flag) {
#ifdef NBIOT_DEBUG0
    char hexbuf[3];

    NBIOT_SERIAL.print("["); 
    NBIOT_SERIAL.print(prefix); 
    NBIOT_SERIAL.print(flag); 
    size_t i = 0, j = 0;
    char *x = (char*)p;
    char *y = (char*)p;
    while(i < n) {
        btoh(*x, hexbuf);
        NBIOT_SERIAL.print(hexbuf); NBIOT_SERIAL.print(" ");

        x++; i++; 
    }
    NBIOT_SERIAL.print("] [");
    while(j < n) {
        NBIOT_SERIAL.print((*y < 32)?'.':(*y));
        y++; j++; 
    }
    NBIOT_SERIAL.println("]");
#endif
}

void dbg_outs(char prefix, const char *p, size_t n, char flag) {
#ifdef NBIOT_DEBUG0
    if ( n <= 16) {
        dbg_outs16(prefix,p,n,flag);
        return;
    }
    size_t k = 0;
    char *q = (char*)p;
    while ( k < n) {
        int l = n-k;
        if ( l > 16) {
            l = 16;
        }
        dbg_outs16(prefix,q,l,flag);
        q += 16;
        k += 16;
    }
#endif
}

void dbg_out0(const char *p, bool nl) {
#ifdef NBIOT_DEBUG0
    NBIOT_SERIAL.print("("); 
    NBIOT_SERIAL.print(p); 
    NBIOT_SERIAL.print(")"); 
    if ( nl) {
        NBIOT_SERIAL.println();
    }
#endif
}

void dbg_out1(const char *p, bool nl) {
#ifdef NBIOT_DEBUG1
    NBIOT_SERIAL.print("("); 
    NBIOT_SERIAL.print(p); 
    NBIOT_SERIAL.print(")"); 
    if ( nl) {
        NBIOT_SERIAL.println();
    }
#endif
}


}


#else

#include <string.h>

// TODO: replace by (empty) macros
void dbg_out(char prefix, const char *p, char flag) { }
void dbg_outs16(char prefix, const char *p, size_t n, char flag) { }
void dbg_outs(char prefix, const char *p, size_t n, char flag) { }
void dbg_out0(const char *p, bool nl) { }
void dbg_out1(const char *p, bool nl) { }

#endif