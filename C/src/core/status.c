#include "../config.h"

void set_custom_status(struct discord *client, const struct discord_ready *event)
{
  if (strlen(status) == 0 || strcmp(status, "_") == 0)
  {
    printf("Status not specified. Skipping...\n");
    return;
  }

  printf("Setting status to: %s\n", status);

  struct discord_activity activities[] =
  {
  {
      .name = status,
      .type = DISCORD_ACTIVITY_GAME,
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
    .status = "online",
    .afk = false,
    .since = discord_timestamp(client),
  };

  discord_set_presence(client, &presence);
}