#include "narrowbandcore.h"
#include "serialcmds.h"

bool PDPContext::operator==(const PDPContext& other) const {
    return (
        (strcmp(this->APN, other.APN) == 0) &&
        (strcmp(this->type, other.type) == 0)
    );
}

NarrowbandCore::NarrowbandCore(CommandAdapter& ca_) : ca(ca_){
    clearLastStatus();
}

void NarrowbandCore::clearLastStatus() {
    lastError = String("");
    lastStatusOk = lastStatusError = false;
}

bool NarrowbandCore::ready() {
    return ca.send_cmd_waitfor_reply("AT", "OK\r\n");
}

void NarrowbandCore::setEcho(bool b_echo) {
    char buf[16];
    sprintf(buf,"ATE=%d", b_echo);
    ca.send_cmd_waitfor_reply(buf, "OK\r\n");
}

/** Given a module response in `buf`, size `n`, this function splits by \r\n and assigns
 * the parts to `arr_res` of `n_max_arr` elements 
 * automatically checks for "OK" and "ERROR" line and stores them in instance vars, not
 * in resulting array.
 * */
int NarrowbandCore::_split_response_array(char *buf, size_t n, char *arr_res[], int n_max_arr) {
    clearLastStatus();
    char *p1 = buf;
    char *p2 = buf;
    int i = 0;
    int j = 0;
    while( i < n && j < n_max_arr) {
        if (*p2 == '\r' || *p2 == '\n') {
            *p2 = 0;

            if ( p2 != p1) {
                ca.dbg_out0(p1);

                if (strcmp(p1, "OK") == 0) {
                    lastStatusOk = true;
                } else {
                    if (strstr(p1,"ERROR") != 0) {
                        lastStatusError = true;
                        lastError = String(p1);
                    } else {
                        // store result
                        arr_res[j++] = p1;
                    }
                }
            }
            p1 = p2+1;
        }

        i++;
        p2++;
    }
    if (lastStatusError) {
        ca.dbg_out0("!!", true);
    } else {
        ca.dbg_out0("<<",true);
    }
    return j;
}

// expects the form +CMD:v1,v2,v3,...v[n_max_arr]
// after that, *buf == command without ':', rest of pointers assigned
// destroys buf
int NarrowbandCore::_split_csv_line(char *buf, size_t n, char *arr_res[], int n_max_arr, const char *p_expect_cmdstring) {
    char *sep = strchr(buf,':');
    int i = 0, j = 0;
    char *p = 0;
    if (buf[0] == '+' && sep != 0) {
        *sep = '\0';
        j = (sep-buf);
        p = ++sep;

        if ( p_expect_cmdstring != NULL) {
            if ( strcmp(buf, p_expect_cmdstring) != 0) {
                return -2;      // unexpected command
            }
        }

        while (sep != 0 && i < n_max_arr && j < n) {
          arr_res[i++] = p;
          sep = strchr(p,',');
          if ( sep != 0) {
            *sep = '\0';
            j = (sep-buf);
            p = ++sep;
          }
        }

        return i;
    } else {
        return -1;      // format invalid
    }

    return 0;
}


String NarrowbandCore::getModelIdentification() {
    clearLastStatus();

    const char *cmd = "AT+CGMM";
    char buf[128];
    size_t n = ca.send_cmd_recv_reply_stop(cmd, buf, sizeof(buf), "OK\r");

    char *p[4];
    int elems = _split_response_array(buf, n, p, 4);
    bool ok = (lastStatusOk && !lastStatusError && elems >= 1);
    if (ok) {
        return String(p[0]);
    }
    return String();
}

String NarrowbandCore::getManufacturerIdentification() {
    const char *cmd = "AT+CGMI";
    char buf[128];
    size_t n = ca.send_cmd_recv_reply_stop(cmd, buf, sizeof(buf), "OK\r");

    char *p[4];
    int elems = _split_response_array(buf, n, p, 4);
    bool ok = (lastStatusOk && !lastStatusError && elems >= 1);
    if (ok) {
        return String(p[0]);
    }
    return String();
}

