#include "../config.h"

int create_conf(void)
{
  FILE *fp = fopen(BOT_CONF, "w");
  if (fp == NULL)
  {
    printf("Failed to create config file\n");
    return -1;
  }

  fprintf(fp,
    "# Welcome to the bot config! Thank you for using the Lorem Ipsum binary.\n"
    "# In here, you can edit the config and change the info needed. Some parts of the bot is hard coded still.\n"

    "main {  # housing of the main config info\n"
    "  version %s;  # Do not change this. This allows the config to check if it's needing something the config is missing. Delete and re-run to generate new config after version change.\n\n"

    "  bot_name \"_\";  # input the name for commands and such\n"
    "  BOT_TOKEN _;   # Replace the underscore with your bot token\n"
    "}\n\n"

    "status {  # for rich presence and the main status I forget the name of\n"
    "  rich_presence \"_\";   # add a rich presence if desired\n"
    "  custom_status \"_\";   # add a custom status if desired\n"
    "}\n\n"
    
    "update {  # Be notified if config version is out of date. This works by notifying the user (in this case you) with a ping in a channel if the config is out of date.\n"
    "  notify no;  # change to yes if you desire to\n"
    "  user _;  # user ID of who to ping\n"
    "  channel _;  # channel where to notify\n"
    "}\n",
    CURRENT_VERSION
  );

  fclose(fp);

  // creating folders for 
  if (mkdir("apps", 0775) != 0) printf("Failed to create folder for apps\n");
  if (mkdir("commands", 0775) != 0) printf("Failed to create commands folder\n");

  FILE *def = fopen("apps/default.app", "w");
  if (def == NULL) printf("Failed to create default app config\n");

  fprintf(def,
    "# Welcome to the apps config! This is a basic app to show.\n\n"

    "app {\n"
    "  name \"ping\";  # put your app name here\n"
    "  desc \"prints pong\";  # put your app descripion here\n"
    "  funct \"ping\";  # how your app works. Example: '/ping'\n"
    "  out \"pong\";  # bot output when app is run\n"
    "}\n\n\n"


    "# here's a clean template to use, just remove the pound signs\n\n"
    
    "#app {\n"
    "#  name \"_\";\n"
    "#  desc \"_\";\n"
    "#  funct \"_\";\n"
    "#  out \"_\";\n"
    "#}\n"
  );

  fclose(def);

  FILE *de = fopen("commands/default.cmd", "w");
  if (de == NULL) printf("Failed to create default cmd config\n");

  fprintf(de,
    "# Welcome to the commands config! This one is a lot simpler than the apps.\n\n"

    "cmd {\n"
    "  funct \"ping\";  # the main function that makes the bot go. Just be aware, this will also do it if the word is in a message instead of the standalone word.\n"
    "  out \"pong\";  # what the bot will print out when the command is ran\n"
    "}\n\n\n"


    "# here's a clean template you can use. Just make sure to remove the pound signs\n\n"

    "#cmd {\n"
    "#  funct \"_\";\n"
    "#  out \"_\";\n"
    "#}\n"
  );

  fclose(de);



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
  printf("\nOpening config...\n"

    "\nBot paths:\n"
    "config: '%s'\n"
    "apps: '%s'\n"
    "cmds: '%s'\n",
    BOT_CONF, apps_dir, cmds_dir
  );

  FILE *cf = fopen(BOT_CONF, "r");
  if (cf == NULL)
  {
    printf("Config file not found! Creating one...\n");
    create_conf();
    return -1;
  }

  else
  {
    fclose(cf);
    printf("Config file exists! Reading now...\n");
  }

  if (bt_conf() != 0) return -1;

  return 0;
}