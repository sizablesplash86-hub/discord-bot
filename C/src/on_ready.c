#include "config.h"

void on_ready(struct discord *client, const struct discord_ready *event)
{
  log_info("Succesfully connected to Discord as %s#%s!", event->user->username, event->user->discriminator);
  arch_struct(client, event);
}
