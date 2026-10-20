# This is the code in python used on my status checker that I'm adding here to modify



import asyncio
import glob
import os
import socket
import subprocess
import time
import discord
from discord.ext import tasks

try:
    import psutil
except ImportError:
    psutil = None

# --- Configuration ---
TOKEN = "INSERT_TOKEN_HERE"  # Revoke & reset exposed token in Discord Developer Portal!

STATUS_CHANNEL_ID = 125679
ALERT_CHANNEL_ID = 34678
USER_ID_TO_PING = 34567
TEMP_CHANNEL_ID = 3456

SERVER_HOST = "deb.sizablesplash.com"
SERVER_PORT = 22   # I changed this, might need to again
SSH_USER = "root"  # Change to your SSH username on machine 'deb'
TEMP_THRESHOLD = 80.0

intents = discord.Intents.default()
client = discord.Client(intents=intents)

# State & Embed tracking
status_message = None
temp_alert_active = False

last_server_state = None  # True if online, False if offline
downtime_started_at = None # Tracked when deb goes offline
server_offline_alert_active = False


def check_remote_server(host, port=80, timeout=3):
    """Checks TCP port accessibility."""
    try:
        with socket.create_connection((host, port), timeout=timeout):
            return True
    except (socket.timeout, OSError):
        return False


def get_remote_server_boot_time(host, user):
    """Fetches exact Linux boot timestamp directly from target host via SSH."""
    try:
        cmd = ["ssh", "-o", "ConnectTimeout=3", "-o", "StrictHostKeyChecking=no", f"{user}@{host}", "date -d \"$(uptime -s)\" +%s"]
        output = subprocess.check_output(cmd, stderr=subprocess.DEVNULL, timeout=4)
        return int(output.decode().strip())
    except Exception:
        return None


def get_laptop_temp_raw():
    """Scans all Linux thermal zones and psutil entries for active CPU temperatures."""
    active_temps = []

    if psutil and hasattr(psutil, "sensors_temperatures"):
        try:
            sensor_data = psutil.sensors_temperatures()
            for key in ("coretemp", "cpu_thermal", "k10temp", "zenpower", "acpitz"):
                if key in sensor_data:
                    for entry in sensor_data[key]:
                        if entry.current and entry.current > 0:
                            active_temps.append(entry.current)
            if active_temps:
                return max(active_temps)
        except Exception:
            pass

    for zone_path in glob.glob("/sys/class/thermal/thermal_zone*"):
        try:
            with open(f"{zone_path}/temp", "r") as f:
                temp_c = float(f.read().strip()) / 1000.0
                if 15.0 <= temp_c <= 110.0:
                    active_temps.append(temp_c)
        except Exception:
            continue

    if active_temps:
        return max(active_temps)

    return None


def get_system_stats():
    """Retrieves CPU load and RAM usage if psutil is available."""
    if not psutil:
        return None, None
    try:
        cpu_load = psutil.cpu_percent(interval=None)
        mem = psutil.virtual_memory()
        ram_used_gb = mem.used / (1024 ** 3)
        ram_total_gb = mem.total / (1024 ** 3)
        
        cpu_str = f"`{cpu_load:.1f}%`"
        ram_str = f"`{mem.percent:.1f}%` (`{ram_used_gb:.2f} GB / {ram_total_gb:.2f} GB`)"
        return cpu_str, ram_str
    except Exception:
        return None, None


