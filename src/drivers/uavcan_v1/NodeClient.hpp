/****************************************************************************
 *
 *   Copyright (c) 2021 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file NodeClient.hpp
 *
 * Defines basic implementation of UAVCAN PNP for dynamic Node ID
 *
 * @author Peter van der Perk <peter.vanderperk@nxp.com>
 */

#pragma once


#include <px4_platform_common/defines.h>
#include <drivers/drv_hrt.h>

#include "CanardInterface.hpp"

#include <uavcan/node/ID_1_0.h>
#include <uavcan/pnp/NodeIDAllocationData_1_0.h>
#include <uavcan/pnp/NodeIDAllocationData_2_0.h>

#include "Services/AccessRequest.hpp"
#include "Services/ListRequest.hpp"

#define PNP1_PORT_ID                                 uavcan_pnp_NodeIDAllocationData_1_0_FIXED_PORT_ID_
#define PNP1_PAYLOAD_SIZE                            uavcan_pnp_NodeIDAllocationData_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_
#define PNP2_PORT_ID                                 uavcan_pnp_NodeIDAllocationData_2_0_FIXED_PORT_ID_
#define PNP2_PAYLOAD_SIZE                            uavcan_pnp_NodeIDAllocationData_2_0_SERIALIZATION_BUFFER_SIZE_BYTES_

class NodeClient : public UavcanBaseSubscriber
{
public:
	NodeClient(CanardInstance &ins, UavcanParamManager &pmgr) : UavcanBaseSubscriber(ins, "NodeIDAllocationData", 0),
		_canard_instance(ins) { };

	void subscribe() override
	{

		canardRxSubscribe(&_canard_instance,
				  CanardTransferKindMessage,
				  (_canard_instance.mtu_bytes == CANARD_MTU_CAN_FD ? PNP2_PORT_ID : PNP1_PORT_ID),  // The fixed Subject-ID
				  (_canard_instance.mtu_bytes == CANARD_MTU_CAN_FD ? PNP2_PAYLOAD_SIZE : PNP1_PAYLOAD_SIZE),
				  CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_USEC,
				  &_subj_sub._canard_sub);
	}

	bool HandleNodeIDRequest(uavcan_pnp_NodeIDAllocationData_1_0 &msg);
	bool HandleNodeIDRequest(uavcan_pnp_NodeIDAllocationData_2_0 &msg);

	void callback(const CanardTransfer &receive); // NodeIDAllocation callback

	void update();

private:

	CanardInstance &_canard_instance;
	CanardTransferID _node_id_alloc_transfer_id{0};

	hrt_abstime _nodealloc_request_last{0};
};
