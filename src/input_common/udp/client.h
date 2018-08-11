// Copyright 2018 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <tuple>
#include <vector>
#include <boost/optional.hpp>
#include "common/common_types.h"
#include "common/thread.h"
#include "common/vector_math.h"

namespace InputCommon::CemuhookUDP {

static constexpr u16 DEFAULT_PORT = 26760;
static constexpr const char* DEFAULT_ADDR = "127.0.0.1";

class Socket;

namespace Response {
struct PadData;
struct PortInfo;
struct Version;
} // namespace Response

struct DeviceStatus {
    std::mutex update_mutex;
    std::tuple<Math::Vec3<float>, Math::Vec3<float>> motion_status;
    std::tuple<float, float, bool> touch_status;

    // calibration data for scaling the device's touch area to 3ds
    struct CalibrationData {
        u16 min_x;
        u16 min_y;
        u16 max_x;
        u16 max_y;
    };
    boost::optional<CalibrationData> touch_calibration;
};

class Client {
public:
    explicit Client(std::shared_ptr<DeviceStatus> status, const std::string& host = DEFAULT_ADDR,
                    u16 port = DEFAULT_PORT, u8 pad_index = 0, u32 client_id = 24872);
    ~Client();
    void ReloadSocket(const std::string& host = "127.0.0.1", u16 port = 26760,
                      u32 client_id = 24872);

private:
    void OnVersion(Response::Version);
    void OnPortInfo(Response::PortInfo);
    void OnPadData(Response::PadData);
    void StartCommunication(const std::string& host, u16 port, u32 client_id);

    std::unique_ptr<Socket> socket;
    std::shared_ptr<DeviceStatus> status;
    std::thread thread;
    u64 packet_sequence = 0;
};

/// An async job allowing configuration of the touchpad calibration.
class CalibrationConfigurationJob {
public:
    enum class Status {
        Initialized,
        Ready,
        Stage1Completed,
        Completed,
    };
    /**
     * Constructs and starts the job with the specified parameter.
     *
     * @param status_callback Callback for job status updates
     * @param data_callback Called when calibration data is ready
     */
    explicit CalibrationConfigurationJob(const std::string& host, u16 port, u32 client_id,
                                         std::function<void(Status)> status_callback,
                                         std::function<void(u16, u16, u16, u16)> data_callback);
    ~CalibrationConfigurationJob();
    void Stop();

private:
    Common::Event complete_event;
};

void TestCommunication(const std::string& host, u16 port, u32 client_id,
                       std::function<void()> success_callback,
                       std::function<void()> failure_callback);

} // namespace InputCommon::CemuhookUDP
