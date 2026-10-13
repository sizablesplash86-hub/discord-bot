#include "config.h"

void on_message(struct discord *client, const struct discord_message *event)
{
  hist(client, event);
  bt_inf(client, event);
}