#include "../config.h"

void fake_ban_struct(struct discord *client, const struct discord_ready *event)
{
  struct discord_create_global_application_command params =
  {
    .name = "ban",
    .description = "fake ban someone",
    .options = &(struct discord_application_command_options)
    {
      .size = 1,
      .array = (struct discord_application_command_option[])
      {
        {
          .type = DISCORD_APPLICATION_OPTION_USER,
          .name = "user",
          .description = "The user to fake ban",
          .required = true
        }
      }
    }
  };

  discord_create_global_application_command(client, event->application->id, &params, NULL);
}