#include "config.h"

int create_conf(void)
{
  FILE *fp = fopen(BOT_CONF, "w");
  if (fp == NULL)
  {
    printf("Failed to open config file\n");
    return -1;
  }

  fprintf(fp,
    "# Hello! Welcome to my discord bot commands. This config will likely be changed and updated in future releases. Make sure to delete this file each time\n"
    "# This config file allows you the downloader to use my code without me exposing my token or you compiling it yourself.\n"
    "# This program is exclusive to the POSIX operating systems thanks to the header sys/stat.h so this will not work on Windows. Actually, I just looked it up, this code might work on Windows. I'll have to try it. Currently in version 1.0, the only thing in the config is the bot token and status stuff as you can see. I'll update these for future releases.\n"
    "# thank you for using the lorem ipsum binary\n\n"

    "main {  # housing of the main config info\n"
    "  version = %s  # do not change this, this is to make sure your config isn't missing stuff the code expects\n\n"

    "  bot_name _;  # input the name for commands and such\n"
    "  BOT_TOKEN _;   # Replace the underscore with your bot token\n"
    "}\n\n"

    "status {\n"
    "  rich_presence _;   # add a rich presence if desired\n"
    "  custom_status _;   # add a custom status if desired\n"
    "}\n\n"
    
    "text_cmds {  # text commands. Like if some says \"UNIX\" it'll print what you wanna say\n"
    "#  # example _;  # replace the example with your desired command\n"
    "#  # !example2 \"runs your example\";\n"
    "#  # example3 \"When someone says the word 'example3', it'll print this\";\n"
    "}\n\n"
    
    "slash_cmds {  # slash commands a part of the app\n"
    "  example {\n"
    "    desc \"example bot command\";  # this is the description for the bot\n"
    "    output \"what the bot will say\";  # the output when the slash command is run\n"
    "  }\n"
    "}\n\n"

    "# more to be added eventually, idk when\n",
    CURRENT_VERSION
  );

  fclose(fp);

  FILE *rm = fopen("README.md", "w");
  if (rm == NULL) 
  {
    printf("ERROR! Failed to create starter guide file. Just read the GitHub docs.\n");
  }
  else
  {
    fprintf(rm,
      "Discord bot starter guide\n\n"
      "Hello! SizableSplash (aka: Trevor) here. Thank you for downloading my Discord bot program. This code is intended to work with the Linux/POSIX operating systems. I designed this code on Debian, no guarentees to work on other distros. It should work, just be aware if it doesn't, you'll know why. I just haven't tested it... yet. Windows compatibility is what I'm more curious about. My bot can do other things, though. It can... ok, that's really about it. I designed this code to work like the JDS for Minecraft. Run it, it generates the config on startup. To create the background process, create the daemon in ```/etc/systemd/system/``` Run ```sudo nano /etc/systemd/system/discord-bot.service``` and paste the following:\n\n"

      "[Unit]\n"
      "Description=C Discord Bot Daemon\n"
      "After=network.target\n\n"

      "[Service]\n"
      "Type=simple\n"
      "User=root\n"
      "WorkingDirectory=/path/to/binary/here/\n"
      "ExecStart=/path/to/binary/here/binary\n"
      "Restart=on-failure\n"
      "RestartSec=10\n\n"

      "[Install]\n"
      "WantedBy=multi-user.target\n"

      "Just replace your actual root path. Do it somewhere like ```/etc/discord-bot/```. Run ```sudo systemctl daemon-reload && sudo systemctl enable discord-bot && sudo systemctl start discord-bot && sudo systemctl status discord-bot``` to start it. Thank you for using my code!\n"
    );
    fclose(rm);
    printf("Created README starter guide!\n");
  }

  printf("\nConfig file sucessfully written! Please input your config needed\n\n");
  return 1;
}


int startup(void)
{
  printf("\nOpening config...\n");

  FILE *cf = fopen("bot_config.conf", "r");
  if (cf == NULL)
  {
    printf("Config file not found! Creating one...\n");
    create_conf();
    return -1;
  }

  bt_conf();

  return 0;
}