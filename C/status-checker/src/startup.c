#include "config.h"

bot_config startup(void)
{
  bot_config config = {0}; // Initialize struct to zero/empty values

  FILE *tbd = fopen("bot_config.conf", "r");
  if (tbd == NULL)
  {
    printf("\n[*] Config file not found! Creating one...\n");
    FILE *fp = fopen("bot_config.conf", "w");
    if (fp == NULL)
    {
      printf("\n[*] ERROR. Failed to create config file.\n");
      return config;
    }
    fprintf(fp,
      "ACTIVE = no  # change to yes after you edit the config\n"
      "BOT_TOKEN =   # your bot token obviously goes here\n"
      "BOT_TESTING_CHANNEL =   # this is currently unused in the code\n"
      "STATUS_CHANNEL_ID =   # where it alerts your server status\n"
      "ADMIN_USER_ID =   # ID to alert member when server goes offline\n"
      "ALERT_CHANNEL_ID =   # channel to alert member when server goes offline. put the same as the status channel if you desire\n"
      "TEMP_CHANNEL_ID =   # currently unused\n"
      "STATUS_MSG_ID =   # need to figure this out\n"
      "TEMP_MSG_ID =   # same with this\n"
      "SERVER_HOST =   # your server's IP. eg (fake tailscale IP): 100.465.123.345\n"
      "SERVER_PORT = 22  # change if desired\n"
      "SSH_USER = root  # this assumes you permit root login change if desired\n"
      "TEMP_THRESHOLD = 80.0  # change if desired\n"
      "CHECK_INTERVAL_MS = 15000  # change if desired\n"
    );
    fclose(fp);
    printf("\n[*] Config created! Input your needed info and rerun.\n");

    FILE *rm = fopen("README.md", "w");
    if (rm == NULL) 
    {
      printf("ERROR! Failed to create starter guide file. Just read the GitHub docs.\n");
    }
    else
    {
      fprintf(rm,
        "Discord bot starter guide\n\n"
        "Hello! SizableSplash (aka: Trevor) here. Thank you for downloading my Discord bot program. This code is primarily intended to work as a status checker for a home server. This code is intended to work with the Linux/POSIX operating systems. I designed this code on Debian, no guarentees to work on other distros. It might work, I just haven't tested it. My bot can do other things, though. It can... ok, that's really about it. I designed this code to work like the JDS for Minecraft. Run it, it generates the config on startup. To create the background process, create the daemon in ```/etc/systemd/system/``` Run ```sudo nano /etc/systemd/system/discord-bot.service``` and paste the following:\n\n"

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
    }
    if (system("ls README.md > /dev/null") == 0) printf("\n[*] README sucessfully created\n");
    return config;
  }

  printf("\n[*] Scanning config...\n");

  fseek(tbd, 0, SEEK_END);
  long length = ftell(tbd);
  fseek(tbd, 0, SEEK_SET);

  char *buffer = malloc(length + 1);
  if (buffer == NULL)
  {
    printf("ERROR. Memory allocation failed.\n");
    fclose(tbd);
    return config;
  }

  fread(buffer, 1, length, tbd);
  buffer[length] = '\0';
  fclose(tbd);

  char *line = strtok(buffer, "\n");
  while (line != NULL)
  {
    if (line[0] != '\0' && line[0] != '#')
    {
      char key[64], value[256];
      if (sscanf(line, "%63[^=] = %255[^\n]", key, value) == 2)
      {
        char *end = key + strlen(key) - 1;
        while (end > key && (*end == ' ' || *end == '\t')) *end-- = '\0';

        char *comment = strchr(value, '#');
        if (comment != NULL) *comment = '\0';
        
        end = value + strlen(value) - 1;
        while (end >= value && (*end == ' ' || *end == '\t' || *end == '\r')) *end-- = '\0';

        // Map keys directly into the bot_config struct
        if (strcmp(key, "ACTIVE") == 0) {
            strncpy(config.active, value, sizeof(config.active) - 1);
        } else if (strcmp(key, "BOT_TOKEN") == 0) {
            strncpy(config.bot_token, value, sizeof(config.bot_token) - 1);
        } else if (strcmp(key, "BOT_TESTING_CHANNEL") == 0) {
            config.testing_channel = strtoull(value, NULL, 10);
        } else if (strcmp(key, "STATUS_CHANNEL_ID") == 0) {
            config.status_channel_id = strtoull(value, NULL, 10);
        } else if (strcmp(key, "ADMIN_USER_ID") == 0) {
            config.admin_user_id = strtoull(value, NULL, 10);
        } else if (strcmp(key, "ALERT_CHANNEL_ID") == 0) {
            config.alert_channel_id = strtoull(value, NULL, 10);
        } else if (strcmp(key, "TEMP_CHANNEL_ID") == 0) {
            config.temp_channel_id = strtoull(value, NULL, 10);
        } else if (strcmp(key, "STATUS_MSG_ID") == 0) {
            config.status_msg_id = strtoull(value, NULL, 10);
        } else if (strcmp(key, "TEMP_MSG_ID") == 0) {
            config.temp_msg_id = strtoull(value, NULL, 10);
        } else if (strcmp(key, "SERVER_HOST") == 0) {
            strncpy(config.server_host, value, sizeof(config.server_host) - 1);
        } else if (strcmp(key, "SERVER_PORT") == 0) {
            config.server_port = atoi(value);
        } else if (strcmp(key, "SSH_USER") == 0) {
            strncpy(config.ssh_user, value, sizeof(config.ssh_user) - 1);
        } else if (strcmp(key, "TEMP_THRESHOLD") == 0) {
            config.temp_threshold = (float)atof(value);
        } else if (strcmp(key, "CHECK_INTERVAL_MS") == 0) {
            config.check_interval_ms = atol(value);
        }
      }
    }
    line = strtok(NULL, "\n");
  }

  free(buffer);
  return config;
}

void log_open(void)
{
  FILE *log = fopen("bot.log", "a");
  if (log == NULL)
  {
    printf("\n[*] ERROR. Failed to open file\n");
    return;
  }

  time_t now = time(NULL);
  char *timestamp = ctime(&now);
  timestamp[strcspn(timestamp, "\n")] = 0;

  fprintf(log,"[%s] Service started\n", timestamp);

  fclose(log);
}

void conf_file(void)
{
  bot_config cfg = startup();
  printf("[*] Loaded Server Host: %s\n", cfg.server_host);
  printf("[*] Loaded Server Port: %d\n", cfg.server_port);
}