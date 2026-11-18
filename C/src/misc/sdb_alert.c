#include "../config.h"

// Helper to check /proc/mounts to see if sdb1 is genuinely mounted
bool is_sdb_mounted()
{
  FILE *fp = fopen("/proc/mounts", "r");
  if (!fp) return false;

  char line[512];
  bool mounted = false;
  while (fgets(line, sizeof(line), fp))
  {
    if (strstr(line, "/mnt/sdb") != NULL || strstr(line, "sdb1") != NULL)
    {
      mounted = true;
      break;
    }
  }
  fclose(fp);
  return mounted;
}

void *sdb_monitor_thread(void *arg)
{
  struct discord *client = (struct discord *)arg;
  bool already_alerted = false;

  sleep(10);

  while (1)
  {
    if (!is_sdb_mounted())
    {
      if (!already_alerted)
      {
        printf("\n[*] SDB IS OFFLINE\n");
        time_t current_time = time(NULL);
        char msg[256];
        snprintf(msg, sizeof(msg), "<@%llu> **ALERT**\nThe SDB went offline at <t:%ld:t>!", (unsigned long long)ADMIN_USER_ID, (long)current_time);
        
        struct discord_create_message params = { .content = msg };
        discord_create_message(client, ALERT_CHANNEL_ID, &params, NULL);
                
        already_alerted = true;
      }
    } 

    else
    {
      if (already_alerted)
      {
        printf("\n[*] SDB has recovered and is back online\n");
        already_alerted = false;
      }
      else printf("\n[*] SDB is online\n");
    }

    sleep(480);
  }

  return NULL;
}

int check_sdb(struct discord *client)
{
  pthread_t thread_id;
  if (pthread_create(&thread_id, NULL, sdb_monitor_thread, (void *)client) != 0) perror("Failed to create SDB monitor thread");
  else pthread_detach(thread_id);

  return 0;
}