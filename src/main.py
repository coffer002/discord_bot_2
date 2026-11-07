import os
import discord
from discord.ext import commands

# Importa nossas funções unificadas
from utils.logger import log, set_global_logs
from math_tools.math_wrapper import evaluate_math_expression, set_c_logs
from welcome.welcome_message import welcome_message

MASTER_LOGS = True #default value for logs, can be changed to False to disable all logs

set_global_logs(MASTER_LOGS) 
set_c_logs(MASTER_LOGS)      


intents = discord.Intents.default()
intents.message_content = True
bot = commands.Bot(command_prefix='!', intents=intents)

@bot.event
async def on_ready():
    log("main.py", f"Bot iniciado. Logado como {bot.user.name}")
    print(f'Logged in as {bot.user.name}')

@bot.command()
async def math(ctx, *, expression):
    log("main.py", f"Comando !math chamado por {ctx.author.name}")
    try:
        result = evaluate_math_expression(expression)
        await ctx.send(f'Result: {result}')
    except Exception as e:
        log("main.py", f"Erro: {e}")
        await ctx.send(f'Error: {e}')

@bot.command()
async def welcome(ctx, *, user: discord.Member):
    log("main.py", f"Comando !welcome chamado por {ctx.author.name}")
    await ctx.send(welcome_message(user))

if __name__ == "__main__":
    log("main.py", "Iniciando o bot do Discord...")
    bot.run(os.getenv('DISCORD_TOKEN'))