"""
Python CAN DBC C source generator.
"""

import sys
import math
import re
import os
import cantools
from typing import List, Tuple
from collections import OrderedDict
from cantools.database.can.message import Message
from cantools.database.can.signal import Signal


CAN_MSG_TYPE = "CanTxMsgTypeDef"

CAN_MSG_TYPEDEF = f"""
typedef struct
{{
  uint16_t StdId; /*!< Specifies the standard identifier. */
  uint32_t ExtId; /*!< Specifies the extended identifier. */
  uint32_t IDE; /*!< Specifies the type of identifier for the message that will be transmitted.  */
  uint32_t DLC; /*!< Specifies the length of the frame that will be transmitted. */
  uint8_t Data[8]; /*!< Contains the data to be transmitted. */
}} {CAN_MSG_TYPE};
"""

dbc_db = None


def get_bit_len(bits_min: int) -> int:
    """
    Return the minimum number of bits required ot represent an integer with the given bit length
    :param bits_min: Data bit length
    :return: byte friendly size
    """
    # return 2^(ceil[log2(x])
    return max([int(2 ** math.ceil(math.log2(bits_min))), 8])


def strip_signal_name(signal_n: str):
    """
    Remove prepended signal source from name
    :param signal_n:
    :return:
    """
    return re.sub("[a-zA-Z]*[_]", "", signal_n, count=1) if re.match("[a-zA-Z]*_[\\S]*", signal_n) else signal_n


def get_signal_types(signals: List[Signal], value_tables: OrderedDict, per_mode: bool = True) -> List[Tuple[str, str]]:
    """

    :param signals:
    :param per_mode:
    :param value_tables:
    :return List[Tuple[str, str]]:
    """
    out = []
    for signal in signals:
        
        bit_len = get_bit_len(signal.length)
        sign = "" if signal.is_signed else "u"

        # PER signals have the message sender as a prefix for DBC creation. Remove the first xyzw_ from the signal
        # name. To avoid this feature, do not place any _'s in the message if the message does not follow this format
        sig_name = strip_signal_name(signal.name) if per_mode is True else signal.name
        dtype = f"{sign}int{bit_len}_t"

        if signal.choices is not None:

            for table_name, table in value_tables:
                table_vals = set([table_entry for i, table_entry in table.items()])
                signal_vals = set([table_entry for i, table_entry in signal.choices.items()])

                if table_vals.difference(signal_vals) == set([]):
                    dtype = table_name.upper() + "_t"
                    continue

        out.append((dtype, sig_name))
    return out


def get_message_dtype(message: Message):
    return f"MSG_{message.name}_t"


def generate_value_table(table_name: str, values: OrderedDict) -> str:
    """
    Given a value table, create struct typedefs for each table with a given name
    :param table_name: Name description of the table
    :param values: Ordered list of value descriptions. See cantools ValueTables
    :return: Generated code block
    """
    out = "typedef enum {\n"
    # cantools gives us the ids in descending order, best to have them in ascending order for enum creation.
    out += ',\n'.join([f"\t{name.upper()} = {val}" for val, name in reversed(values.items())])
    out += f"\n}} {table_name.upper()}_t; \n"
    return out


def generate_message_typedef(message: Message, value_tables: OrderedDict, per_mode: bool = True) -> str:
    """
    Generate message typedef for a CAN message.
    :param message: Message object with signal information. See cantools Message
    :param per_mode: Enable signal reformatting for PER DBC formats
    :return: Generated code block
    """
    out = "typedef struct {\n"

    signal: Signal
    out += '\n'.join([f"\t{dtype} {name};" for dtype, name in get_signal_types(message.signals, value_tables)])

    out += f"\n}} {get_message_dtype(message)};\n"
    return out


def generate_packing_function_deceleration(message: Message):
    """
    Generate packing function header for a given message
    :param message:
    :return:
    """
    out = ""
    out += f"void pack_{message.name}({CAN_MSG_TYPE}* msg, {get_message_dtype(message)}* out);"
    return out


