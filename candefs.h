
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
// Traction_Control_State enum options
typedef enum {
	TC_Enabled = 1U,
	TC_Disabled = 0U,
} Traction_Control_State_t;

// Calibrate_Pedal_Position enum options
typedef enum {
	CAL_Brake_Low = 4U,
	CAL_Brake_High = 3U,
	CAL_Throttle_Low = 2U,
	CAL_Thottle_High = 1U,
	CAL_Clear = 0U,
} Calibrate_Pedal_Position_t;

// APPS_State enum options
typedef enum {
	APPS_Asserted = 1U,
	APPS_Not_Asserted = 0U,
} APPS_State_t;

// Car_State enum options
typedef enum {
	CS_Recover = 5U,
	CS_Error = 4U,
	CS_Ready_To_Drive = 3U,
	CS_Reset = 2U,
	CS_Pre_Ready_To_Drive = 1U,
	CS_Init = 0U,
} Car_State_t;

// Precharge_State enum options
typedef enum {
	PC_Done = 2U,
	PC_Precharging = 1U,
	PC_Idle = 0U,
} Precharge_State_t;



/*
*   Message Struct Definitions
*/


typedef union {
	uint64_t all_data;
	uint8_t bytes[8];
	struct {
		uint64_t Accumulator_I : 12;  // Instantanious accumulator current measurment	 Range: [0, 4095]	
		uint64_t padding_12 : 4;
		uint64_t Accumulator_SOC : 12;  // Instantanious accumulator SOC estmation	
		uint64_t padding_28 : 4;
		uint64_t Accumulator_V : 12;  // Instantanious accumulator voltage measurment	 Range: [0, 4095]	
		uint64_t padding_44 : 20;
	} fields;
} BMS_AccumulatorData_t;


typedef union {
	uint64_t all_data;
	uint8_t bytes[8];
	struct {
		uint64_t Accumulator_I_Max : 12;  // Accumulator Current maximum as set by BMS	 Range: [0, 4095]	
		uint64_t padding_12 : 4;
		uint64_t Accumulator_V_Max : 12;  // Accumulator Voltage maximum as set by BMS	
		uint64_t padding_28 : 4;
		uint64_t Accumulator_V_Min : 12;  // Accumulator Voltage minimum as set by BMS	
		uint64_t padding_44 : 20;
	} fields;
} BMS_AccumulatorLimits_t;


typedef union {
	uint64_t all_data;
	uint8_t bytes[8];
	struct {
		uint64_t Cell_Temperature_Average : 12;  // Average cell temperature measurement	 Range: [0, 4095]	
		uint64_t padding_12 : 4;
		uint64_t Cell_Temperature_High : 12;  // Highest cell temperature measurement	 Range: [0, 4095]	
		uint64_t padding_28 : 4;
		uint64_t Cell_Temperature_Low : 12;  // Lowest cell temperature measurement	 Range: [0, 4095]	
		uint64_t padding_44 : 20;
	} fields;
} BMS_CellTemperatures_t;


typedef union {
	uint64_t all_data;
	uint8_t bytes[8];
	struct {
		uint64_t Cell_Voltage_High : 12;  // Highest cell voltage measurement	 Range: [0, 4095]	
		uint64_t padding_12 : 4;
		uint64_t Cell_Voltage_Low : 12;  // Lowest cell voltage measurement	 Range: [0, 4095]	
		uint64_t padding_28 : 4;
		uint64_t Cell_Voltage_Average : 12;  // Average cell voltage measurement	 Range: [0, 4095]	
		uint64_t padding_44 : 20;
	} fields;
} BMS_CellVoltages_t;


typedef union {
	uint64_t all_data;
	uint8_t bytes[8];
	struct {
		uint64_t Traction_Control_State : 1;  // Tracton control enable status	 Range: [0, 1]	
		uint64_t padding_1 : 63;
	} fields;
} Dash_TCButton_t;


typedef union {
	uint64_t all_data;
	uint8_t bytes[8];
	struct {
		uint64_t Car_Start : 1;  // Start command to begin ready to drive transition	 Range: [0, 1]	
		uint64_t padding_1 : 63;
	} fields;
} Dash_StartButton_t;


typedef union {
	uint64_t all_data;
	uint8_t bytes[8];
	struct {
		uint64_t Calibrate_Pedal_Position : 3;  // Calibrate min/max throttle travel position	
		uint64_t padding_3 : 61;
	} fields;
} Dash_PedalCalibration_t;


typedef union {
	uint64_t all_data;
	uint8_t bytes[8];
	struct {
		uint64_t APPS_State : 8;  // Accelerator Pedal Position Sensor plausibility state	 Range: [0, 255]	
		uint64_t Car_State : 8;  // Car drive state	 Range: [0, 255]	
		uint64_t Precharge_State : 8;  // Car HV bus Precharge state	
		uint64_t padding_24 : 40;
	} fields;
} Main_Heartbeat_t;


typedef union {
	uint64_t all_data;
	uint8_t bytes[8];
	struct {
		uint64_t Brake_Position_A : 12;  // Position of the throttle pedal	 Units: %	 Range: [0, 4095]	
		uint64_t padding_12 : 4;
		uint64_t Brake_Position_B : 12;  // Position of the brake pedal	 Units: %	 Range: [0, 4095]	
		uint64_t padding_28 : 4;
		uint64_t Throttle_Position_A : 12;  // Position of the throttle pedal	 Units: %	 Range: [0, 4095]	
		uint64_t padding_44 : 4;
		uint64_t Throttle_Position_B : 12;  // Position of the throttle pedal	 Units: %	 Range: [0, 4095]	
		uint64_t padding_60 : 4;
	} fields;
} Pedal_Positions_t;




#endif
