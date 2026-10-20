#include "config.h"

static void set_status(struct discord *client, const char *status, enum discord_activity_types type, const char *text)
{
  struct discord_activity activities[] =
  {
    {
      .name = (char *)text,
      .type = type,
    },
  };

  struct discord_presence_update presence =
  {
    .activities =
      &(struct discord_activities)
      {
        .size = sizeof(activities) / sizeof *activities,
        .array = activities,
      },
      .status = (char *)status,
      .afk = false,
      .since = discord_timestamp(client),
  };

  discord_set_presence(client, &presence);
}

static int load_presence(struct discord *client)
{
  FILE *ts = fopen("bot_config.conf", "r");  // Opens config file
  if (ts == NULL)
  {
    printf("Failed to open config file.\n");
    return 0;
  }

  char line[1024];
  int found = 0;
  while (fgets(line, sizeof(line), ts) != NULL)
  {
    if (strncmp(line, "rich_presence = ", 16) == 0)
    {
      found = 1;
      break;
    }
  }
  fclose(ts);

  if (!found)
  {
    printf("Rich presence not found in config.\n");
    return 0;
  }

  char *rich_prs = line + 16;
  line[strcspn(line, "\r\n#")] = 0;
  rich_prs[strcspn(rich_prs, "\r\n#")] = 0;

  printf("Rich presence = %s\n", rich_prs);
  set_status(client, "online", DISCORD_ACTIVITY_GAME, rich_prs);  // Parses status to it
  return 1;
}

static time_t last_mtime = 0;

void on_cycle(struct discord *client)
{
  static time_t last_check = 0;
  time_t now = time(NULL);
  if (now - last_check < 5) return;
  last_check = now;

  struct stat st;
  if (stat("rich_presence.conf", &st) != 0) return;
  if (st.st_mtime == last_mtime) return;

  last_mtime = st.st_mtime;
  load_presence(client);
}

void rich_presence(struct discord *client, const struct discord_ready *event)
{
  struct stat st;
  if (stat("rich_presence.conf", &st) == 0) last_mtime = st.st_mtime;
  load_presence(client);
}