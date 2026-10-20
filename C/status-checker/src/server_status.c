/* * * * * * * * * * * * * * * * * *
 * Status checker on the Sony VAIO *
 * * * * * * * * * * * * * * * * * */

#include "config.h"

/* Global configuration state */
static bot_config cfg;

/* Global runtime state */
static u64snowflake status_message_id = 0;
static bool status_msg_searched = false;
static bool temp_alert_active = false;
static bool last_server_state = false;
static bool has_run_once = false;
static time_t downtime_started_at = 0;
static bool server_offline_alert_active = false;

/* Previous CPU values for calculating delta load */
static unsigned long long prev_user = 0, prev_nice = 0, prev_system = 0, prev_idle = 0, prev_iowait = 0, prev_irq = 0, prev_softirq = 0;

/* Check TCP port availability with timeout */
static bool check_remote_server(const char *host, int port_num, int timeout_sec)
{
  char port_str[16];
  snprintf(port_str, sizeof(port_str), "%d", port_num);

  struct addrinfo hints, *res, *rp;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(host, port_str, &hints, &res) != 0) return false;

  bool connected = false;
  for (rp = res; rp != NULL; rp = rp->ai_next)
  {
    int fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (fd < 0) continue;

    struct timeval tv = { .tv_sec = timeout_sec, .tv_usec = 0 };
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv, sizeof(tv));

    if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0)
    {
      connected = true;
      close(fd);
      break;
    }
    close(fd);
  }
  freeaddrinfo(res);
  return connected;
}

/* Fetch remote boot time timestamp via SSH */
static time_t get_remote_server_boot_time(const char *host, const char *user)
{
  char cmd[512];
  snprintf(cmd, sizeof(cmd), "ssh -o ConnectTimeout=3 -o StrictHostKeyChecking=no %s@%s 'date -d \"$(uptime -s)\" +%%s' 2>/dev/null", user, host);

  FILE *fp = popen(cmd, "r");
  if (!fp) return 0;

  char buf[64];
  time_t boot_time = 0;
  if (fgets(buf, sizeof(buf), fp) != NULL) boot_time = (time_t)atoll(buf);
  pclose(fp);
  return boot_time;
}

/* Scans /sys/class/thermal/thermal_zone* for CPU temperatures */
static double get_laptop_temp_raw(void)
{
  glob_t globbuf;
  double max_temp = -1.0;

  if (glob("/sys/class/thermal/thermal_zone*/temp", 0, NULL, &globbuf) == 0)
  {
    for (size_t i = 0; i < globbuf.gl_pathc; i++)
    {
      FILE *f = fopen(globbuf.gl_pathv[i], "r");
      if (f)
      {
        long millideg = 0;
        if (fscanf(f, "%ld", &millideg) == 1)
        {
          double c = (double)millideg / 1000.0;
          if (c >= 15.0 && c <= 110.0 && c > max_temp) max_temp = c;
        }
        fclose(f);
      }
    }
    globfree(&globbuf);
  }
  return max_temp;
}

