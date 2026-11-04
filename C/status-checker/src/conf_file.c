#include "config.h"

// SpyderFly's site config reader to modify later

typedef struct
{
  char active[8];
  char bot_token[128];
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
  if (c == '{' || c == '}' || c == ';')
  {
    buf[0] = (char)c;
    buf[1] = '\0';
    return 1;
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

int parse_config(const char *fullpath, ServerConfig *config)
{
  FILE *fp = fopen(fullpath, "r");
  if (!fp)
  {
    printf("Failed to open config file: %s\n", fullpath);
    return -1;
  }

  char token[256];
  int inside_server_block = 0;

  while (next_token(fp, token, sizeof(token)))
  {
    if (strcmp(token, "server") == 0)
    {
      if (next_token(fp, token, sizeof(token)) && strcmp(token, "{") == 0)
      {
        inside_server_block = 1;
      }
      else
      {
        printf("Syntax error: expected '{' after server\n");
        fclose(fp);
        return -1;
      }
    } 
    else if (inside_server_block)
    {
      if (strcmp(token, "}") == 0)
      {
        inside_server_block = 0;
        break; 
      }

      int directive_handled = 1;

      if (strcmp(token, "listen") == 0)
      {
        if (next_token(fp, token, sizeof(token)))
        {
          snprintf(config->port, sizeof(config->port), "%s", token);
        }
      } 
      else if (strcmp(token, "root") == 0)
      {
        if (next_token(fp, token, sizeof(token)))
        {
          size_t len = strlen(token);
          if (len > 1 && token[len - 1] == '/') token[len - 1] = '\0';
          snprintf(config->root, sizeof(config->root), "%s", token);
        }
      } 
      else if (strcmp(token, "server_name") == 0) 
      {
        if (next_token(fp, token, sizeof(token)))
        {
          snprintf(config->domain, sizeof(config->domain), "%s", token);
        }
      }
      else if (strcmp(token, "ssl_certificate") == 0)
      {
        if (next_token(fp, token, sizeof(token)))
        {
          snprintf(config->fullchain, sizeof(config->fullchain), "%s", token);
        }
      }
      else if (strcmp(token, "ssl_certificate_key") == 0)
      {
        if (next_token(fp, token, sizeof(token)))
        {
          snprintf(config->privkey, sizeof(config->privkey), "%s", token);
        }
      }
      else if (strcmp(token, "ssl_trusted_certificate") == 0)
      {
        if (next_token(fp, token, sizeof(token)))
        {
          snprintf(config->trusted_cert, sizeof(config->trusted_cert), "%s", token);
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
    else
    {
      printf("Could not find server block\n");
      return -1;
    }
  }
  fclose(fp);
  return 0;
}

int ste_conf(const char *full_path, const char *site_name)
{
  ServerConfig cfg = {0};

  if (parse_config(full_path, &cfg) == 0)
  {
    printf("Successfully Parsed Config:\n"
      "  Port:        %s\n"
      "  Root:        %s\n"
      "  Server Name: %s\n\n"

      "  SSL info:\n"
      "    fullchain:      %s\n"
      "    privkey:        %s\n"
      "    trusted_cert:   %s\n",
      cfg.port, cfg.root, cfg.domain,

      cfg.fullchain, cfg.privkey, cfg.trusted_cert
    );
  }

  else
  {
    printf("Error in site config\n");
    return -1;
  }

  return 0;
}