String NarrowbandCore::getIMEI() {
    const char *cmd = "AT+CGSN";
    char buf[128];
    size_t n = ca.send_cmd_recv_reply_stop(cmd, buf, sizeof(buf), "OK\r");

    char *p[4];
    int elems = _split_response_array(buf, n, p, 4);
    bool ok = (lastStatusOk && !lastStatusError && elems >= 1);
    if (ok) {
        return String(p[0]);
    }
    return String();
}

bool NarrowbandCore::getOperatorSelection(int& mode, int& format, String& operatorName) {
    mode = -1; format = -1; operatorName = "";

    const char *cmd = "AT+COPS?";
    char buf[256];
    size_t n = ca.send_cmd_recv_reply(cmd, buf, sizeof(buf));


    // split command lines..
    char *p[4];
    int elems = _split_response_array(buf, n, p, 4);
    bool ok = (lastStatusOk && !lastStatusError && elems >= 1);
    for ( int i = 0; ok && i < elems; i++) {
        char *q[3];
        // split line into comma-sep elements
        int n = _split_csv_line(p[i], strlen(p[i]), q, 3, "+COPS");

        if ( n == 1) {
            mode = atoi(q[0]);
            return true;
        }
        if ( n == 3) {
            mode = atoi(q[0]);
            format = atoi(q[1]);
            operatorName = String(q[2]);
            return true;
        }
    }
    return false;

/*
    char *p[4];
    int elems = _split_response_array(buf, n, p, 4);
    bool ok = (lastStatusOk && !lastStatusError && elems >= 2);
    if (ok && String(p[0]).equals(cmd)) {
        String cops = String(p[1]);
        String cops_mode, cops_format, cops_opsnname;
        if (cops.startsWith("+COPS:")) {
            cops = cops.substring(7);
            int x = cops.indexOf(',');
            if ( x > 0) {
                cops_mode = cops.substring(0,x);
                mode = cops_mode.toInt();
                cops = cops.substring(x+1,cops.length());

                // format
                x = cops.indexOf(',');
                if ( x > 0) {
                    cops_format = cops.substring(0,x);
                    format = cops_format.toInt();

                    cops = cops.substring(x+1,cops.length());

                    // operator Name
                    x = cops.indexOf(',');
                    if ( x > 0) {
                        cops_opsnname = cops.substring(0,x);
                        cops = cops.substring(x+1,cops.length());
                    } else {
                        cops_opsnname = cops;
                    }
                    operatorName = cops_opsnname;
                } else {
                    cops_format = cops;
                    format = cops_format.toInt();
                }
            } else {
                cops_mode = cops;
                mode = cops_mode.toInt();
            }

        }
        return true;
    }
    return false;
    */
}

bool NarrowbandCore::setOperatorSelection(const OperatorSelectMode mode) {
    char cmd[64];
    sprintf(cmd, "AT+COPS=%d", mode);
    char buf[256];
    size_t n = ca.send_cmd_recv_reply(cmd, buf, sizeof(buf));

    char *p[4];
    int elems = _split_response_array(buf, n, p, 4);
    bool ok = (lastStatusOk && !lastStatusError && elems >= 2);
    return ok;
}

bool NarrowbandCore::getModuleFunctionality(bool& fullFunctionality) {
    const char *cmd = "AT+CFUN?";
    char buf[256];

    long t = ca.getTmeout();
    ca.setTimeout(2000);

    size_t n = ca.send_cmd_recv_reply_stop(cmd, buf, sizeof(buf), "OK\r");

    ca.setTimeout(t);

    char *p[2];
    int elems = _split_response_array(buf, n, p, 4);
    bool ok = (lastStatusOk && !lastStatusError && elems >= 2);
    if (ok && String(p[0]).equals(cmd)) {
        String res = String(p[1]);
        if (res.startsWith("+CFUN:")) {
            res = res.substring(7);
            fullFunctionality = (bool)res.toInt();
            return true;
        }
    }
    return false;
}

