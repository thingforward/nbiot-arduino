#ifndef __NARROWBANDCORE_H
#define __NARROWBANDCORE_H

#include <Arduino.h>
#include "commandadapter.h"

namespace Narrowband {

enum class OperatorSelectMode : int {
    Unknown = -1,
    Automatic = 0,
    Manual = 1,
    Deregister = 2
};

enum class SocketType : int {
    Datagram = 0,
    Raw = 1
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

    /** reboots the module. Returns after reboot. */
    void reboot();

    /** turns echo on/off. Methods below require echo == false */
    void setEcho(bool b_echo = false);

    /** Returns the Manufacturer Identification (AT+CGMI) */
    String getManufacturerIdentification();

    /** Returns Model Identification (AT+CGMM) */
    String getModelIdentification();
    
    /** Returns the International Mobile Equipment Identity. (AT+CGSN) */
    String getIMEI();

    /** Returns the International Mobile Subscriber Identity. (AT+CIMI) */
    String getIMSI();

    /** Retrieves the current operator selection (AT+COPS?) */
    bool getOperatorSelection(OperatorSelectMode& mode, int& format, char *operatorName);

    /** */
    bool setOperatorSelection(OperatorSelectMode mode, const char *operatorName);

    /** */
    int getPDPContexts(PDPContext* arrContext, size_t sz_max_context);

    /** */
    bool addPDPContexts(PDPContext& ctx);

    /** */
    bool getModuleFunctionality(bool& fullFunctionality);

    /** */
    bool setModuleFunctionality(bool fullFunctionality);

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
    bool setAttachStatus(bool attached);
    
    /**
     * Sets the CDP server to host:port(coap)
     */
    bool setCDPServer(String host, int port = 5683);

    /** Creates a socket of given type, protocol, local port. Receive Control
     * is disable by default. (AT+NSOCR)
     */
    int createSocket( SocketType s, int protocol, int listenPort, bool bWithReceiveControl = false);

    /**
     * Closes an open socket (AT+NSOCL)
     */
    bool closeSocket( int socket);

    /**
     * Sends content to a remote UDP address:port combination, using an open socket. (AT+NSOST)
     */ 
    bool sendTo(int socket, const char *remoteAddr, int remotePort, size_t length, const uint8_t *p_data);

    /**
     * Listens on a socket with Receive control, for a message indication (+NOSNMI), up to 
     * a maximum time frame indicated by timeout.
     * Returns the number of bytes indicated or 0 if no message has been indicated.
     */
    size_t waitForMessageIndication(int socket, unsigned long timeout);

    /**
     * Receives data from a socket after a message indication. Stores data
     * in buffer
     */
    bool recv(int socket, char *buf, size_t sz_buf, unsigned long timeout);


    /** returns true if last command returned an error status */
    bool hasError() { return lastStatusError; }

    /** error status message if last response included such as message */
    String getLastError() { return lastError; }

private:
    CommandAdapter  &ca;

    String  lastError;
    bool    lastStatusOk, lastStatusError;

    int _split_response_array(char *buf, size_t n, char *arr_res[], int n_max_arr);

    int _split_csv_line(char *buf, size_t n, char *arr_res[], int n_max_arr, const char *p_expect_cmdstring = NULL);

    void clearLastStatus();

    void dbg_out1(const char *p, bool nl = false);

};

}


#endif