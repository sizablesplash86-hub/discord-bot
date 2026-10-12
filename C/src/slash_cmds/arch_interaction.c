#include "../config.h"

void arch_interaction(struct discord *client, const struct discord_interaction *event)
{
  if (event->type != DISCORD_INTERACTION_APPLICATION_COMMAND)
    return; /* return if interaction isn't a slash command */

  if (strcmp(event->data->name, "arch") == 0)
  {
    struct discord_interaction_response params =
    {
      .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
      .data = &(struct discord_interaction_callback_data)
      {
        .content = "I use Arch btw"
      }
    };
    discord_create_interaction_response(client, event->id, event->token, &params, NULL);
  }
}