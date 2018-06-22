// Copyright 2016 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include "core/hle/service/nwm/nwm.h"
#include "core/hle/service/nwm/nwm_cec.h"
#include "core/hle/service/nwm/nwm_ext.h"
#include "core/hle/service/nwm/nwm_inf.h"
#include "core/hle/service/nwm/nwm_sap.h"
#include "core/hle/service/nwm/nwm_soc.h"
#include "core/hle/service/nwm/nwm_tst.h"
#include "core/hle/service/nwm/nwm_uds.h"

namespace Service {
namespace NWM {

void InstallInterfaces(SM::ServiceManager& service_manager) {
    std::make_shared<NWM_CEC>()->InstallAsService(service_manager);
    std::make_shared<NWM_EXT>()->InstallAsService(service_manager);
    std::make_shared<NWM_INF>()->InstallAsService(service_manager);
    std::make_shared<NWM_SAP>()->InstallAsService(service_manager);
    std::make_shared<NWM_SOC>()->InstallAsService(service_manager);
    std::make_shared<NWM_TST>()->InstallAsService(service_manager);
    std::make_shared<NWM_UDS>()->InstallAsService(service_manager);
}

} // namespace NWM
} // namespace Service
