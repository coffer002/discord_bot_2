import ctypes

libmath = ctypes.CDLL('/usr/local/lib/libmath.so')

libmath.evaluate_math_expression.argtypes = [ctypes.c_char_p]
libmath.evaluate_math_expression.restype = ctypes.c_double

def evaluate_math_expression(expression):
    result = libmath.evaluate_math_expression(expression.encode('utf-8'))
    return result
