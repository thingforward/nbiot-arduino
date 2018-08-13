#ifndef __NARROWBAND_H
#define __NARROWBAND_H

#include <stdint.h>

#include "narrowbandcore.h"

namespace Narrowband {


class Narrowband : public NarrowbandCore {
public:
    Narrowband(CommandAdapter& ca_, boolean b_reboot = false);

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
    bool attach(const long timeout_msec = 5000);

    /**
     * Checks for Registration, Connection and Attachment. Returns true, if all are valid, false otherwise.
     */
    bool isAttached();

    /**
     * detatches from the nb network.
     */
    bool detach();

    /**
     * Sends UDP packet
     */
    bool sendUDP( const char *ip, const int port, const uint8_t *p_data, const size_t sz_data);

    /**
     * Sends UDP packet and waits for reply
     */
    bool sendReceiveUDP( const char *ip, const int port, 
        const uint8_t *p_data, const size_t sz_data,
        uint8_t *p_response_data, const size_t sz_response_data,
        const long timeout_msec = 5000);

    /**
     * ICMP PING to a remote host
     */
    bool ping(const char *ip, const long timeout_msec = 5000);
};

}

#endif