def generate_unpacking_function_decelerations(message: Message):
    """
    Generate packing function header for a given message
    :param message:
    :return:
    """
    out = ""
    out += f"{get_message_dtype(message)} unpack_{message.name}({CAN_MSG_TYPE}* message);"
    return out


def GenerateHeaderFile(my_db, dbc_name: str):

    # Generate ValueTables Section
    value_tables_header = \
        """/* 
*   Value Table Struct Definitions
*/
"""
    value_tables = '\n'.join([
        generate_value_table(table_name, values)
        for table_name, values in my_db.dbc.value_tables.items()
    ])
    values_section = '\n'.join([value_tables_header, value_tables])

    # Generate Message structs Section
    message_structs_header = \
        """/* 
*   Message Struct Definitions
*/
"""
    message_structs = '\n'.join([
        generate_message_typedef(message, my_db.dbc.value_tables.items())
        for message in my_db.messages
    ])
    struct_section = '\n'.join([message_structs_header, message_structs])

    # Generate message pack function decelerations section
    pack_header = \
        """/* 
*   Message packing function decelerations
*/
"""
    pack = '\n'.join(generate_packing_function_deceleration(message) for message in my_db.messages)
    pack_section = '\n'.join([pack_header, pack])

    # Generate message unpack function decelerations section
    unpack_header = \
        """/* 
*   Message unpacking function decelerations
*/
"""
    unpack = '\n'.join(generate_unpacking_function_decelerations(message) for message in my_db.messages)
    unpack_section = '\n'.join([unpack_header, unpack])
    output = \
        f"""
#ifndef _{dbc_name}_CAN_H
#define _{dbc_name}_CAN_H

#include "stdint.h"

{CAN_MSG_TYPEDEF}

{values_section}

{struct_section}

{pack_section}

{unpack_section}

#endif
"""

    with open(f"{dbc_name}_PER.h", mode='w') as header_output_file:
        header_output_file.write(output)


def generate_pack_method(message: Message, per_mode=True) -> str:

    out = generate_packing_function_deceleration(message)[:-1]
    out += " {\n"

    prev_bits_added = 0
    total_length = 0

    signal: Signal
    for signal in message.signals:
        sig_name = strip_signal_name(signal.name) if per_mode else signal.name
        signal_remaining = signal.length

        byte_fields = range(math.floor(total_length / 8), math.ceil((total_length + signal.length) / 8))
        first_insert = True
        for byte_index in byte_fields:

            bits_in_byte = total_length % 8
            # add up to 8 bits, do not overlap the 8bit boundry in the Data field
            bits_to_add = 8 - bits_in_byte if (bits_in_byte + signal_remaining) > 8 else signal_remaining
            value_mask = hex(((2 ** bits_to_add) - 1) << (signal.length - signal_remaining))

            shift_amt = '' if bits_in_byte == 0 and first_insert else prev_bits_added
            # When adding the first bits of a signal, we will only need to left shift to not
            # overlap the previous signal upper bits
            shift_dir = '' if shift_amt == '' else " << " if first_insert else " >> "

            oper = "=" if bits_in_byte == 0 else "|="

            out += f"\tmsg->Data[{byte_index}] {oper} (out->{sig_name} & {value_mask}){shift_dir}{shift_amt};\n"
            prev_bits_added = bits_to_add
            signal_remaining -= bits_to_add
            total_length += bits_to_add
            first_insert = False

    out += "}\n"

    return out


def GenerateSourceFile(my_db, dbc_name: str) -> None:

    pack_methods = '\n'.join([generate_pack_method(message) for message in my_db.messages])
    unpack_methods = ""

    output = \
        f"""
#include "{dbc_name}_PER.h"

{pack_methods}

{unpack_methods}

"""
    with open(f"{dbc_name}_PER.c", mode='w') as header_output_file:
        header_output_file.write(output)


if __name__ == "__main__":

    dbc_db = cantools.db.load_file(filename=sys.argv[1])
    name = os.path.basename(sys.argv[1]).split(".")[0]

    GenerateHeaderFile(my_db=dbc_db, dbc_name=name)
    GenerateSourceFile(my_db=dbc_db, dbc_name=name)


