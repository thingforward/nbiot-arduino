#ifndef __NARROWBANDCORE_H
#define __NARROWBANDCORE_H

#include <Arduino.h>
#include "commandadapter.h"

namespace Narrowband {

/**
 * Operator Selection Mode
 */
enum class OperatorSelectMode : int {
    Unknown = -1,
    Automatic = 0,
    Manual = 1,
    Deregister = 2
};

/** Socket Types */
enum class SocketType : int {
    Datagram = 0,
    Raw = 1
};

/**
 * A PDPContext defines a context id, type and
 * APN string 
 */
struct PDPContext {
    int     cid;
    char    type[16];
    char    APN[64];

    bool operator==(const PDPContext& other) const;
};

/**
 * NarrowbandCore communicates with a modem via a CommandAdapter instance
 * (e.g. connected via Serial/UART) and transforms modem requests into 
 * modem commands (e.g. query IMSI, open socket etc.).
 */
class NarrowbandCore {
    friend class Narrowband;
public:
    /** Constructs a NarrowbandCore object, ties it to
     * given CommandAdapter */
    NarrowbandCore(CommandAdapter& ca_);

    /** checks if module is ready. */
    bool ready();

    /** reboots the module. Returns after reboot. */
    void reboot();

    /** turns echo on/off. Methods below require echo == false */
    void setEcho(bool b_echo = false);

    /** Returns the Manufacturer Identification */
    String getManufacturerIdentification();

    /** Returns Model Identification */
    String getModelIdentification();
    
    /** Returns the International Mobile Equipment Identity. */
    String getIMEI();

    /** Returns the International Mobile Subscriber Identity. */
    String getIMSI();

    /** Retrieves the current operator selection */
    bool getOperatorSelection(OperatorSelectMode& mode, int& format, String& operatorName);

    /** set the current operator selection mode. Mode can be Manual, Automatic or deregistering .
     * In Manual mode, an `operatorName` has to be specified.
     */
    bool setOperatorSelection(OperatorSelectMode mode, const char *operatorName);

    /** Retrieves defined PDP contexts. Data is copied over into supplied array
     * of given size. 
     * Returns number of defined contexts.
    */
    int getPDPContexts(PDPContext* arrContext, size_t sz_max_context);

    /** Adds a PDP context. */
    bool addPDPContexts(const PDPContext& ctx);

    /** Retrieves the PDP address */
    bool getPDPAddress(String& pdpAddress);

    /** Retrieves the module functionality (true=full functionality)
     * returns success of command
     */
    bool getModuleFunctionality(bool& fullFunctionality);

    /** Sets the module functionality */
    bool setModuleFunctionality(bool fullFunctionality);

    /** Retrieves the current network registration settings */
    bool getNetworkRegistration(int& mode, int& status);

    /** sets the network registration */
    bool setNetworkRegistration(const int status);

    /** retrieves the current connection status */
    bool getConnectionStatus(int& urcEnabled, bool& connected);

    /** sets the connection status */
    bool setConnectionStatus(const bool connected);

    /** retrieves the current attachement status */
    bool getAttachStatus(bool& attached);

    /** sets the attachment status */
    bool setAttachStatus(bool attached);
    
    /** retrieves signal quality (RSSI, bit error rate) */
    bool getSignalQuality(int& rssi, int& ber);

    /**
     * Sets the CDP server to host:port(coap)
     */
    bool setCDPServer(String host, int port = 5683);

    /** Creates a socket of given type, protocol, local port. Receive Control
     * is disable by default. 
     */
    int createSocket( SocketType s, int protocol, int listenPort, bool bWithReceiveControl = false);

    /**
     * Closes an open socket 
     */
    bool closeSocket( int socket);

    /**
     * Sends content to a remote UDP address:port combination, using an open socket. 
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
    bool recv(int socket, uint8_t *buf, size_t sz_buf, unsigned long timeout);

    /**
     * ICMP PING to a remote host
     */
    bool ping(const char *ip, const long timeout_msec = 5000);

    /** 
     * Reads and parses data from modem for given time indicated by `timeout`. If `cb_modem_msg` is 
     * given, it is called for each line. 
     */
    int waitForResponse(unsigned long timeout, void(*cb_modem_msg)(const char *p_msg_line, const void *ctx) = NULL, const void *context = NULL);

    /** returns true if last command returned an error status */
    bool hasError() { return lastStatusError; }

    /** error status message if last response included such as message */
    String getLastError() { return lastError; }

private:
    CommandAdapter  &ca;

    String  lastError;
    bool    lastStatusOk, lastStatusError;


protected:
    void dbg_out1(const char *p, bool nl = false);

    int _split_response_array(char *buf, size_t n, char *arr_res[], int n_max_arr);

    int _split_csv_line(char *buf, size_t n, char *arr_res[], int n_max_arr, const char *p_expect_cmdstring = NULL);

    void clearLastStatus();
};

}


#endif