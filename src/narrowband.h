#ifndef __NARROWBAND_H
#define __NARROWBAND_H

#include <stdint.h>

#include "narrowbandcore.h"

namespace Narrowband {

/**
 * Narrowband offers high level methods to interact with 
 * a modem. Typical approach is to begin a session, attach
 * to the network, send/receive data.
 * It uses methods from Narrowband core.
 */
class Narrowband {
public:
    Narrowband(NarrowbandCore& nb_, bool b_reboot = false);

    /**
     * Enables module functionality
     */
    void begin();

    /**
     * Disables module functionality
     */
    void end();

    /**
     * Checks if module is in attention and enabled
     */
    operator bool();

    /**
     * Attaches to the narrowband network, according to module/band setup. If timeout is > 0, this method
     * is synchronous, waits for successful attachment and returns within maximum timeout specified.
     * If timeout_msec <= 0, attach calls are triggered but not waited for successful completion (asynchronous)
     * @return successful attachment (sychronous) or error-free triggering (asynchronous).
     */
    bool attach(long timeout_msec = 5000, long wait_time_msec = 500);

    /**
     * Checks for Registration, Connection and Attachment. Returns true, if all are valid, false otherwise.
     */
    bool isAttached();

    /**
     * detatches from the nb network.
     */
    bool detach(long timeout_msec = 5000);

    /**
     * Sends UDP packet
     */
    bool sendUDP( const char *ip, const int port, const uint8_t *p_data, const size_t sz_data);
    bool sendUDP( const char *ip, const int port, String content);

    /**
     * Sends UDP packet and waits for reply
     */
    bool sendReceiveUDP( const char *ip, const int port, 
        const uint8_t *p_data, const size_t sz_data,
        uint8_t *p_response_data, const size_t sz_response_data,
        const long timeout_msec = 5000);
    bool sendReceiveUDP( const char *ip, const int port, 
        String request, String& response,
        const long timeout_msec = 5000);

    /**
     */
    void notify_status(const char *l);

    NarrowbandCore& getCore() const { return core_driver; };

private:
    void cb_attach_messages(const char *p, const void *ctx);

protected:
    int lastnotified_cereg, lastnotified_cscon, lastnotified_cgatt; 
    NarrowbandCore& core_driver;   
};

}

#endif