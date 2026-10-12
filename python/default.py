import datetime
import zoneinfo
import discord
from discord.ext import commands, tasks
import config

# Enable necessary intents
intents = discord.Intents.default()
intents.message_content = True

# Initialize the bot
bot = commands.Bot(command_prefix='!', intents=intents)

# Configuration
REMINDER_CHANNEL_ID = 1544347450607796334  # Integer, not string

# Define your time zone and schedule time
TIME_ZONE = zoneinfo.ZoneInfo("America/Chicago")
REMINDER_TIME = datetime.time(hour=9, minute=25, second=0, tzinfo=TIME_ZONE)
REMINDER_DAYS = [1, 3]  # 0 = Monday, 1 = Tuesday, etc.


# Background Task
@tasks.loop(time=REMINDER_TIME)
async def scheduled_post():
    today = datetime.datetime.now().weekday()
    if today in REMINDER_DAYS:
        channel = bot.get_channel(REMINDER_CHANNEL_ID)
        if channel:
            await channel.send("Public Speaking begins in 5 minutes!!!!")
            print("Scheduled message sent successfully.")
        else:
            print("Could not find the specified channel ID.")


@scheduled_post.before_loop
async def before_scheduled_post():
    await bot.wait_until_ready()


# Bot Event Handlers
@bot.event
async def on_ready():
    print(f'Bot is ready. Logged in as {bot.user}')
    # Start the background task if it isn't running already
    if not scheduled_post.is_running():
        scheduled_post.start()


# Bot Commands
@bot.command()
async def ping(ctx):
    await ctx.send('Pong!')


@bot.command()
async def neil(ctx):
    await ctx.send('NEILL!!!!!!!!!!')


# Run the bot
bot.run(config.TOKEN)