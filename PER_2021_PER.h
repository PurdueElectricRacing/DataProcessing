
#ifndef _PER_2021_CAN_H
#define _PER_2021_CAN_H

#include "stdint.h"


typedef struct
{
  uint16_t StdId; /*!< Specifies the standard identifier. */
  uint32_t ExtId; /*!< Specifies the extended identifier. */
  uint32_t IDE; /*!< Specifies the type of identifier for the message that will be transmitted.  */
  uint32_t DLC; /*!< Specifies the length of the frame that will be transmitted. */
  uint8_t Data[8]; /*!< Contains the data to be transmitted. */
} CanTxMsgTypeDef;


/* 
*   Value Table Struct Definitions
*/

typedef enum {
	INCOMPLETE = 0,
	COMPLETE = 1
} PRECHARGESTATE_t; 

typedef enum {
	INIT = 0,
	PRE_READY_TO_DRIVE = 1,
	READY_TO_DRIVE = 2,
	ERROR = 3,
	RESET = 4,
	RECOVER = 5
} CARSTATE_t; 

typedef enum {
	TRACTION_CONTROL_DISABLE = 0,
	TRACTON_CONTROL_ENABLE = 1,
	LAUNCH_CONTROL = 2
} TRACTIONMODE_t; 


/* 
*   Message Struct Definitions
*/

typedef struct {
	uint8_t Heartbeat;
} MSG_MAIN_Heartbeat_t;

typedef struct {
	CARSTATE_t CarState;
} MSG_MAIN_StateChange_t;

typedef struct {
	uint8_t Heartbeat;
} MSG_PDL_Heartbeat_t;

typedef struct {
	uint8_t Heartbeat;
} MSG_DSH_Heartbeat_t;

typedef struct {
	int8_t StartButton;
	TRACTIONMODE_t TractionMode;
} MSG_DSH_Buttons_t;

typedef struct {
	uint16_t ThrottlePosition_a;
	uint16_t ThrottlePosition_b;
	uint16_t BreakPosition_a;
	uint16_t BreakPosition_b;
} MSG_PDL_Throttle_t;


/* 
*   Message packing function decelerations
*/

void pack_MAIN_Heartbeat(CanTxMsgTypeDef* msg, MSG_MAIN_Heartbeat_t* out);
void pack_MAIN_StateChange(CanTxMsgTypeDef* msg, MSG_MAIN_StateChange_t* out);
void pack_PDL_Heartbeat(CanTxMsgTypeDef* msg, MSG_PDL_Heartbeat_t* out);
void pack_DSH_Heartbeat(CanTxMsgTypeDef* msg, MSG_DSH_Heartbeat_t* out);
void pack_DSH_Buttons(CanTxMsgTypeDef* msg, MSG_DSH_Buttons_t* out);
void pack_PDL_Throttle(CanTxMsgTypeDef* msg, MSG_PDL_Throttle_t* out);

/* 
*   Message unpacking function decelerations
*/

MSG_MAIN_Heartbeat_t unpack_MAIN_Heartbeat(CanTxMsgTypeDef* message);
MSG_MAIN_StateChange_t unpack_MAIN_StateChange(CanTxMsgTypeDef* message);
MSG_PDL_Heartbeat_t unpack_PDL_Heartbeat(CanTxMsgTypeDef* message);
MSG_DSH_Heartbeat_t unpack_DSH_Heartbeat(CanTxMsgTypeDef* message);
MSG_DSH_Buttons_t unpack_DSH_Buttons(CanTxMsgTypeDef* message);
MSG_PDL_Throttle_t unpack_PDL_Throttle(CanTxMsgTypeDef* message);

#endif
