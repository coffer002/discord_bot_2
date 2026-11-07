import discord
from discord.ext import commands
from welcome import welcome_message
from math_wrapper import evaluate_math_expression
import os


ENABLE_LOGS = True  

def log(message):
    if ENABLE_LOGS:
        print(f"[LOG] [main.py] {message}", flush=True) 


intents = discord.Intents.default()
intents.message_content = True

bot = commands.Bot(command_prefix='!', intents=intents)

@bot.event
async def on_ready():
    log(f"Bot iniciado com sucesso. Logado como {bot.user.name} (ID: {bot.user.id})")
    print(f'Logged in as {bot.user.name}')

@bot.command()
async def math(ctx, *, expression):
    log(f"Comando !math chamado por {ctx.author.name}. Expressão: '{expression}'")
    try:
        result = evaluate_math_expression(expression)
        log(f"Expressão avaliada com sucesso. Resultado: '{result}'")
        await ctx.send(f'Result: {result}')
    except Exception as e:
        log(f"Erro ao avaliar a expressão '{expression}': {e}")
        await ctx.send(f'Error: {e}')

@bot.command()
async def welcome(ctx, *, user: discord.Member):
    log(f"Comando !welcome chamado por {ctx.author.name} para o usuário {user.name}")
    await ctx.send(welcome_message(user))

if __name__ == "__main__":
    log("Iniciando o bot do Discord...")
    bot.run(os.getenv('DISCORD_TOKEN'))