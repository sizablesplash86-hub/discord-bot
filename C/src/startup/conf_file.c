#include "../config.h"

typedef struct
{
  char version[20];
  char bot_name[128];
  char bot_token[256];
  char rich_presence[512];
  char status[512];

  char notify[64];
  char usr_id[512];
  char not_chan[512];

} bot_config;

typedef struct
{
  char name[128];
  char desc[512];
  char funct[128];
  char out[1024];
} app_conf;

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
  
  if (c == '{' || c == '}' || c == ';')
  {
    buf[0] = (char)c;
    buf[1] = '\0';
    return 1;
  }

  if (c == '"')
  {
    while ((c = fgetc(fp)) != EOF)
    {
      if (c == '"')
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
  int inside_update_block = 0;

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

    else if (strcmp(token, "update") == 0)
    {
      if (next_token(fp, token, sizeof(token)) && strcmp(token, "{") == 0)
      {
        inside_update_block = 1;
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

    else if (inside_update_block)
    {
      if (strcmp(token, "}") == 0)
      {
        inside_status_block = 0;
        continue; 
      }

      int directive_handled = 1;


      if (strcmp(token, "notify") == 0)
      {
        if (next_token(fp, token, sizeof(token)))
        {
          snprintf(config->notify, sizeof(config->notify), "%s", token);
        }
        else snprintf(error, sizeof(error), "%s", token);
      }

      else if (strcmp(token, "user") == 0)
      {
        if (next_token(fp, token, sizeof(token)))
        {
          snprintf(config->usr_id, sizeof(config->usr_id), "%s", token);
        }
        else snprintf(error, sizeof(error), "%s", token);
      }

      else if (strcmp(token, "channel") == 0)
      {
        if (next_token(fp, token, sizeof(token)))
        {
          snprintf(config->not_chan, sizeof(config->not_chan), "%s", token);
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

    else printf("Testing\n");
  }
  fclose(fp);
  return 0;
}

int app_file(const char *app_path)
{
  app_conf cfg = {0};

  FILE *fp = fopen(app_path, "r");
  if (!fp)
  {
    printf("Failed to open config file: %s\n", BOT_CONF);
    return -1;
  }

  char token[256];
  int inside_app_block = 0;

  while (next_token(fp, token, sizeof(token)))
  {
    if (strcmp(token, "app") == 0)
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
    else if (inside_app_block)
    {
      if (strcmp(token, "}") == 0)
      {
        inside_app_block = 0;
        continue;
      }

      int directive_handled = 1;

      if (strcmp(token, "name") == 0)
      {
        if (next_token(fp, token, sizeof(token)))
        {
          snprintf(cfg.name, sizeof(cfg.name), "%s", token);
        }
      }
      
      else if (strcmp(token, "desc") == 0)
      {
        if (next_token(fp, token, sizeof(token)))
        {
          snprintf(cfg.desc, sizeof(cfg.desc), "%s", token);
        }
      }

      else if (strcmp(token, "funct") == 0)
      {
        if (next_token(fp, token, sizeof(token)))
        {
          snprintf(cfg.funct, sizeof(cfg.funct), "%s", token);
        }
      }

      else if (strcmp(token, "out") == 0)
      {
        if (next_token(fp, token, sizeof(token)))
        {
          snprintf(cfg.out, sizeof(cfg.out), "%s", token);
        }
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
  }

  fclose(fp);

  return 0;
}

int bt_conf(void)
{
  bot_config cfg = {0};

  if (parse_config(&cfg) == 0)
  {
    if (strcmp(cfg.bot_token, "_") == 0 || strcmp(cfg.bot_token, "") == 0)
    {
      printf("\033[31mBOT TOKEN NOT FOUND!!\033[0m Please enter one.\n");
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

  else if (cfgmaj > curmaj || cfgmin > curmin || cfgpat > curpat) printf("\nEither you're me testing,\n"
    "or you changed the version,\n"
    "or someone who compiled the code yourself.\n"
    "Either way, continue.\n\n"
  );

  else if (cfgmaj == curmaj || cfgmin == curmin || cfgpat == curpat) printf("Config version matches code version\n");

  snprintf(bot_name, sizeof(bot_name), "%s", cfg.bot_name);
  snprintf(BOT_TOKEN, sizeof(BOT_TOKEN), "%s", cfg.bot_token);
  snprintf(rich_prcnse, sizeof(rich_prcnse), "%s", cfg.rich_presence);
  snprintf(status, sizeof(status), "%s", cfg.status);

  printf("\nConfig read:\n\n"

    "  Not showing bot token for privacy\n\n"

    "  Bot name = \"%s\"\n"
    "  Rich presence = \"%s\"\n"
    "  Custom status = \"%s\"\n"
    "\n",
    bot_name, rich_prcnse, status
  );
  if (
    strcmp(cfg.notify, "yes") == 0 ||
    strcmp(cfg.notify, "Yes") == 0 ||
    strcmp(cfg.notify, "YES") == 0
  )
  {
    snprintf(ntfy, sizeof(ntfy), "%s", cfg.notify);
    snprintf(usr_id, sizeof(usr_id), "%s", cfg.usr_id);
    snprintf(chnl_id, sizeof(chnl_id), "%s", cfg.not_chan);
  }

  read_apps();

  return 0;
}