#include "../config.h"

void website(struct discord *client, const struct discord_message *event)
{
  if (!event || !event->author) return;
  if (event->author->bot) return;

  if (event->content)
  { 
    if (strstr(event->content, "!website") != NULL)
    {
      struct discord_create_message params = { .content = "https://www.sizablesplash.com" };

      if (discord_create_message(client, event->channel_id, &params, NULL) != 0) printf("ERROR. Failed to send message.\n");
    }
  }
}