#include "config.h"

void on_interaction(struct discord *client, const struct discord_interaction *event)
{
  arch_interaction(client, event);
}