bool NarrowbandCore::setModuleFunctionality(const bool fullFunctionality) {
    char cmd[64];
    sprintf(cmd, "AT+CFUN=%d", fullFunctionality?1:0);
    char buf[256];

    long t = ca.getTmeout();
    ca.setTimeout(2000);

    size_t n = ca.send_cmd_recv_reply_stop(cmd, buf, sizeof(buf), "OK\r");

    ca.setTimeout(t);

    char *p[4];
    int elems = _split_response_array(buf, n, p, 4);
    bool ok = (lastStatusOk && !lastStatusError && elems >= 2);
    return ok;
}

bool NarrowbandCore::getNetworkRegistration(int& mode, int& status) {
    mode = -1; status = -1;

    const char *cmd = "AT+CEREG?";
    char buf[256];
    size_t n = ca.send_cmd_recv_reply_stop(cmd, buf, sizeof(buf), "OK\r");

    char *p[4];
    int elems = _split_response_array(buf, n, p, 4);
    bool ok = (lastStatusOk && !lastStatusError && elems >= 2);
    if (ok && String(p[0]).equals(cmd)) {
        String line = String(p[1]), part;
        int x;
        if (line.startsWith("+CEREG:")) {
            x = line.indexOf(':');
            line = line.substring(x+1,line.length());
            x = line.indexOf(',');
            if ( x > 0) {
                part = line.substring(0,x);
                mode = part.toInt();
                line = line.substring(x+1,line.length());

                x = line.indexOf(',');
                if ( x > 0) {
                    part = line.substring(0,x);
                    status = part.toInt();
                    //line = line.substring(x+1,line.length());

                } else {
                    status = line.toInt();
                }

            } else {
                mode = line.toInt();
            }

        }
        return true;
    }
    return false;
}

bool NarrowbandCore::setNetworkRegistration(const int status) {
    char cmd[64];
    sprintf(cmd, "AT+CEREG=%d", status);
    char buf[256];

    long t = ca.getTmeout();
    ca.setTimeout(5000);

    size_t n = ca.send_cmd_recv_reply_stop(cmd, buf, sizeof(buf), "OK\r");

    ca.setTimeout(t);

    char *p[4];
    int elems = _split_response_array(buf, n, p, 4);
    bool ok = (lastStatusOk && !lastStatusError && elems >= 1);
    return ok;
}

bool NarrowbandCore::getConnectionStatus(int& urcEnabled, bool& connected) {
    urcEnabled = -1; connected = false;

    const char *cmd = "AT+CSCON?";
    char buf[256];
    size_t n = ca.send_cmd_recv_reply_stop(cmd, buf, sizeof(buf), "OK\r");

    char *p[4];
    int elems = _split_response_array(buf, n, p, 4);
    bool ok = (lastStatusOk && !lastStatusError && elems >= 2);
    if (ok && String(p[0]).equals(cmd)) {
        String line = String(p[1]), part;
        int x;
        if (line.startsWith("+CSCON:")) {
            line = line.substring(7);
            x = line.indexOf(',');
            if ( x > 0) {
                part = line.substring(0,x);
                urcEnabled = part.toInt();
                line = line.substring(x+1,line.length());
                connected = (bool)line.toInt();
            } else {
                urcEnabled = line.toInt();
            }

        }
        return true;
    }
    return false;
}

bool NarrowbandCore::setConnectionStatus(const bool connected) {
    char cmd[64];
    sprintf(cmd, "AT+CSCON=%d", connected?1:0);
    char buf[256];

    long t = ca.getTmeout();
    ca.setTimeout(5000);

    size_t n = ca.send_cmd_recv_reply_stop(cmd, buf, sizeof(buf), "OK\r");

    ca.setTimeout(t);

    char *p[4];
    int elems = _split_response_array(buf, n, p, 4);
    bool ok = (lastStatusOk && !lastStatusError && elems >= 1);
    return ok;
}

