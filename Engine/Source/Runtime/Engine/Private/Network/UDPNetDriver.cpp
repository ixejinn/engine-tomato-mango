#include "Network/UDPNetDriver.h"
#include "Network/SocketAddress.h"
#include "Network/UDPSocket.h"
#include "Utils/Logger.h"

namespace tomato
{
    UDPNetDriver::UDPNetDriver()
    {
        InitSocket();
    }

    UDPNetDriver::~UDPNetDriver() {}

    bool UDPNetDriver::InitSocket()
    {
        /**
        * Create UDP socket for game networking.
        * If this fails, network service cannot start.
        */
        socket_ = UDPSocket::CreateSocket();

        // INADDR_ANY allows receiving packets from any Network Interface Controller.
        port = 9000;
        SocketAddress myAddr((uint32_t)INADDR_ANY, port);
        if (socket_->Bind(myAddr) != NO_ERROR)
        {
            std::cout << socket_->Bind(myAddr) << '\n';
            return false;
        }

        TMT_INFO << "Initializing UDP NetDriver at " << myAddr.ToString();

        if (socket_ == nullptr)
            return false;

        if (socket_->SetNonBlockingMode(true) != NO_ERROR)
            return false;

        return true;
    }

    bool UDPNetDriver::SendPacket(const void* buffer, int& byteSentCount, const SocketAddress& inToAddress)
    {
        byteSentCount = socket_->SendTo(buffer, MAX_PACKET_SIZE, inToAddress);
        if (byteSentCount >= 0)
            return true;

        return false;
    }

    bool UDPNetDriver::RecvPacket(void* buffer, int& receivedBytes, SocketAddress& outFromAddress)
    {
        receivedBytes = socket_->ReceiveFrom(buffer, MAX_PACKET_SIZE, outFromAddress);
        if (receivedBytes > 0)
            return true;

        return false;
    }
}
