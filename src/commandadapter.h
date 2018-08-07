#ifndef __COMMANDADAPTER_H
#define __COMMANDADAPTER_H

#include <stddef.h>

/**
 * Abstract class `CommandAdapter`, used by upper layer classes to execute
 * commands, do i/o with underlying communication mechanisms (e.g. serial or spi)
 */
class CommandAdapter {
    friend class NarrowbandCore;

public:

    /**
     * Sends a command via modem_serial
     * Does not wait for a response nor capture the response
     */
    virtual void send_cmd(const char *cmd) = 0;

    /**
     * Sends `cmd` to `modem_serial`. Waits for up to `timeout` msecs for a response
     * If response contains `reply`, returns true, false otherwise.
     */
    virtual bool send_cmd_waitfor_reply(const char *cmd, const char *reply) = 0;
   
    /**
     * Sends `cmd` to `modem_serial`. Waits for up to `timeout` msecs for a response. Response is captured
     * in `replybuffer` of size `sz_replybuffer.
     */
    virtual size_t send_cmd_recv_reply(const char *cmd, char *replybuffer, size_t sz_replybuffer) = 0;
   
    /**
     * Sends `cmd` to `modem_serial`. Waits for up to `timeout` msecs for a response OR `stopWord` appears in response.
     * Response is captured in `replybuffer` of size `sz_replybuffer.
     */
    virtual size_t send_cmd_recv_reply_stop(const char *cmd, char *replybuffer, size_t sz_replybuffer, const char *stopWord) = 0;
   
    void setTimeout(long timeout);

    long getTmeout() const;

protected:
    void dbg_out(char prefix, const char *p, char flag = ' ');
    void dbg_out0(const char *p, bool nl = false);

    long    timeout;
};

#endif