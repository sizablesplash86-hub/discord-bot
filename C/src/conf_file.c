#include "config.h"

typedef struct
{
  char version[20];
  char bot_name[128];
  char bot_token[256];
  char rich_presence[512];
  char status[512];
  char cmd[128];

  char app[128];  // add something for this

/*  For status checker but too lazy to remove
  u64snowflake testing_channel;
  u64snowflake status_channel_id;
  u64snowflake admin_user_id;
  u64snowflake alert_channel_id;
  u64snowflake temp_channel_id;
  u64snowflake status_msg_id;
  u64snowflake temp_msg_id;
  char server_host[256];
  int server_port;
  char ssh_user[64];
  float temp_threshold;
  long check_interval_ms;
*/
} bot_config;

int next_token(FILE *fp, char *buf, size_t max_len)
{
  int c;
  size_t len = 0;

  while ((c = fgetc(fp)) != EOF)
  {
    if (c == '#')
    {
      while ((c = fgetc(fp)) != EOF && c != '\n');
      continue;
    }
    if (!isspace(c))
    {
      ungetc(c, fp);
      break;
    }
  }

  if (c == EOF) return 0;

  c = fgetc(fp);
  
  // Handle structural single-character tokens
  if (c == '{' || c == '}' || c == ';')
  {
    buf[0] = (char)c;
    buf[1] = '\0';
    return 1;
  }

  // Handle quoted strings (e.g. "Lorem Ipsum")
  if (c == '"')
  {
    while ((c = fgetc(fp)) != EOF)
    {
      if (c == '"') // Closing quote found
      {
        break;
      }
      if (len < max_len - 1)
      {
        buf[len++] = (char)c;
      }
    }
    buf[len] = '\0';
    return len > 0 ? 1 : 0;
  }

  ungetc(c, fp);
  while ((c = fgetc(fp)) != EOF)
  { 
    if (isspace(c) || c == '{' || c == '}' || c == ';')
    {
      ungetc(c, fp);
      break;
    }
    if (len < max_len - 1)
    {
      buf[len++] = (char)c;
    }
  }
  buf[len] = '\0';
  return len > 0 ? 1 : 0;
}

