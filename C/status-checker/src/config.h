/* Portable headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // (introduced in C99)
#include <time.h>
#include <inttypes.h> // (introduced in C99)

/* POSIX headers */
#include <unistd.h> // (POSIX standard symbolic constants and system call prototypes)
#include <glob.h> // (Pathname pattern matching)
#include <pthread.h> // (POSIX threads)
#include <netdb.h> // (Network database operations and DNS resolution)
#include <sys/types.h> // (Fundamental system data types like pid_t and uid_t)
#include <sys/socket.h> // (Socket networking interfaces)
#include <sys/time.h> // (Additional time structures like timeval)

/* Concord C Library */
#include <concord/discord.h>
#include <concord/log.h>

/* Libssh Library */
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <libssh2_publickey.h>

/* bot token and such */
#include <hide_from_github/bot.h>


/* Unused... I think */
#define _POSIX_C_SOURCE 200809L

/* structure things */
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

/* void commands */
bot_config startup(void);
//void conf_file(void);

/* characters */
