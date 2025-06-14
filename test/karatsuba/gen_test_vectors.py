#!/usr/bin/env python3

import json
import random

def cpp_div(a: int, b: int) -> int:
    """Returns the result of integral division a / b that aligns with C++ integral division behavior"""
    return int(a / b)

def cpp_mod(a: int, b: int) -> int:
    """Returns the result of modulo operation a % b that aligns with C++ modulo operation behavior"""
    return a % b if a * b >= 0 else (a % b) - b

def random_bigint(bitlen: int = 128) -> int:
    """Generate a random signed integer of up to 'bitlen' bits."""
    assert bitlen > 0, "bitlen must be > 0"
    n = random.getrandbits(bitlen)
    return n if random.random() < 0.5 else -n

def twos_complement(num: int):
    if (num < 0):
        n_tetrades = len(hex(num)) - 3
        n_tetrades += n_tetrades % 2 # round up to the closest even number
        n_bytes = n_tetrades // 2
        N = n_bytes * 8
        num = num & ((1 << N) - 1)
    return num

def format_number(num: int, format: str) -> str:
    if format == "hex":
        return hex(twos_complement(num))[2:].upper()
    elif format == "bin":
        return bin(twos_complement(num))[2:].upper()
    else:
        return str(num)

def gen_binary_ops(
    op_name: str,
    py_op: callable,
    count: int = 10,
    allow_zero_rhs: bool = True,
    format = "dec") -> list[dict]:
    """
    Generates a list of test vectors for a binary operation.

    Args:
        op_name (str): The name of the operation to be tested.
        py_op (callable): A Python function implementing the binary operation, taking two arguments.
        count (int, optional): The number of test vectors to generate. Defaults to 50.
        allow_zero_rhs (bool, optional): If False, excludes test cases where the right-hand side operand is zero. Defaults to True.

    Returns:
        list[dict]: A list of dictionaries, each containing:
            - "function": The operation name.
            - "format": The format of numbers (args and expected) can be "dec", "hex" or "bin".
            - "args": A list with the two formatted operands.
            - "expected": The formatted expected result of the operation.
    """
    vectors = []
    for _ in range(count):
        a = random_bigint()
        b = random_bigint()
        if not allow_zero_rhs and b == 0:
            continue
        try:
            result = py_op(a, b)
            vectors.append({
                "function": op_name,
                "format" : format,
                "args": [format_number(a, format), format_number(b, format)],
                "expected": format_number(result, format)
            })
        except Exception as ex:
            continue
    return vectors

def main():
    test_vectors = []

    # Arithmetic
    test_vectors += gen_binary_ops(op_name = "+", py_op = lambda a, b: a + b)
    test_vectors += gen_binary_ops(op_name = "-", py_op = lambda a, b: a - b)
    test_vectors += gen_binary_ops(op_name = "*", py_op = lambda a, b: a * b)
    test_vectors += gen_binary_ops(op_name = "/", py_op = lambda a, b: cpp_div(a, b), allow_zero_rhs=False)
    test_vectors += gen_binary_ops(op_name = "%", py_op = lambda a, b: cpp_mod(a, b), allow_zero_rhs=False)

    # Bitwise
    test_vectors += gen_binary_ops(op_name = "&", py_op = lambda a, b: a & b, format = "hex")
    test_vectors += gen_binary_ops(op_name = "|", py_op = lambda a, b: a | b, format = "hex")
    test_vectors += gen_binary_ops(op_name = "^", py_op = lambda a, b: a ^ b, format = "hex")

    # Save to JSON
    with open("very_long_integer_test_vectors.json", "w") as f:
        json.dump(test_vectors, f, indent=2)

    print(f"Generated {len(test_vectors)} test vectors.")

if __name__ == "__main__":
    main()
