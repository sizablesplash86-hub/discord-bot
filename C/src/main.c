#include "config.h"
#include <hide_from_github/bot.h>

int main()
{
  struct discord *client = discord_init(BOT_TOKEN);  // hidden. if you desire to reuse this code, add at the top or in the config file a line saying #define BOT_TOKEN "the_bot_token_here"
  discord_add_intents(client, DISCORD_GATEWAY_MESSAGE_CONTENT);
  discord_set_on_ready(client, &on_ready);
  discord_set_on_message_create(client, &on_message);
  discord_set_on_interaction_create(client, &on_interaction);
  discord_run(client);
}