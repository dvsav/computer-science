import json
import random

def random_bigint(bitlen: int = 128) -> int:
    """Generate a random signed integer of up to 'bitlen' bits."""
    assert bitlen > 0, "bitlen must be > 0"
    n = random.getrandbits(bitlen)
    return n if random.random() < 0.5 else -n

def gen_binary_ops(
    op_name: str,
    py_op: callable,
    count: int = 10,
    allow_zero_rhs: bool = True) -> list[dict]:
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
                "format" : "dec",
                "args": [str(a), str(b)],
                "expected": str(result)
            })
        except Exception:
            continue
    return vectors

def main():
    test_vectors = []

    # Arithmetic
    test_vectors += gen_binary_ops("+", lambda a, b: a + b)
    test_vectors += gen_binary_ops("-", lambda a, b: a - b)
    test_vectors += gen_binary_ops("*", lambda a, b: a * b)
    test_vectors += gen_binary_ops("/", lambda a, b: a // b, allow_zero_rhs=False)
    test_vectors += gen_binary_ops("%", lambda a, b: a % b,  allow_zero_rhs=False)

    # Bitwise
    test_vectors += gen_binary_ops("&", lambda a, b: a & b)
    test_vectors += gen_binary_ops("|", lambda a, b: a | b)
    test_vectors += gen_binary_ops("^", lambda a, b: a | b)

    # Save to JSON
    with open("very_long_integer_test_vectors.json", "w") as f:
        json.dump(test_vectors, f, indent=2)

    print(f"Generated {len(test_vectors)} test vectors.")

if __name__ == "__main__":
    main()