/* Calculate CPU usage percentage and memory usage from /proc */
static bool get_system_stats(char *cpu_out, size_t cpu_size, char *ram_out, size_t ram_size)
{
  FILE *fp = fopen("/proc/stat", "r");
  if (!fp) return false;

  unsigned long long user, nice, system, idle, iowait, irq, softirq;
  if (fscanf(fp, "cpu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait, &irq, &softirq) != 7)
  {
    fclose(fp);
    return false;
  }
  fclose(fp);

  double cpu_percent = 0.0;
  if (prev_user || prev_idle) 
  {
    unsigned long long prev_idle_all = prev_idle + prev_iowait;
    unsigned long long idle_all = idle + iowait;

    unsigned long long prev_non_idle = prev_user + prev_nice + prev_system + prev_irq + prev_softirq;
    unsigned long long non_idle = user + nice + system + irq + softirq;

    unsigned long long prev_total = prev_idle_all + prev_non_idle;
    unsigned long long total = idle_all + non_idle;

    unsigned long long total_d = total - prev_total;
    unsigned long long idle_d = idle_all - prev_idle_all;

    if (total_d > 0) cpu_percent = (double)(total_d - idle_d) / (double)total_d * 100.0;
  }
  prev_user = user; prev_nice = nice; prev_system = system;
  prev_idle = idle; prev_iowait = iowait; prev_irq = irq; prev_softirq = softirq;

  fp = fopen("/proc/meminfo", "r");
  if (!fp) return false;

  char line[128];
  unsigned long long mem_total_kb = 0, mem_avail_kb = 0;
  while (fgets(line, sizeof(line), fp)) {
    if (strncmp(line, "MemTotal:", 9) == 0) sscanf(line, "MemTotal: %llu kB", &mem_total_kb);
    if (strncmp(line, "MemAvailable:", 13) == 0) sscanf(line, "MemAvailable: %llu kB", &mem_avail_kb);
  }
  fclose(fp);

  if (mem_total_kb == 0) return false;

  unsigned long long mem_used_kb = mem_total_kb - mem_avail_kb;
  double mem_percent = ((double)mem_used_kb / (double)mem_total_kb) * 100.0;
  double mem_used_gb = (double)mem_used_kb / (1024.0 * 1024.0);
  double mem_total_gb = (double)mem_total_kb / (1024.0 * 1024.0);

  snprintf(cpu_out, cpu_size, "`%.1f%%`", cpu_percent);
  snprintf(ram_out, ram_size, "`%.1f%%` (`%.2f GB / %.2f GB`)", mem_percent, mem_used_gb, mem_total_gb);
  return true;
}

/* Builds Concord embed structure using safe buffers */
static struct discord_embed build_status_embed(bool is_online, double temp_val, time_t remote_boot_time, struct discord_embed_field fields[], int *field_count, char *status_val, size_t status_val_size, char *temp_val_str, size_t temp_val_size, char *temp_channel_str, size_t temp_channel_size, char *updated_str, size_t updated_str_size)
{
  bool is_temp_critical = (temp_val >= cfg.temp_threshold);
  int color = (!is_online || is_temp_critical) ? 0xED4245 /* RED */ : 0x57F287 /* GREEN */;

  if (is_online)
  {
    if (remote_boot_time > 0) snprintf(status_val, status_val_size, "**ONLINE**\n*Online since <t:%ld:f> (<t:%ld:R>)*", (long)remote_boot_time, (long)remote_boot_time);
    else snprintf(status_val, status_val_size, "**ONLINE**");
  } 
  else
  {
    if (downtime_started_at > 0) snprintf(status_val, status_val_size, "**OFFLINE**\n*Offline since <t:%ld:f> (<t:%ld:R>)*", (long)downtime_started_at, (long)downtime_started_at);
    else snprintf(status_val, status_val_size, "**OFFLINE**");
  }

  if (temp_val > 0.0) snprintf(temp_val_str, temp_val_size, "`%.1f°C`%s", temp_val, is_temp_critical ? "**CRITICAL**" : "");
  else snprintf(temp_val_str, temp_val_size, "`Unavailable`");

  static char cpu_str[64], ram_str[128];
  bool has_stats = get_system_stats(cpu_str, sizeof(cpu_str), ram_str, sizeof(ram_str));

  snprintf(temp_channel_str, temp_channel_size, "Check <#%d>", cfg.temp_channel_id);

  time_t now = time(NULL);
  snprintf(updated_str, updated_str_size, "<t:%ld:t> (<t:%ld:R>)", (long)now, (long)now);

  int idx = 0;
  fields[idx++] = (struct discord_embed_field){ .name = "HP Compaq Server", .value = status_val, .Inline = false };
  fields[idx++] = (struct discord_embed_field){ .name = "Status Checker Temp", .value = temp_val_str, .Inline = false };

  if (has_stats)
  {
    fields[idx++] = (struct discord_embed_field){ .name = "Status Checker CPU Load", .value = cpu_str, .Inline = true };
    fields[idx++] = (struct discord_embed_field){ .name = "Status Checker RAM Usage", .value = ram_str, .Inline = true };
    fields[idx++] = (struct discord_embed_field){ .name = "To see the server load", .value = temp_channel_str, .Inline = false };
  }

  fields[idx++] = (struct discord_embed_field){ .name = "Last Updated", .value = updated_str, .Inline = false };
  *field_count = idx;

  return (struct discord_embed)
  {
    .title = "Server Status",
    .color = color,
    .fields = &(struct discord_embed_fields)
    {
      .size = *field_count,
      .array = fields
    }
  };
}

/* Combined periodic worker loop */
static void on_check_timer(struct discord *client)
{
  time_t current_time = time(NULL);

  bool is_online = check_remote_server(cfg.server_host, cfg.server_port, 3);
  double temp_val = get_laptop_temp_raw();

  time_t remote_boot_time = 0;
  if (is_online)
  {
    remote_boot_time = get_remote_server_boot_time(cfg.server_host, cfg.ssh_user);
    downtime_started_at = 0;
  }
  else
  {
    if (!has_run_once || last_server_state == true || downtime_started_at == 0)
    {
      downtime_started_at = current_time;
    }
  }
  last_server_state = is_online;
  has_run_once = true;

  if (!is_online && !server_offline_alert_active)
  {
    server_offline_alert_active = true;
    char msg[256];
    snprintf(msg, sizeof(msg), "<@%d> **SERVER OFFLINE ALERT**\nThe HP Compaq Server went offline at <t:%ld:t>!", cfg.admin_user_id, (long)current_time);

    struct discord_create_message params = { .content = msg };
    discord_create_message(client, cfg.alert_channel_id, &params, NULL);
  }
  else if (is_online && server_offline_alert_active)
  {
    server_offline_alert_active = false;
    char msg[] = "**SERVER RECOVERED**\nThe HP Compaq Server is back online.";
    struct discord_create_message params = { .content = msg };
    discord_create_message(client, cfg.alert_channel_id, &params, NULL);
  }

  if (temp_val > 0.0)
  {
    if (temp_val >= cfg.temp_threshold && !temp_alert_active)
    {
      temp_alert_active = true;
      char msg[256];
      snprintf(msg, sizeof(msg), "<@%d> **HIGH TEMPERATURE ALERT**\nLaptop CPU temperature is critical: **%.1f°C**!", cfg.admin_user_id, temp_val);

      struct discord_create_message params = { .content = msg };
      discord_create_message(client, cfg.alert_channel_id, &params, NULL);
    } 
    else if (temp_val < (cfg.temp_threshold - 5.0) && temp_alert_active)
    {
      temp_alert_active = false;
      char msg[256];
      snprintf(msg, sizeof(msg), "**TEMPERATURE NORMALIZED**\nLaptop CPU cooled down to **%.1f°C**.", temp_val);
      struct discord_create_message params = { .content = msg };
      discord_create_message(client, cfg.alert_channel_id, &params, NULL);
    }
  }

  if (!status_msg_searched)
  {
    status_msg_searched = true;
    struct discord_get_channel_messages params = { .limit = 20 };
    struct discord_messages msgs = { 0 };

    struct discord_ret_messages ret = { .sync = &msgs };
    if (CCORD_OK == discord_get_channel_messages(client, cfg.status_channel_id, &params, &ret))
    {
      const struct discord_user *bot_user = discord_get_self(client);
      for (int i = 0; i < msgs.size; i++)
      {
        if (msgs.array[i].author->id == bot_user->id && msgs.array[i].embeds->size > 0)
        {
          status_message_id = msgs.array[i].id;
          break;
        }
      }
      discord_messages_cleanup(&msgs);
    }
  }

  struct discord_embed_field fields[8];
  int field_count = 0;
  char status_val[256], temp_val_str[128], temp_channel_str[64], updated_str[128];
    
  struct discord_embed embed = build_status_embed(is_online, temp_val, remote_boot_time, fields, &field_count, status_val, sizeof(status_val), temp_val_str, sizeof(temp_val_str), temp_channel_str, sizeof(temp_channel_str), updated_str, sizeof(updated_str));

  bool edited = false;
  if (status_message_id != 0)
  {
    struct discord_edit_message edit_params = { .embeds = &(struct discord_embeds){ .size = 1, .array = &embed } };
    if (CCORD_OK == discord_edit_message(client, cfg.status_channel_id, status_message_id, &edit_params, NULL)) edited = true;
  }

  if (!edited)
  {
    struct discord_create_message send_params = { .embeds = &(struct discord_embeds){ .size = 1, .array = &embed } };
    struct discord_message sent_msg = { 0 };
    struct discord_ret_message ret = { .sync = &sent_msg };
        
    if (CCORD_OK == discord_create_message(client, cfg.status_channel_id, &send_params, &ret))
    {
      status_message_id = sent_msg.id;
      discord_message_cleanup(&sent_msg);
    }
  }
}

static void *status_worker_thread(void *arg)
{
  struct discord *client = (struct discord *)arg;
    
  while (true)
  {
    on_check_timer(client);
    long interval = cfg.check_interval_ms > 0 ? cfg.check_interval_ms / 1000 : 15;
    sleep(interval);
  }
  return NULL;
}

static void on_ready(struct discord *client, const struct discord_ready *event)
{
  log_info("Status Monitor Bot logged in as %s#%s!", event->user->username, event->user->discriminator);

  pthread_t worker_tid;
  if (pthread_create(&worker_tid, NULL, status_worker_thread, client) != 0) log_error("Failed to create status worker thread!");
  else pthread_detach(worker_tid);
}

int main(void)
{
  cfg = startup();

  if (strcmp(cfg.active, "yes") != 0)
  {
    printf("\n[*] Bot is currently disabled in 'bot_config.conf'. Set ACTIVE = yes to run.\n");
    return 0;
  }

  struct discord *client = discord_init(cfg.bot_token);
  discord_set_on_ready(client, &on_ready);
  discord_run(client);

  discord_cleanup(client);
  return 0;
}