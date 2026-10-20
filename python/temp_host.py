# the temp code from the proxmox host. Leaving here to modify.



import discord
from discord.ext import commands, tasks
import psutil
import datetime
import asyncio
import logging

# Set up basic logging so task errors show cleanly in journalctl
logging.basicConfig(level=logging.INFO)

# --- CONFIGURATION ---
TOKEN = 'BOT_TOKEN_HERE'
CHANNEL_ID = 143654343564  # Replace with target channel ID

# Set to 30s to safely avoid Discord's per-message edit rate limits
UPDATE_INTERVAL = 30  

intents = discord.Intents.default()
bot = commands.Bot(command_prefix="!", intents=intents)

# Global variable to store the persistent status message
status_message = None

def get_server_temperatures():
    """Reads system temperature metrics via psutil."""
    try:
        temps = psutil.sensors_temperatures()
        if not temps:
            return "No temperature sensors found."
        
        formatted_temps = []
        for sensor_name, entries in temps.items():
            for entry in entries:
                label = entry.label or sensor_name
                current = entry.current
                high = entry.high or "N/A"
                critical = entry.critical or "N/A"
                formatted_temps.append(f"**{label}**: `{current}°C` (Max: {high}°C | Crit: {critical}°C)")
        
        return "\n".join(formatted_temps)
    except Exception as e:
        return f"Error reading sensors: {str(e)}"

@bot.event
async def on_ready():
    print(f'Logged in as {bot.user.name} ({bot.user.id})')
    # Start the background status loop if not already running
    if not update_temperature_display.is_running():
        update_temperature_display.start()

@tasks.loop(seconds=UPDATE_INTERVAL)
async def update_temperature_display():
    global status_message
    
    channel = bot.get_channel(CHANNEL_ID)
    if not channel:
        return

    # Gather system metrics
    temp_info = get_server_temperatures()
    cpu_usage = psutil.cpu_percent()
    ram = psutil.virtual_memory()

    # Determine status color (Green < 60°C/85% CPU, Gold 60-85%, Red > 85%)
    embed_color = discord.Color.green()
    if cpu_usage > 85:
        embed_color = discord.Color.red()
    elif cpu_usage > 60:
        embed_color = discord.Color.gold()

    # Unix timestamp for Discord's dynamic relative time formatting
    now_unix = int(datetime.datetime.now(datetime.timezone.utc).timestamp())

    # Construct status embed
    embed = discord.Embed(
        title="🖥️ Real-Time Server Hardware Monitor",
        color=embed_color,
        timestamp=datetime.datetime.now(datetime.timezone.utc)
    )
    embed.add_field(name="🌡️ Temperatures", value=temp_info, inline=False)
    embed.add_field(name="⚡ CPU Load", value=f"`{cpu_usage}%`", inline=True)
    embed.add_field(name="🧠 RAM Usage", value=f"`{ram.percent}%` ({round(ram.used / 1e9, 2)} GB / {round(ram.total / 1e9, 2)} GB)", inline=True)
    
    # Live updating relative timestamp on Discord UI
    embed.add_field(name="🕒 Last Updated", value=f"<t:{now_unix}:R>", inline=False)
    embed.set_footer(text="Auto-updating • Discord API rate-limit compliant")

    # Locate existing message or send a new one
    if status_message is None:
        async for message in channel.history(limit=10):
            if message.author == bot.user and message.embeds:
                status_message = message
                break
        
        if status_message is None:
            status_message = await channel.send(embed=embed)
            return

    # Edit existing message in-place
    try:
        await status_message.edit(embed=embed)
    except discord.NotFound:
        # Re-create message if deleted manually
        status_message = await channel.send(embed=embed)

@update_temperature_display.before_loop
async def before_update():
    await bot.wait_until_ready()

# Error handler to prevent silent freezes on 503/HTTP exceptions
@update_temperature_display.error
async def update_temperature_display_error(error):
    logging.error(f"Unhandled error in update_temperature_display loop: {error}")
    await asyncio.sleep(15)
    update_temperature_display.restart()

bot.run(TOKEN)