bool NarrowbandCore::getAttachStatus(bool& attached) {
    attached = false;

    const char *cmd = "AT+CGATT?";
    char buf[256];
    size_t n = ca.send_cmd_recv_reply_stop(cmd, buf, sizeof(buf), "OK\r");

    char *p[4];
    int elems = _split_response_array(buf, n, p, 4);
    bool ok = (lastStatusOk && !lastStatusError && elems >= 2);
    if (ok && String(p[0]).equals(cmd)) {
        String line = String(p[1]), part;
        int x;
        if (line.startsWith("+CGATT:")) {
            line = line.substring(7);
            attached = (bool)line.toInt();
        }
        return true;
    }
    return false;
}

bool NarrowbandCore::setAttachStatus(const bool attached) {
    char cmd[64];
    sprintf(cmd, "AT+CGATT=%d", attached?1:0);
    char buf[256];

    long t = ca.getTmeout();
    ca.setTimeout(5000);

    size_t n = ca.send_cmd_recv_reply_stop(cmd, buf, sizeof(buf), "OK\r");

    ca.setTimeout(t);

    char *p[4];
    int elems = _split_response_array(buf, n, p, 4);
    bool ok = (lastStatusOk && !lastStatusError && elems >= 1);
    return ok;
}

bool NarrowbandCore::getSignalQuality(int& rssi, int& ber) {
    rssi = -1; ber = -1;

    const char *cmd = "AT+CSQ";
    char buf[256];
    size_t n = ca.send_cmd_recv_reply_stop(cmd, buf, sizeof(buf), "OK\r");

    // split command lines..
    char *p[4];
    int elems = _split_response_array(buf, n, p, 4);
    bool ok = (lastStatusOk && !lastStatusError && elems >= 1);
    for ( int i = 0; ok && i < elems; i++) {
        char *q[2];
        // split line into comma-sep elements
        int n = _split_csv_line(p[i], strlen(p[i]), q, 2, &cmd[2]);

        if ( n == 2) {
            rssi = atoi(q[0]);
            ber = atoi(q[1]);
            return true;
        }
    }
    return false;
}

int NarrowbandCore::getPDPContexts(PDPContext *arrContext, size_t sz_max_context) {
    const char *cmd = "AT+CGDCONT?";
    char buf[1024];
    size_t n = ca.send_cmd_recv_reply_stop(cmd, buf, sizeof(buf), "OK\r");
    int j = 0;

    // split command lines..
    char *p[12];
    int elems = _split_response_array(buf, n, p, 12);
    bool ok = (lastStatusOk && !lastStatusError && elems >= 1);
    for ( int i = 0; ok && i < elems; i++) {
        char *q[4];
        // split line into comma-sep elements
        int n = _split_csv_line(p[i], strlen(p[i]), q, 4, "+CGDCONT");

        if ( n >= 3) {
            PDPContext& p = arrContext[j++];
            memset(&p,0,sizeof(p) );
            p.cid = atoi(q[0]);

            char *x = q[1];
            int l = strlen(x);
            if ( l > 0) {
                if (x[l-1] == '"') {
                    x[l-1] = 0;
                    l--; 
                }
                if (*x == '"') {
                    x++; l--;
                }
                strncpy(p.type, x, l); 
            }

            x = q[2];
            l = strlen(x);
            if ( l > 0) {
                if (x[l-1] == '"') {
                    x[l-1] = 0;
                    l--; 
                }
                if (*x == '"') {
                    x++; l--;
                }
                strncpy(p.APN, x,l);
            }
        }
    }
    return j;

}

bool NarrowbandCore::addPDPContexts(PDPContext& ctx) {
    char cmdbuf[128];
    sprintf(cmdbuf, "AT+CGDCONT=%d,\"%s\",\"%s\"", ctx.cid, ctx.type, ctx.APN);

    char buf[128];
    size_t n = ca.send_cmd_recv_reply_stop(cmdbuf, buf, sizeof(buf), "OK\r");
    return (lastStatusOk && !lastStatusError);
}
