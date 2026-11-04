#include "config.h"

char BOT_TOKEN[256];
char rich_prcnse[256];
char status[256];

int main(void)
{
  if (startup() != 0) return -1;

  struct discord *client = discord_init(BOT_TOKEN);
  discord_add_intents(client, DISCORD_GATEWAY_MESSAGE_CONTENT);
  
  discord_set_on_ready(client, &on_ready);
  discord_set_on_cycle(client, &on_cycle);
  discord_set_on_message_create(client, &on_message);
  discord_set_on_interaction_create(client, &on_interaction);
  discord_run(client);
}