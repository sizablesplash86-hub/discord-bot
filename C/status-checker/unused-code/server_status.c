// This is meant to work with my status checker. This is a seperate project from the src.



// Chapter 14, p. 320 C Programming A Modern Approach Second Edition by K. N. King - preprocessing directives and header inclusions (#include)
#include "config.h"

// Chapter 16, p. 378 C Programming A Modern Approach Second Edition by K. N. King - declaring a struct type to bundle related variables together
struct monitor_context
{
  // Chapter 17, p. 414 C Programming A Modern Approach Second Edition by K. N. King - pointer variable inside a structure definition
  struct discord *client;
  // Chapter 7, p. 129 C Programming A Modern Approach Second Edition by K. N. King - unsigned 64-bit integer type (typedef) representing a snowflake ID
  u64snowflake status_msg_id;
  // Chapter 2, p. 18 C Programming A Modern Approach Second Edition by K. N. King - standard integer variable declaration
  int temp_alert_active;
};

// Chapter 9, p. 183 C Programming A Modern Approach Second Edition by K. N. King - function definition taking and returning generic void pointers
// Chapter 29, p. 619 The Linux Programming Interface by Michael Kerrisk - POSIX thread start routine signature: void *(*start_routine)(void *)
void *telemetry_worker(void *arg)
{
  // Chapter 17, p. 418 C Programming A Modern Approach Second Edition by K. N. King - explicit pointer type cast from void* back to struct monitor_context*
  struct monitor_context *ctx = (struct monitor_context *)arg;
  // Chapter 8, p. 161 C Programming A Modern Approach Second Edition by K. N. King - declaring a 1D character array to serve as a string buffer
  char buffer[256];

  // Chapter 6, p. 99 C Programming A Modern Approach Second Edition by K. N. King - while loop with non-zero expression (1) creating an infinite loop
  while (1)
  {
    // Chapter 23, p. 488 The Linux Programming Interface by Michael Kerrisk - sleep() function used to suspend thread execution for a given number of seconds
    sleep(60);

    // 1. Fetch Uptime & Format
    // Example uptime string (replace with actual SSH or /proc logic)
    // Chapter 22, p. 547 C Programming A Modern Approach Second Edition by K. N. King - snprintf() safe formatted output to a bounded string buffer
    // Chapter 26, p. 668 C Programming A Modern Approach Second Edition by K. N. King - time() function from <time.h> used to get current calendar time
    snprintf(buffer, sizeof(buffer), 
      "**Deb Uptime:** `12 days, 4 hours`\n"
      "**Status Checker Temp:** `48.2°C`\n"
      "*(Last Updated: <t:%ld:R>)*", time(NULL));

    // Chapter 16, p. 382 C Programming A Modern Approach Second Edition by K. N. King - C99 designated initializers for structure members (.content = buffer)
    struct discord_edit_message mod_params = { .content = buffer };

    // Chapter 5, p. 74 C Programming A Modern Approach Second Edition by K. N. King - if statement testing equality/inequality expression (!=)
    // Chapter 17, p. 419 C Programming A Modern Approach Second Edition by K. N. King - right-arrow operator (->) accessing structure member through pointer
    if (ctx->status_msg_id != 0)
    {
      // not in books. https://cogmasters.github.io/concord/discord_8h.html - Concord API call to edit an existing channel message by its snowflake ID
      discord_edit_message(ctx->client, STATUS_CHANNEL_ID, ctx->status_msg_id, &mod_params, NULL);
    }

    // 2. Check Thermal Threshold (Hysteresis check)
    // Chapter 7, p. 136 C Programming A Modern Approach Second Edition by K. N. King - floating-point type (float) declaration and initialization
    float current_temp = 76.5; // read from sysfs

    // Chapter 5, p. 75 C Programming A Modern Approach Second Edition by K. N. King - compound logical condition using AND (&&) and NOT (!) operators
    if (current_temp >= 75.0 && !ctx->temp_alert_active)
    {
      // Chapter 2, p. 19 C Programming A Modern Approach Second Edition by K. N. King - assigning an integer value to a variable
      ctx->temp_alert_active = 1;
      char alert[128];
      snprintf(alert, sizeof(alert), "<@%llu> **WARNING:** Status Checker temperature is %.1f°C!", ADMIN_USER_ID, current_temp);
      struct discord_create_message msg_params = { .content = alert };
      // not in books. https://cogmasters.github.io/concord/discord_8h.html - Concord API call to post a new message to a channel
      discord_create_message(ctx->client, ALERT_CHANNEL_ID, &msg_params, NULL);
    }
    else if (current_temp <= 68.0 && ctx->temp_alert_active)
    {
      ctx->temp_alert_active = 0;
      struct discord_create_message msg_params = { .content = "**RESOLVED:** Temperature normalized." };
      // not in books. https://cogmasters.github.io/concord/discord_8h.html - Concord API call to post a new message to a channel
      discord_create_message(ctx->client, ALERT_CHANNEL_ID, &msg_params, NULL);
    }
  }
  // Chapter 9, p. 185 C Programming A Modern Approach Second Edition by K. N. King - return statement passing NULL pointer out of void* function
  return NULL;
}

// Chapter 9, p. 183 C Programming A Modern Approach Second Edition by K. N. King - function definition taking pointers to structs
// Chapter 17, p. 438 C Programming A Modern Approach Second Edition by K. N. King - const qualifier ensuring pointee structure data is read-only
void on_ready(struct discord *client, const struct discord_ready *event)
{
  // Chapter 2, p. 23 C Programming A Modern Approach Second Edition by K. N. King - printf() formatted console printing using %s conversion specifier
  // Chapter 17, p. 419 C Programming A Modern Approach Second Edition by K. N. King - chaining arrow operators (->) across nested pointer-to-struct members
  printf("Logged in as %s#%s\n", event->user->username, event->user->discriminator);
}

// Chapter 9, p. 184 C Programming A Modern Approach Second Edition by K. N. King - function returning an int with void parameter list
int bot_run(void)
{
    // Initialize the Concord client handle with the bot token
    struct discord *client = discord_init(BOT_TOKEN);

    // Registers callback function triggered on Discord READY gateway event
    discord_set_on_ready(client, &on_ready);

    // Designated initializer for local struct variable
    struct monitor_context ctx =
    {
        .client = client,
        .status_msg_id = 112233445566778899ULL, // Pre-seeded or created message
        .temp_alert_active = 0
    };

    // Chapter 29, p. 620 The Linux Programming Interface by Michael Kerrisk - pthread_t thread identifier data type
    pthread_t thread_id;

    // Chapter 29, p. 620 The Linux Programming Interface by Michael Kerrisk - pthread_create() function to start a new POSIX thread concurrently
    if (pthread_create(&thread_id, NULL, telemetry_worker, &ctx) != 0)
    {
        fprintf(stderr, "Error: Failed to create telemetry worker thread.\n");
        discord_cleanup(client);
        return 1;
    }

    // Detach the thread so it cleans up automatically when it exits (optional, but good practice for long-running workers)
    pthread_detach(thread_id);

    // Start the Discord gateway event loop (blocks and runs the bot)
    discord_run(client);

    // Cleanup resources if discord_run ever exits
    discord_cleanup(client);
    return 0;
}

int main(void)
{
  bot_run();
}