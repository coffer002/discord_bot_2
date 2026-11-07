import ctypes

ENABLE_LOGS = True  

def log(message):
    if ENABLE_LOGS:
        print(f"[LOG] [math_wrapper.py] {message}", flush=True) 

log("Carregando a biblioteca libmath.so...")
try:
    libmath = ctypes.CDLL('/usr/local/lib/libmath.so')
    log("Biblioteca carregada com sucesso.")
except Exception as e:
    log(f"Erro ao carregar a biblioteca: {e}")
    raise

libmath.evaluate_math_expression.argtypes = [ctypes.c_char_p]
libmath.evaluate_math_expression.restype = ctypes.c_char_p  

def evaluate_math_expression(expression):
    log(f"Preparando para enviar expressão ao C: '{expression}'") 

    raw_result = libmath.evaluate_math_expression(expression.encode('utf-8'))
    decoded_result = raw_result.decode('utf-8')

    log(f"Resultado decodificado recebido do C: '{decoded_result}'")
    
    return decoded_result