import discord
from discord.ext import commands
from welcome import welcome_message
from math_wrapper import evaluate_math_expression
import os

intents = discord.Intents.default()
intents.message_content = True

bot = commands.Bot(command_prefix='!', intents=intents)

@bot.event
async def on_ready():
    print(f'Logged in as {bot.user.name}')

@bot.command()
async def math(ctx, *, expression):
    try:
        result = evaluate_math_expression(expression)
        await ctx.send(f'Result: {result}')
    except Exception as e:
        await ctx.send(f'Error: {e}')

@bot.command()
async def welcome(ctx, *, user: discord.Member):
    await ctx.send(welcome_message(user))

bot.run(os.getenv('DISCORD_TOKEN'))
