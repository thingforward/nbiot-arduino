#ifndef __NARROWBANDCORE_H
#define __NARROWBANDCORE_H

#include "serialcmds.h"

enum class OperatorSelectMode : int {
    Automatic = 0,
    Manual = 1,
    Deregister = 2
};

struct PDPContext {
    int     cid;
    char    type[16];
    char    APN[64];

    bool operator==(const PDPContext& other) const;
};

class NarrowbandCore {
public:
    NarrowbandCore(CommandAdapter& ca_);

    /** checks if module is ready (AT) */
    bool ready();

    /** turns echo on/off. Methods below require echo == false */
    void setEcho(bool b_echo = false);

    /** Returns the Manufacturer Identification (AT+CGMI) */
    String getManufacturerIdentification();

    /** Returns Model Identification (AT+CGMM) */
    String getModelIdentification();
    
    /** */
    String getIMEI();

    /** */
    bool getOperatorSelection(int& mode, int& format, String& operatorName);

    /** */
    bool setOperatorSelection(const OperatorSelectMode mode);

    /** */
    int getPDPContexts(PDPContext* arrContext, size_t sz_max_context);

    /** */
    bool addPDPContexts(PDPContext& ctx);

    /** */
    bool getModuleFunctionality(bool& fullFunctionality);

    /** */
    bool setModuleFunctionality(const bool fullFunctionality);

    /** */
    bool getNetworkRegistration(int& mode, int& status);

    /** */
    bool setNetworkRegistration(const int status);

    /** */
    bool getConnectionStatus(int& urcEnabled, bool& connected);

    /** */
    bool setConnectionStatus(const bool connected);

    /** */
    bool getAttachStatus(bool& attached);

    /** */
    bool getSignalQuality(int& rssi, int& ber);

    /** */
    bool setAttachStatus(const bool attached);
    

    /** returns true if last command returned an error status */
    bool hasError() { return lastStatusError; }

    /** error status message if last response included such as message */
    String getLastError() { return lastError; }

private:
    CommandAdapter  &ca;

    String  lastError;
    bool    lastStatusOk, lastStatusError;

    void clearLastStatus();

    int _split_response_array(char *buf, size_t n, char *arr_res[], int n_max_arr);

    int _split_csv_line(char *buf, size_t n, char *arr_res[], int n_max_arr, const char *p_expect_cmdstring = NULL);

};

#endif