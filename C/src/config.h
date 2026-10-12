#include <stdio.h>
#include <string.h>
#include <concord/discord.h>
#include <concord/log.h>
#include <hide_from_github/bot.h>

#define GUILD_ID 1234567898765431

void on_ready(struct discord *client, const struct discord_ready *event);
void on_interaction(struct discord *client, const struct discord_interaction *event);
void on_message(struct discord *client, const struct discord_message *event);

void arch_struct(struct discord *client, const struct discord_ready *event);
void arch_interaction(struct discord *client, const struct discord_interaction *event);
