from utils.logger import log

def welcome_message(user):
    log("welcome.py", f"Gerando boas-vindas para: {user.name}")
    return f'Welcome, {user.mention}!'