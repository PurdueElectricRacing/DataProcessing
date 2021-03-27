
#ifndef _PER_2021_CAN_H
#define _PER_2021_CAN_H

#include "stdint.h"

/** [[[cog
# Setup COG variables. Load DBC from directory.
# make sure "-Ddbc_filepath=<PATH_TO_DBC>" is defined when run from the command line

import sys
import math
import re
import os
import cantools
from collections import OrderedDict
from cantools.database.can.message import Message
from cantools.database.can.signal import Signal
import cog

db = cantools.db.load_file(filename=dbc_filepath)
name = os.path.basename(sys.argv[1]).split(".")[0]

]]] */
// [[[end]]]

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
/** [[[cog

for message in db.messages:
	for signal in message.signals:
		if signal.choices:
			cog.outl(f"// {signal.name} enum options")
			cog.outl("typedef enum {")
			[cog.outl(f"\t{name} = {val}U,") for val, name in signal.choices.items()]
			cog.outl(f"}} {signal.name}_t;\n")
]]] */
// [[[end]]]


/*
*   Message Struct Definitions
*/
/** [[[cog

def add_field(signal=None, name=None, length=None):
	if signal:
		name = signal.name
		length = signal.length
	cog.out("\t\tuint64_t ")
	cog.out(str(name))
	cog.out(" : " +str(length) + f";")

def add_field_doc(signal):
	
	cog.out("//")
	if signal.comment: 
		cog.out(f" {signal.comment}\t")
	if signal.unit: 
		cog.out(f" Units: {signal.unit}\t")
	if signal.minimum or signal.maximum: 
		cog.out(f" Range: [{signal.minimum}, {signal.maximum}]\t")


def out_from_signal_list(signals, message_name):
	cog.outl(
"""

typedef union {
	uint64_t all_data;
	uint8_t bytes[8];
	struct {""")

	current_message_bit = 0
	for signal in signals:
		if signal.start != current_message_bit:
			add_field(name="padding_"+str(current_message_bit), length=signal.start - current_message_bit)
			cog.outl("")

		add_field(signal)
		cog.out("  ")
		add_field_doc(signal)
		current_message_bit = signal.start + signal.length
		cog.outl("")
	
	if current_message_bit != 64:
		add_field(name="padding_"+str(current_message_bit), length=64 - current_message_bit)

	cog.outl("\n\t} fields;\n} " + message_name+ ";")

for message in (message for message in db.messages if not message.is_multiplexed()):
	out_from_signal_list(message.signals, message.name + "_t")

]]] */
// [[[end]]]




#endif
