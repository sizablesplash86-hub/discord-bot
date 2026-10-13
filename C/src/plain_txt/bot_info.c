#include "../config.h"

void bt_inf(struct discord *client, const struct discord_message *event)
{
  if (!event || !event->author) return;
  if (event->author->bot) return;

  if (event->content)
  { 
    if (strstr(event->content, "!bot_info") != NULL)
    {
      struct discord_create_message params = { .content = "This discord bot is programmed and maintained by SizableSplash. An aspiring programmer from Minnesota. He has a specific interest for the C programming language and uses it along with Concord by Cogmasters to make this discord bot. SizableSplash also created the web server [SpyderFly](<https://spyderfly.sizablesplash.com>) and the Minecraft server manager [DaemonCraft](<https://daemoncraft.sizablesplash.com>). You can visit his website at [sizablesplash.com](<https://www.sizablesplash.com>) for more info or visit the [GitHub](<https://github.com/sizablesplash86-hub/discord-bot>) to see the code this is on." };

      if (discord_create_message(client, event->channel_id, &params, NULL) != 0) printf("ERROR. Failed to send message.\n");
    }
  }
}