ENABLE_LOGS = True  
def log(message):
    if ENABLE_LOGS:
        print(f"[LOG] [welcome_message.py] {message}", flush=True) 


def welcome_message(user):
    log(f"Gerando mensagem de boas-vindas para o usuário: {user.name} (ID: {user.id})")
    return f'Welcome, {user.mention}!'