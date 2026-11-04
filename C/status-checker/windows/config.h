#ifndef CONFIG_H
#define CONFIG_H

/* Portable headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <inttypes.h>

/* Windows headers */
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <process.h>

/* Concord C Library */
#include "discord.h"
//#include <concord/discord_codecs.h>
#include "log.h"

/* Libssh2 Library */
#include "libssh2.h"
#include "libssh2_sftp.h"
#include "libssh2_publickey.h"

#define sleep(sec) Sleep((sec) * 1000)

/* Structure definitions */
typedef struct
{
  char active[8];
  char bot_token[128];
  u64snowflake testing_channel;
  u64snowflake status_channel_id;
  u64snowflake admin_user_id;
  u64snowflake alert_channel_id;
  u64snowflake temp_channel_id;
  u64snowflake status_msg_id;
  u64snowflake temp_msg_id;
  char server_host[256];
  int server_port;
  char ssh_user[64];
  float temp_threshold;
  long check_interval_ms;
} bot_config;

/* Function prototypes */
bot_config startup(void);
void save_persisted_msg_id(u64snowflake msg_id);

#endif
