import ../../config
import discord

client = discord.Client(intents=discord.Intents.default())

@client.event
async def on_ready():
  activity = discord.CustomActivity(name="Portable Sports")  # this is the status
  await client.change_presence(activity=activity)
  print(f"Logged in as {client.user} with custom status set!")

client.run(TOKEN)
