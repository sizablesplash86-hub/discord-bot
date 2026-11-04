#include "config.h"

// Leaving this here, this is the function for DaemonCraft's config file that'll help

typedef struct {
    char edition[64];
    char version[512];
    char mod[128];
    char min[20];
    char max[20];
} ServerConfig;

int next_token(FILE *fp, char *buf, size_t max_len)
{
  int c;
  size_t len = 0;

  while ((c = fgetc(fp)) != EOF)
  {
    if (c == '#')
    {
      // Skip until end of line
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

int parse_config(const char *inst_conf, ServerConfig *config)
{
  FILE *fp = fopen(inst_conf, "r");
  if (!fp)
  {
    printf("Failed to open config file: %s\n", inst_conf);
    return -1;
  }

  char token[256];

  while (next_token(fp, token, sizeof(token)))
  {
    if (strcmp(token, "edition") == 0)
    {
      if (next_token(fp, token, sizeof(token)))
      {
        snprintf(config->edition, sizeof(config->edition), "%s", token);
      }
    }
    else if (strcmp(token, "version") == 0)
    {
      if (next_token(fp, token, sizeof(token)))
      {
        snprintf(config->version, sizeof(config->version), "%s", token);
      }
    }
    else if (strcmp(token, "mod") == 0)
    {
      if (next_token(fp, token, sizeof(token)))
      {
        snprintf(config->mod, sizeof(config->mod), "%s", token);
      }
    }
    else if (strcmp(token, "min") == 0)
    {
      if (next_token(fp, token, sizeof(token)))
      {
        snprintf(config->min, sizeof(config->min), "%s", token);
      }
    }
    else if (strcmp(token, "max") == 0)
    {
      if (next_token(fp, token, sizeof(token)))
      {
        snprintf(config->max, sizeof(config->max), "%s", token);
      }
    }

    else
    {
      printf("Sum kinda error occured idfk\n");
      return -1;
    }
  }
  fclose(fp);
  return 0;
}

int inst_conf_file(const char *inst_name, const char *inst_path)
{
  printf("Opening config...\n");

  char inst_conf[256];
  snprintf(inst_conf, sizeof(inst_conf), "%s/%s.conf", inst_path, inst_name);

  ServerConfig cfg = {0};

  if (parse_config(inst_conf, &cfg) == 0)
  {
    printf("Successfully Parsed Config:\n"
      "  Edition:   %s\n"
      "  Version:   %s\n"
      "  Mod:       %s\n"
      "  Min:       %s\n"
      "  Max:       %s\n",
      cfg.edition, cfg.version, cfg.mod, cfg.min, cfg.max
    );
  }

  else
  {
    printf("Sum error occured, what do you want?\n");
    return -1;
  }

  char inst_edition[64], inst_version[512], inst_mod[128], inst_min[20], inst_max[20];
  snprintf(inst_edition, sizeof(inst_edition), "%s", cfg.edition);
  snprintf(inst_version, sizeof(inst_version), "%s", cfg.version);
  snprintf(inst_mod, sizeof(inst_mod), "%s", cfg.mod);
  snprintf(inst_min, sizeof(inst_min), "%s", cfg.min);
  snprintf(inst_max, sizeof(inst_max), "%s", cfg.max);

  return 0;
}