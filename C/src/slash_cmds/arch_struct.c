#include "../config.h"

void arch_struct(struct discord *client, const struct discord_ready *event)
{
  struct discord_create_guild_application_command params =
  {
    .name = "Arch",
    .description = "Announce your superiority"
  };
  discord_create_guild_application_command(client, event->application->id, GUILD_ID, &params, NULL);
}