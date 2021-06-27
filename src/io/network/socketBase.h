#ifndef SP2_IO_NETWORK_SOCKET_BASE_H
#define SP2_IO_NETWORK_SOCKET_BASE_H

#include <io/network/socketBase.h>
#include "nonCopyable.h"


namespace sp {
namespace io {
namespace network {


class SocketBase : sp::NonCopyable
{
public:
    void setBlocking(bool blocking);
    void setTimeout(int milliseconds);

protected:
    bool isLastErrorNonBlocking();

    int handle = -1;
    bool blocking = true;
    
    friend class Selector;
};

}//namespace network
}//namespace io
}//namespace sp

#endif//SP2_IO_NETWORK_SOCKET_BASE_H
