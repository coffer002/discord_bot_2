import ctypes
from utils.logger import log

log("math_wrapper.py", "Carregando a biblioteca libmath.so...")
try:
    libmath = ctypes.CDLL('/usr/local/lib/libmath.so')
    log("math_wrapper.py", "Biblioteca carregada com sucesso.")
except Exception as e:
    log("math_wrapper.py", f"Erro ao carregar a biblioteca: {e}")
    raise

# Define as assinaturas das funções do C
libmath.evaluate_math_expression.argtypes = [ctypes.c_char_p]
libmath.evaluate_math_expression.restype = ctypes.c_char_p  
libmath.set_math_logs.argtypes = [ctypes.c_int] # Nova função de log

# Função para o main.py controlar os logs do C
def set_c_logs(enabled: bool):
    libmath.set_math_logs(1 if enabled else 0)

def evaluate_math_expression(expression):
    log("math_wrapper.py", f"Preparando para enviar expressão ao C: '{expression}'")
    raw_result = libmath.evaluate_math_expression(expression.encode('utf-8'))
    decoded_result = raw_result.decode('utf-8')
    log("math_wrapper.py", f"Resultado recebido do C: '{decoded_result}'")
    return decoded_result