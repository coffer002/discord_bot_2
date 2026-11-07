# logger.py
ENABLE_LOGS = True

def set_global_logs(state: bool):
    global ENABLE_LOGS
    ENABLE_LOGS = state

def log(module_name: str, message: str):
    if ENABLE_LOGS:
        print(f"[LOG] [{module_name}] {message}", flush=True)