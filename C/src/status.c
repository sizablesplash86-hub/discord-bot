/*   This is for the status. It doesn't work right now.

#include "config.h"




//  Reads "<key> = value" from rich_presence.conf into out. Returns 1 on success.
static int read_conf(const char *key, char *out, size_t out_size)
{
  FILE *f = fopen("rich_presence.conf", "r");
  if (f == NULL) return 0;

  char line[1024];
  size_t klen = strlen(key);
  int found = 0;
  while (fgets(line, sizeof(line), f) != NULL)
  {
    if (strncmp(line, key, klen) == 0 && strncmp(line + klen, " = ", 3) == 0)
    {
      found = 1;
      break;
    }
  }
  fclose(f);
  if (!found) return 0;

  line[strcspn(line, "\r\n;")] = 0;
  char *val = line + klen + 3;
  while (*val == ' ' || *val == '\t') val++;

  snprintf(out, out_size, "%s", val);
  return out[0] != '\0';
}

static void apply_presence(struct discord *client)
{
  char game[256]   = "";
  char custom[256] = "";
  int has_game   = read_conf("rich_presence", game,   sizeof(game));
  int has_custom = read_conf("custom_status", custom, sizeof(custom));

  struct discord_activity activities[2];
  memset(activities, 0, sizeof(activities));
  int n = 0;

  if (has_game)
  {
    activities[n].name = game;
    activities[n].type = DISCORD_ACTIVITY_GAME;
    n++;
  }
  if (has_custom)
  {
    activities[n].name  = "Custom Status";   //  placeholder, not displayed
    activities[n].type  = DISCORD_ACTIVITY_CUSTOM;
    activities[n].state = custom;            //  the visible text
    n++;
  }

  printf("Presence: game='%s' custom='%s'\n", game, custom);

  struct discord_presence_update presence =
  {
    .activities = &(struct discord_activities){ .size = n, .array = activities },
    .status = "online",
    .afk    = false,
    .since  = discord_timestamp(client),
  };
  discord_set_presence(client, &presence);
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
  apply_presence(client);
}

void on_ready(struct discord *client, const struct discord_ready *event)
{
  struct stat st;
  if (stat("rich_presence.conf", &st) == 0) last_mtime = st.st_mtime;
  apply_presence(client);
}


*/