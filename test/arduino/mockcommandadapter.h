#include <string.h>

#include <commandadapter.h>

using namespace Narrowband;

class MockCommandAdapter : public CommandAdapter {

public:
    MockCommandAdapter();

    virtual void send_cmd(const char *cmd);
    virtual bool send_cmd_waitfor_reply(const char *cmd, const char *reply) ;
    virtual size_t send_cmd_recv_reply(const char *cmd, char *replybuffer, size_t sz_replybuffer);
    virtual size_t send_cmd_recv_reply_stop(const char *cmd, char *replybuffer, size_t sz_replybuffer, const char *stopWord);

    enum Methods {
        _send_cmd = 0,
        _send_cmd_waitfor_reply,
        _send_cmd_recv_reply,
        _send_cmd_recv_reply_stop
    };

    void clear();

    const char *getLastCmd() { return ( const char *)last_cmd_buf; };

    bool getNumCalls(int idx) { return num_called[idx]; };

    void setRetvalBool(bool v) { retval_bool = v; }

    void setRetvalSize_t(size_t v) { retval_size_t = v; }

private:
    int     num_called[4];
    char    last_cmd_buf[1024];
    bool    retval_bool;
    size_t  retval_size_t;
};

MockCommandAdapter::MockCommandAdapter() {
    clear();
}

inline void MockCommandAdapter::clear() {
    memset(last_cmd_buf,0,sizeof(last_cmd_buf));
    memset(num_called,0,sizeof(num_called));
    retval_bool = false;
    retval_size_t = 0;
}

inline void MockCommandAdapter::send_cmd(const char *cmd) {
    num_called[Methods::_send_cmd]++;
    strncpy(last_cmd_buf, cmd, sizeof(last_cmd_buf));
}

inline bool MockCommandAdapter::send_cmd_waitfor_reply(const char *cmd, const char *reply) {
    num_called[Methods::_send_cmd_waitfor_reply]++;
    strncpy(last_cmd_buf, cmd, sizeof(last_cmd_buf));
    return retval_bool;
}

inline size_t MockCommandAdapter::send_cmd_recv_reply(const char *cmd, char *replybuffer, size_t sz_replybuffer) {
    num_called[Methods::_send_cmd_recv_reply]++;
    strncpy(last_cmd_buf, cmd, sizeof(last_cmd_buf));
    return 0;
}

inline size_t MockCommandAdapter::send_cmd_recv_reply_stop(const char *cmd, char *replybuffer, size_t sz_replybuffer, const char *stopWord) {
    num_called[Methods::_send_cmd_recv_reply_stop]++;
    strncpy(last_cmd_buf, cmd, sizeof(last_cmd_buf));
    return 0;
}