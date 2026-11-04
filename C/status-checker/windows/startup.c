#include "config.h"

bot_config startup(void)
{
  bot_config config = {0};

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
      "ALERT_CHANNEL_ID =   # channel to alert member when server goes offline.\n"
      "TEMP_CHANNEL_ID =   # currently unused\n"
      "STATUS_MSG_ID =   # need to figure this out\n"
      "TEMP_MSG_ID =   # same with this\n"
      "SERVER_HOST =   # your server's IP. eg: 192.168.1.50\n"
      "SERVER_PORT = 22  # change if desired\n"
      "SSH_USER = root  # change if desired\n"
      "TEMP_THRESHOLD = 80.0  # change if desired\n"
      "CHECK_INTERVAL_MS = 15000  # change if desired\n"
    );
    fclose(fp);
    printf("\n[*] Config created! Input your needed info and rerun.\n");

    FILE *rm = fopen("starter_guide.md", "w");
    if (rm != NULL)
    {
      fprintf(rm,
        "Discord bot Windows setup guide\n\n"
        "Thank you for using this bot program. Since you are on Windows, you can run this executable (.exe) directly from Command Prompt or PowerShell after filling out 'bot_config.conf'. To run it continuously in the background, consider using Windows Task Scheduler or NSSM (Non-Sucking Service Manager).\n"
      );
      fclose(rm);
    }
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

        if (strcmp(key, "ACTIVE") == 0) {
            strncpy(config.active, value, sizeof(config.active) - 1);
        } else if (strcmp(key, "BOT_TOKEN") == 0) {
            strncpy(config.bot_token, value, sizeof(config.bot_token) - 1);
        } else if (strcmp(key, "BOT_TESTING_CHANNEL") == 0) {
            config.testing_channel = _strtoui64(value, NULL, 10);
        } else if (strcmp(key, "STATUS_CHANNEL_ID") == 0) {
            config.status_channel_id = _strtoui64(value, NULL, 10);
        } else if (strcmp(key, "ADMIN_USER_ID") == 0) {
            config.admin_user_id = _strtoui64(value, NULL, 10);
        } else if (strcmp(key, "ALERT_CHANNEL_ID") == 0) {
            config.alert_channel_id = _strtoui64(value, NULL, 10);
        } else if (strcmp(key, "TEMP_CHANNEL_ID") == 0) {
            config.temp_channel_id = _strtoui64(value, NULL, 10);
        } else if (strcmp(key, "STATUS_MSG_ID") == 0) {
            config.status_msg_id = _strtoui64(value, NULL, 10);
        } else if (strcmp(key, "TEMP_MSG_ID") == 0) {
            config.temp_msg_id = _strtoui64(value, NULL, 10);
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

void save_persisted_msg_id(u64snowflake msg_id)
{
  FILE *f = fopen("status_msg.id", "w");
  if (f)
  {
    fprintf(f, "%llu", (unsigned long long)msg_id);
    fclose(f);
  }
}