int parse_config(bot_config *config)
{
  FILE *fp = fopen(BOT_CONF, "r");
  if (!fp)
  {
    printf("Failed to open config file: %s\n", BOT_CONF);
    return -1;
  }

  char token[256];

  int inside_server_block = 0;
  int inside_main_block = 0;
  int inside_status_block = 0;
  int inside_text_block = 0;
  int inside_app_block = 0;

  char error[512];

  while (next_token(fp, token, sizeof(token)))
  {
    if (strcmp(token, "main") == 0)
    {
      if (next_token(fp, token, sizeof(token)) && strcmp(token, "{") == 0)
      {
        inside_main_block = 1;
        continue;
      }
      else
      {
        printf("Syntax error: expected '{' after %s\n", token);
        fclose(fp);
        return -1;
      }
    }

    else if (strcmp(token, "status") == 0)
    {
      if (next_token(fp, token, sizeof(token)) && strcmp(token, "{") == 0)
      {
        inside_status_block = 1;
        continue;
      }
      else
      {
        printf("Syntax error: expected '{' after %s\n", token);
        fclose(fp);
        return -1;
      }
    }

    else if (strcmp(token, "text_cmds") == 0)
    {
      if (next_token(fp, token, sizeof(token)) && strcmp(token, "{") == 0)
      {
        inside_text_block = 1;
        continue;
      }
      else
      {
        printf("Syntax error: expected '{' after %s\n", token);
        fclose(fp);
        return -1;
      }
    }

    else if (strcmp(token, "slash_cmds") == 0)
    {
      if (next_token(fp, token, sizeof(token)) && strcmp(token, "{") == 0)
      {
        inside_app_block = 1;
        continue;
      }
      else
      {
        printf("Syntax error: expected '{' after %s\n", token);
        fclose(fp);
        return -1;
      }
    }



    else if (inside_main_block)
    {
      if (strcmp(token, "}") == 0)
      {
        inside_main_block = 0;
        continue; 
      }

      int directive_handled = 1;

      if (strcmp(token, "version") == 0)
      {
        if (next_token(fp, token, sizeof(token)))
        {
          snprintf(config->version, sizeof(config->version), "%s", token);
        }
        else snprintf(error, sizeof(error), "%s", token);
      }

      else if (strcmp(token, "bot_name") == 0)
      {
        if (next_token(fp, token, sizeof(token)))
        {
          snprintf(config->bot_name, sizeof(config->bot_name), "%s", token);
        }
        else snprintf(error, sizeof(error), "%s", token);
      }

      else if (strcmp(token, "BOT_TOKEN") == 0)
      {
        if (next_token(fp, token, sizeof(token)))
        {
          snprintf(config->bot_token, sizeof(config->bot_token), "%s", token);
        }
        else snprintf(error, sizeof(error), "%s", token);
      }

      else
      {
        directive_handled = 0;
        char skip_token[256];
        while (next_token(fp, skip_token, sizeof(skip_token)))
        {
          if (strcmp(skip_token, ";") == 0) break;
        }
      }

      if (directive_handled)
      {
        if (next_token(fp, token, sizeof(token)) && strcmp(token, ";") != 0)
        {
          printf("Syntax error: missing ';' after directive\n");
          fclose(fp);
          return -1;
        }
      }
    }

    else if (inside_status_block)
    {
      if (strcmp(token, "}") == 0)
      {
        inside_status_block = 0;
        continue; 
      }

      int directive_handled = 1;


      if (strcmp(token, "rich_presence") == 0)
      {
        if (next_token(fp, token, sizeof(token)))
        {
          snprintf(config->rich_presence, sizeof(config->rich_presence), "%s", token);
        }
        else snprintf(error, sizeof(error), "%s", token);
      }

      else if (strcmp(token, "custom_status") == 0)
      {
        if (next_token(fp, token, sizeof(token)))
        {
          snprintf(config->status, sizeof(config->status), "%s", token);
        }
        else snprintf(error, sizeof(error), "%s", token);
      }

      else
      {
        directive_handled = 0;
        char skip_token[256];
        while (next_token(fp, skip_token, sizeof(skip_token)))
        {
          if (strcmp(skip_token, ";") == 0) break;
        }
      }

      if (directive_handled)
      {
        if (next_token(fp, token, sizeof(token)) && strcmp(token, ";") != 0)
        {
          printf("Syntax error: missing ';' after directive\n");
          fclose(fp);
          return -1;
        }
      }
    }

    else if (inside_text_block)
    {
      TODO: // add this later
    }

    else if (inside_app_block)
    {
      TODO2: // add this later
    }

    else printf("Testing\n");
  }
  fclose(fp);
  return 0;
}

int bt_conf(void)
{
  bot_config cfg = {0};

  if (parse_config(&cfg) == 0)
  {
    if (strcmp(cfg.bot_token, "_") == 0)
    {
      printf("BOT TOKEN NOT FOUND!! Please enter one.\n");
      return -1;
    }
    else printf("Sucessfully read bot config\n");
  }

  else
  {
    printf("Error in bot config\n");
    return -1;
  }

  int cfgmaj, cfgmin, cfgpat;
  int curmaj, curmin, curpat;

  sscanf(cfg.version, "%d.%d.%d", &cfgmaj, &cfgmin, &cfgpat);
  sscanf(CURRENT_VERSION, "%d.%d.%d", &curmaj, &curmin, &curpat);

  if (cfgmaj < curmaj || cfgmin < curmin || cfgpat < curpat)
  {
    printf(
      "\nERROR! Config version is less than code version\n"
      "  Config version = %d.%d.%d\n"
      "  Code version = %d.%d.%d\n"
      "Please delete config and rerun\n\n",
      cfgmaj, cfgmin, cfgpat, curmaj, curmin, curpat
    );
    return -1;
  }

  snprintf(BOT_TOKEN, sizeof(BOT_TOKEN), "%s", cfg.bot_token);
  snprintf(rich_prcnse, sizeof(rich_prcnse), "%s", cfg.rich_presence);

  TODO3: // add some way to continue from here

  return 0;
}
