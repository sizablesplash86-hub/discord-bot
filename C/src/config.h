/* Portable headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* POSIX headers */
#include <sys/stat.h>  // used by the rich presence thing

/* Concord headers */
#include <concord/discord.h>
#include <concord/log.h>
#include <concord/discord_codecs.h>

/* Define stuff */
#define GUILD_ID 1234567898765431
#define CURRENT_VERSION "1.0.0"

/* Characters */
extern char BOT_TOKEN[256];

/* config file */
int startup(void);

/* basic needed stuff */
void on_ready(struct discord *client, const struct discord_ready *event);
void on_interaction(struct discord *client, const struct discord_interaction *event);
void on_message(struct discord *client, const struct discord_message *event);

/* Slash commands */
void arch_struct(struct discord *client, const struct discord_ready *event);
void arch_interaction(struct discord *client, const struct discord_interaction *event);
void fake_ban_struct(struct discord *client, const struct discord_ready *event);
void fake_ban_interaction(struct discord *client, const struct discord_interaction *event);

/* on message commands */
void hist(struct discord *client, const struct discord_message *event);
void bt_inf(struct discord *client, const struct discord_message *event);

/* Other stuff */
void rich_presence(struct discord *client, const struct discord_ready *event);
void on_cycle(struct discord *client);
static void set_custom_status(struct discord *client, const char *status, const char *text);
static void apply_presence(struct discord *client);
