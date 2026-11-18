#include "../config.h"

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
  set_status(client, "online", DISCORD_ACTIVITY_GAME, rich_prcnse);  // Parses status to it
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
  if (stat(BOT_CONF, &st) != 0) return;
  if (st.st_mtime == last_mtime) return;

  last_mtime = st.st_mtime;
  load_presence(client);
}

void rich_presence(struct discord *client, const struct discord_ready *event)
{
  if (strcmp(rich_prcnse, "_") == 0 || strcmp(rich_prcnse, "") == 0)
  {
    printf("No rich presence stated. Exiting...\n");
    return;
  }
/*
  struct stat st;
  if (stat(BOT_CONF, &st) == 0) last_mtime = st.st_mtime;
*/
  load_presence(client);
}