def build_status_embed(is_online, temp_val, remote_boot_time):
    """Constructs the Discord embed using real target system uptime."""
    is_temp_critical = temp_val is not None and temp_val >= TEMP_THRESHOLD
    color = discord.Color.red() if (not is_online or is_temp_critical) else discord.Color.green()

    embed = discord.Embed(
        title="Server Status",
        color=color
    )

    if is_online:
        status_text = "🟢 **ONLINE**"
        if remote_boot_time:
            status_text += f"\n*Online since <t:{remote_boot_time}:f> (<t:{remote_boot_time}:R>)*"
    else:
        status_text = "🔴 **OFFLINE**"
        if downtime_started_at:
            status_text += f"\n*Offline since <t:{downtime_started_at}:f> (<t:{downtime_started_at}:R>)*"

    embed.add_field(name="🌐 HP Compaq Server", value=status_text, inline=False)

    if temp_val is not None:
        temp_str = f"`{temp_val:.1f}°C`"
        if is_temp_critical:
            temp_str += " ⚠️ **CRITICAL**"
    else:
        temp_str = "`Unavailable`"

    embed.add_field(name="Status Checker Temp", value=temp_str, inline=False)

    cpu_str, ram_str = get_system_stats()
    if cpu_str and ram_str:
        embed.add_field(name="⚡ Status Checker CPU Load", value=cpu_str, inline=True)
        embed.add_field(name="🧠 Status Checker RAM Usage", value=ram_str, inline=True)
        embed.add_field(name="To see the server load", value=f"Check <#{TEMP_CHANNEL_ID}>", inline=False)

    now_unix = int(time.time())
    embed.add_field(
        name="🕐 Last Updated",
        value=f"<t:{now_unix}:t> (<t:{now_unix}:R>)",
        inline=False
    )

    return embed


@client.event
async def on_ready():
    print(f"Status Monitor Bot logged in as {client.user}")
    if not check_server_status.is_running():
        check_server_status.start()
    if not check_laptop_temp.is_running():
        check_laptop_temp.start()


@tasks.loop(seconds=15)
async def check_server_status():
    global status_message, last_server_state, downtime_started_at, server_offline_alert_active

    status_channel = client.get_channel(STATUS_CHANNEL_ID)
    alert_channel = client.get_channel(ALERT_CHANNEL_ID)
    if not status_channel:
        return

    is_online = await asyncio.to_thread(check_remote_server, SERVER_HOST, SERVER_PORT)
    temp_val = await asyncio.to_thread(get_laptop_temp_raw)
    
    remote_boot_time = None
    current_time = int(time.time())

    if is_online:
        remote_boot_time = await asyncio.to_thread(get_remote_server_boot_time, SERVER_HOST, SSH_USER)
        downtime_started_at = None
    else:
        if last_server_state is True or downtime_started_at is None:
            downtime_started_at = current_time

    last_server_state = is_online

    # Fire offline alert notification ping
    if not is_online and not server_offline_alert_active:
        server_offline_alert_active = True
        if alert_channel:
            msg = (
                f"🚨 <@{USER_ID_TO_PING}> **SERVER OFFLINE ALERT**\n"
                f"The HP Compaq Server went offline at <t:{current_time}:t>!"
            )
            await alert_channel.send(msg)

    # Fire back-online notification ping
    elif is_online and server_offline_alert_active:
        server_offline_alert_active = False
        if alert_channel:
            msg = (
                f"✅ **SERVER RECOVERED**\n"
                f"The HP Compaq Server is back online."
            )
            await alert_channel.send(msg)

    embed = build_status_embed(is_online, temp_val, remote_boot_time)

    if status_message is None:
        async for msg in status_channel.history(limit=20):
            if msg.author == client.user and msg.embeds:
                status_message = msg
                break

    if status_message:
        try:
            await status_message.edit(embed=embed)
        except discord.NotFound:
            status_message = await status_channel.send(embed=embed)
    else:
        status_message = await status_channel.send(embed=embed)


@tasks.loop(seconds=15)
async def check_laptop_temp():
    global temp_alert_active

    alert_channel = client.get_channel(ALERT_CHANNEL_ID)
    if not alert_channel:
        return

    temp_val = await asyncio.to_thread(get_laptop_temp_raw)
    if temp_val is None:
        return

    if temp_val >= TEMP_THRESHOLD and not temp_alert_active:
        temp_alert_active = True
        msg = (
            f"🚨 <@{USER_ID_TO_PING}> **HIGH TEMPERATURE ALERT**\n"
            f"Laptop CPU temperature is critical: **{temp_val:.1f}°C**!"
        )
        await alert_channel.send(msg)

    elif temp_val < (TEMP_THRESHOLD - 5.0) and temp_alert_active:
        temp_alert_active = False
        msg = (
            f"❄️ **TEMPERATURE NORMALIZED**\n"
            f"Laptop CPU cooled down to **{temp_val:.1f}°C**."
        )
        await alert_channel.send(msg)


@check_server_status.before_loop
@check_laptop_temp.before_loop
async def before_checks():
    await client.wait_until_ready()


client.run(TOKEN)