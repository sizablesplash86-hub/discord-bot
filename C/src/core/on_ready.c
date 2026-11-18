#include "../config.h"

void on_ready(struct discord *client, const struct discord_ready *event)
{
  log_info("Succesfully connected to Discord as %s#%s!", event->user->username, event->user->discriminator);

  check_sdb(client);

  arch_struct(client, event);
  fake_ban_struct(client, event);

  // some issues with these two
  rich_presence(client, event);
  set_custom_status(client, event);
}