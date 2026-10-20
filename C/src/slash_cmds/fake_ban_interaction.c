#include "../config.h"

void fake_ban_interaction(struct discord *client, const struct discord_interaction *event)
{
  char response_str[256];    
  // Extract the user option provided by the caller
  if (event->data->options && event->data->options->size > 0)
  {
    // Concord parses option values as strings (json_char *), so convert it using strtoull
    unsigned long long target_user_id = strtoull(event->data->options->array[0].value, NULL, 10);
        
    snprintf(response_str, sizeof(response_str), "successfully banned <@%llu>", target_user_id);
  }
  else snprintf(response_str, sizeof(response_str), "successfully banned someone");

  // Send the response back to Discord
  struct discord_interaction_response resp =
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
    .data = &(struct discord_interaction_callback_data)
    {
      .content = response_str
    }
  };

  discord_create_interaction_response(client, event->id, event->token, &resp, NULL);
}