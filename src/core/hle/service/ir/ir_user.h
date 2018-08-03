// Copyright 2015 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <functional>
#include <memory>
#include <vector>
#include "core/hle/service/service.h"

namespace Kernel {
class Event;
class SharedMemory;
} // namespace Kernel

namespace CoreTiming {
struct EventType;
} // namespace CoreTiming

namespace Service {
namespace IR {

class BufferManager;
class ExtraHID;

/// An interface representing a device that can communicate with 3DS via ir:USER service
class IRDevice {
public:
    /**
     * A function object that implements the method to send data to the 3DS, which takes a vector of
     * data to send.
     */
    using SendFunc = std::function<void(const std::vector<u8>& data)>;

    explicit IRDevice(SendFunc send_func);
    virtual ~IRDevice();

    /// Called when connected with 3DS
    virtual void OnConnect() = 0;

    /// Called when disconnected from 3DS
    virtual void OnDisconnect() = 0;

    /// Called when data is received from the 3DS. This is invoked by the ir:USER send function.
    virtual void OnReceive(const std::vector<u8>& data) = 0;

protected:
    /// Sends data to the 3DS. The actual sending method is specified in the constructor
    void Send(const std::vector<u8>& data);

private:
    const SendFunc send_func;
};

/// Interface to "ir:USER" service
class IR_USER final : public ServiceFramework<IR_USER> {
public:
    IR_USER();
    ~IR_USER();

    void ReloadInputDevices();

private:
    /**
     * InitializeIrNopShared service function
     * Initializes ir:USER service with a user provided shared memory. The shared memory is
     * configured
     * to shared mode (with SharedMemoryHeader at the beginning of the shared memory).
     *  Inputs:
     *      1 : Size of shared memory
     *      2 : Recv buffer size
     *      3 : Recv buffer packet count
     *      4 : Send buffer size
     *      5 : Send buffer packet count
     *      6 : BaudRate (u8)
     *      7 : 0 (Handle descriptor)
     *      8 : Handle of shared memory
     *  Outputs:
     *      1 : Result of function, 0 on success, otherwise error code
     */
    void InitializeIrNopShared(Kernel::HLERequestContext& ctx);

    /**
     * RequireConnection service function
     * Searches for an IR device and connects to it. After connecting to the device, applications
     * can
     * use SendIrNop function, ReceiveIrNop function (or read from the buffer directly) to
     * communicate
     * with the device.
     *  Inputs:
     *      1 : device ID? always 1 for circle pad pro
     *  Outputs:
     *      1 : Result of function, 0 on success, otherwise error code
     */
    void RequireConnection(Kernel::HLERequestContext& ctx);

    /**
     * GetReceiveEvent service function
     * Gets an event that is signaled when a packet is received from the IR device.
     *  Outputs:
     *      1 : Result of function, 0 on success, otherwise error code
     *      2 : 0 (Handle descriptor)
     *      3 : Receive event handle
     */
    void GetReceiveEvent(Kernel::HLERequestContext& ctx);

    /**
     * GetSendEvent service function
     * Gets an event that is signaled when the sending of a packet is complete
     *  Outputs:
     *      1 : Result of function, 0 on success, otherwise error code
     *      2 : 0 (Handle descriptor)
     *      3 : Send event handle
     */
    void GetSendEvent(Kernel::HLERequestContext& ctx);

    /**
     * Disconnect service function
     * Disconnects from the current connected IR device.
     *  Outputs:
     *      1 : Result of function, 0 on success, otherwise error code
     */
    void Disconnect(Kernel::HLERequestContext& ctx);

    /**
     * GetConnectionStatusEvent service function
     * Gets an event that is signaled when the connection status is changed
     *  Outputs:
     *      1 : Result of function, 0 on success, otherwise error code
     *      2 : 0 (Handle descriptor)
     *      3 : Connection Status Event handle
     */
    void GetConnectionStatusEvent(Kernel::HLERequestContext& ctx);

    /**
     * FinalizeIrNop service function
     * Finalize ir:USER service.
     *  Outputs:
     *      1 : Result of function, 0 on success, otherwise error code
     */
    void FinalizeIrNop(Kernel::HLERequestContext& ctx);

    /**
     * SendIrNop service function
     * Sends a packet to the connected IR device
     *  Inpus:
     *      1 : Size of data to send
     *      2 : 2 + (size << 14) (Static buffer descriptor)
     *      3 : Data buffer address
     *  Outputs:
     *      1 : Result of function, 0 on success, otherwise error code
     */
    void SendIrNop(Kernel::HLERequestContext& ctx);

    /**
     * ReleaseReceivedData function
     * Release a specified amount of packet from the receive buffer. This is called after the
     * application reads received packet from the buffer directly, to release the buffer space for
     * future packets.
     *  Inpus:
     *      1 : Number of packets to release
     *  Outputs:
     *      1 : Result of function, 0 on success, otherwise error code
     */
    void ReleaseReceivedData(Kernel::HLERequestContext& ctx);

    void PutToReceive(const std::vector<u8>& payload);

    Kernel::SharedPtr<Kernel::Event> conn_status_event, send_event, receive_event;
    Kernel::SharedPtr<Kernel::SharedMemory> shared_memory;
    IRDevice* connected_device{nullptr};
    std::unique_ptr<BufferManager> receive_buffer;
    std::unique_ptr<ExtraHID> extra_hid;
};

} // namespace IR
} // namespace Service
