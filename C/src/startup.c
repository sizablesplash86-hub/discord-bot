#include "config.h"

int create_conf(void)
{
  FILE *fp = fopen("bot_config.conf", "w");
  if (fp == NULL)
  {
    printf("Failed to open config file\n");
    return -1;
  }

  fprintf(fp, 
    "version = 1.0.0\n"
    "# Hello! Welcome to my discord bot commands. This config will likely be changed and updated in future releases. Make sure to delete this file each time\n"
    "# This config file allows you the downloader to use my code without me exposing my token or you compiling it yourself.\n"
    "# This program is exclusive to the POSIX operating systems thanks to the header sys/stat.h so this will not work on Windows. Actually, I just looked it up, this code might work on Windows. I'll have to try it. Currently in version 1.0, the only thing in the config is the bot token and status stuff as you can see. I'll update these for future releases.\n"
    "# thank you for using the lorem ipsum binary\n\n"
    "BOT_TOKEN =   # input your bot token here\n"
    "rich_presence =   # add a rich presence if desired\n"
    "custom_status =   # add a custom status if desired\n"
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
      "Hello! SizableSplash (aka: Trevor) here. Thank you for downloading my Discord bot program. This code is intended to work with the Linux/POSIX operating systems. I designed this code on Debian, no guarentees to work on other distros. It might work, I just haven't tested it. My bot can do other things, though. It can... ok, that's really about it. I designed this code to work like the JDS for Minecraft. Run it, it generates the config on startup. To create the background process, create the daemon in ```/etc/systemd/system/``` Run ```sudo nano /etc/systemd/system/discord-bot.service``` and paste the following:\n\n"

      "[Unit]\n"
      "Description=Discord bot Lorem Ipsum code\n"
      "After=network.target\n\n"

      "[Service]\n"
      "Type=simple\n"
      "User=root\n"
      "ExecStart=/path/to/binary/here/lorem-ipsum\n"
      "Restart=always\n"
      "RestartSec=10\n\n"

      "[Install]\n"
      "WantedBy=multi-user.target\n\n"

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
  }

  char line2[512];
  int found2 = 0;
  while (fgets(line2, sizeof(line2), cf) != NULL)
  {
    if (strncmp(line2, "version = ", 10) == 0)
    {
      found2 = 1;
      break;
    }
  }
  char *version = line2 + 10;
  if (found2)
  {
    line2[strcspn(line2, "\r\n #")] =0;
    version[strcspn(version, "\r\n #")] = 0;

    printf("Config version is %s\n", version);
  }

  if (strcmp(version, CURRENT_VERSION) < 0)
  {
    printf("Config file is oudated! Please delete and rerun\n");
    return -1;
  }

  
  char line[512];
  int found = 0;
  while (fgets(line, sizeof(line), cf) != NULL)
  {
    if (strncmp(line, "BOT_TOKEN = ", 12) == 0)
    {
      found = 1;
      break;
    }
  }
  fclose(cf);

  char *token = line + 12;
  if (found)
  {
    line[strcspn(line, "\r\n #")] =0;
    token[strcspn(token, "\r\n #")] = 0;
//    printf("Bot token is %s\n", token);

    snprintf(BOT_TOKEN, sizeof(BOT_TOKEN), "%s", token);
  }

  return 0;
}