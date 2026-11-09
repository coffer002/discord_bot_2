import os
import discord
from discord.ext import commands

from utils.logger import log, set_global_logs
from math_tools.math_wrapper import evaluate_math_expression, set_c_logs
from welcome.welcome_message import welcome_message

MASTER_LOGS = True

set_global_logs(MASTER_LOGS)
set_c_logs(MASTER_LOGS)

intents = discord.Intents.default()
intents.message_content = True
intents.members = True

bot = commands.Bot(command_prefix='!', intents=intents)

@bot.event
async def on_ready():
    log("main.py", f"Bot iniciado. Logado como {bot.user.name}")
    print(f'Logged in as {bot.user.name}')

@bot.event
async def on_member_join(member: discord.Member):
    log("main.py", f"Novo membro entrou: {member.name}")
    channel_id = os.getenv('WELCOME_CHANNEL_ID')

    if not channel_id:
        log("main.py", "WELCOME_CHANNEL_ID não configurado no .env")
        return

    channel = bot.get_channel(int(channel_id))
    if not channel:
        try:
            channel = await bot.fetch_channel(int(channel_id))
        except Exception as e:
            log("main.py", f"Erro ao buscar canal de boas-vindas: {e}")
            return

    text, image_path = await welcome_message(member)

    if image_path and os.path.exists(image_path):
        await channel.send(content=text, file=discord.File(image_path))
        os.remove(image_path)  
    else:
        await channel.send(content=text)

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
async def welcome(ctx, user: discord.Member = None):
    user = user or ctx.author
    log("main.py", f"Comando !welcome chamado para {user.name}")
    text, image_path = await welcome_message(user)

    if image_path and os.path.exists(image_path):
        await ctx.send(content=text, file=discord.File(image_path))
        os.remove(image_path)
    else:
        await ctx.send(content=text)

if __name__ == "__main__":
    log("main.py", "Iniciando o bot do Discord...")
    bot.run(os.getenv('DISCORD_TOKEN'))