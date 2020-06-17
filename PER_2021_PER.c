
#include "PER_2021_PER.h"

void pack_MAIN_Heartbeat(CanTxMsgTypeDef* msg, MSG_MAIN_Heartbeat_t* out) {
	msg->Data[0] = (out->Heartbeat & 0x1);
}

void pack_MAIN_StateChange(CanTxMsgTypeDef* msg, MSG_MAIN_StateChange_t* out) {
	msg->Data[0] = (out->CarState & 0x7);
}

void pack_PDL_Heartbeat(CanTxMsgTypeDef* msg, MSG_PDL_Heartbeat_t* out) {
	msg->Data[0] = (out->Heartbeat & 0x1);
}

void pack_DSH_Heartbeat(CanTxMsgTypeDef* msg, MSG_DSH_Heartbeat_t* out) {
	msg->Data[0] = (out->Heartbeat & 0x1);
}

void pack_DSH_Buttons(CanTxMsgTypeDef* msg, MSG_DSH_Buttons_t* out) {
	msg->Data[0] = (out->StartButton & 0x1);
	msg->Data[0] |= (out->TractionMode & 0x3) << 1;
}

void pack_PDL_Throttle(CanTxMsgTypeDef* msg, MSG_PDL_Throttle_t* out) {
	msg->Data[0] = (out->ThrottlePosition_a & 0xff);
	msg->Data[1] = (out->ThrottlePosition_a & 0xf00) >> 8;
	msg->Data[1] |= (out->ThrottlePosition_b & 0xf) << 4;
	msg->Data[2] = (out->ThrottlePosition_b & 0xff0) >> 4;
	msg->Data[3] = (out->BreakPosition_a & 0xff);
	msg->Data[4] = (out->BreakPosition_a & 0xf00) >> 8;
	msg->Data[4] |= (out->BreakPosition_b & 0xf) << 4;
	msg->Data[5] = (out->BreakPosition_b & 0xff0) >> 4;
}




