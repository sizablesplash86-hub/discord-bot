#include "../config.h"

char BOT_CONF[512];
char apps_dir[512];
char cmds_dir[512];
char BOT_TOKEN[256];
char bot_name[128];
char rich_prcnse[256];
char status[256];

/* for updating */
char ntfy[64];
char usr_id[512];
char chnl_id[512];

int main(int arc, char *argv[])
{
  if (arc < 2)
  {
    printf("\nWelcome to the Lorem Ipusm binary! Thank you for using this.\n\n");

    snprintf(BOT_CONF, sizeof(BOT_CONF), "bot_config.conf");
    snprintf(apps_dir, sizeof(apps_dir), "apps");
    snprintf(cmds_dir, sizeof(cmds_dir), "commands");
  }

  // might do a deb package with this
  else if (strcmp(argv[1], "-s") == 0)
  {
    if (getegid() != 0)
    {
      printf("Root/sudo is needed\n");
      return -1;
    }

    snprintf(BOT_CONF, sizeof(BOT_CONF), "%s/bot_config.conf", BOT_PATH);
    snprintf(apps_dir, sizeof(apps_dir), "%s/apps", BOT_PATH);
    snprintf(cmds_dir, sizeof(cmds_dir), "%s/commands", BOT_PATH);
  }

  else if (strcmp(argv[1], "-ls") == 0)
  {
    printf("\nCurrent bot commands:\n"
      "  /arch       makes bot say \"I use Arch btw\"\n"
      "  unix        makes the bot print a history on UNIX\n"
      "  /ban        fake ban command\n"
      "  !bot_info   lists info on the bot\n"
      "  !website    brings up sizablesplash.com\n"
      "\n"
    );
  }
  
  else if (strcmp(argv[1], "-h") == 0)
  {
    printf("Code commands:\n"
      "  -s     intended for a hard-coded path for a deb package. Expects config to be in %s\n"
      "  -h     lists this\n"
      "  -ls    lists hard-coded bot features\n"

      "\nIf run with no sub-command, bot will start as normal expecting config in the same directory\n\n",
      BOT_PATH
    );
  }

  else
  {
    printf("Something happened, idk what\n");
    return 0;
  }

  if (startup() != 0) return -1;

  struct discord *client = discord_init(BOT_TOKEN);
  discord_add_intents(client, DISCORD_GATEWAY_MESSAGE_CONTENT);
  
  discord_set_on_ready(client, &on_ready);
  discord_set_on_cycle(client, &on_cycle);
  discord_set_on_message_create(client, &on_message);
  discord_set_on_interaction_create(client, &on_interaction);
  discord_run(client);
}