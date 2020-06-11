"""
Python CAN DBC C source generator.
"""

import sys
import math
import re
import cantools
from collections import OrderedDict
from cantools.database.can.message import Message
from cantools.database.can.signal import Signal


def get_bit_len(bits_min: int) -> int:
    """
    Return the minimum number of bits required ot represent an integer with the given bit length
    :param bits_min: Data bit length
    :return: byte friendly size
    """
    # return 2^(ceil[log2(x])
    return max([int(2 ** math.ceil(math.log2(bits_min))), 8])


def generate_value_table(table_name: str, values: OrderedDict) -> str:
    """
    Given a value table, create struct typedefs for each table with a given name
    :param table_name: Name description of the table
    :param values: Ordered list of value descriptions. See cantools ValueTables
    :return: Generated code block
    """
    out = f"typedef enum {table_name.upper()}_t {{\n"
    # cantools gives us the ids in descending order, best to have them in ascending order for enum creation.
    out += ',\n'.join([f"\t{name.upper()} = {val}" for val, name in reversed(values.items())])
    out += '\n}\n'
    return out


def generate_message_typedef(message: Message, per_mode: bool = True) -> str:
    """
    Generate message typedef for a CAN message.
    :param message: Message object with signal information. See cantools Message
    :param per_mode: Enable signal reformatting for PER DBC formats
    :return: Generated code block
    """
    out = "typedef struct {\n"

    signal: Signal
    for signal in message.signals:
        bit_len = get_bit_len(signal.length)
        sign = "" if signal.is_signed else "u"

        # PER signals have the message sender as a prefix for DBC creation. Remove the first xyzw_ from the signal
        # name. To avoid this feature, do not place any _'s in the message if the message does not follow this format
        name = re.sub("[a-zA-Z]*[_]", "", signal.name, count=1) \
            if per_mode and re.match("[a-zA-Z]*_[\\S]*", signal.name) \
            else signal.name

        out += f"\t{sign}int{bit_len}_t {name};\n"
    out += f"}} MSG_{message.name}_t;\n\n"
    return out


if __name__ == "__main__":
    my_db = cantools.db.load_file(filename=sys.argv[1])
    # Generate file header

    # Generate ValueTables Header
    # Generate ValueTables
    value_tables = ''.join([
        generate_value_table(table_name, values)
        for table_name, values in my_db.dbc.value_tables.items()
        ])

    # Generate message structs
    message_structs = ''.join([
        generate_message_typedef(message)
        for message in my_db.messages
    ])

    output = "\n\n".join([value_tables, message_structs])
    print(output)


