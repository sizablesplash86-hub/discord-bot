/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Was gonna add an auto update feature, decided to not. * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#include "config.h"

int update(void)
{
  return 0;  // This was something I was gonna add but got lazy

  

  printf("\nTesting\n\n");

  if (strcmp(REPO_URL, CURRENT_VERSION) > 0)
  {
    printf("Repo URL is greater than current version\n");
    char command[256];
    char latest_version[64];
    snprintf(command, sizeof(command), "curl -s H\"User-Agent: discord-bot\" %s | grep '\"tag_name\":' | sed -E 's/.*\"([^\"]+)\".*/\\1/'", REPO_URL);
    printf("%s\n", command);

    FILE *po = popen(command, "r");
    if (po == NULL) return 0;

    if (fgets(latest_version, sizeof(latest_version), po) != NULL)
    {
      latest_version[strcspn(latest_version, "\r\n")] = '\0';
    }
    pclose(po);

    printf("latest version is %s\nCurrent verison is %s\n", latest_version, CURRENT_VERSION);

    if (strcmp(latest_version, CURRENT_VERSION) > 0)
    {
      printf("New release is here!\n%s --> %s\n", CURRENT_VERSION, latest_version);

      FILE *cf = fopen("bot_config.conf", "r");
      if (cf == NULL)
      {
        printf("Failed to open config file\n");
        return 0;
      }

      char line[512];
      int found = 0;
      while (fgets(line, sizeof(line), cf) != NULL)
      {
        if (strncmp(line, "auto_update = ", 14) == 0)
        {
          found = 1;
          break;
        }
      }
      fclose(cf);

      char *auto_upd = line + 14;
      if (found)
      {
        auto_upd[strcspn(auto_upd, "\r\n #")] = '\0';

        printf("Auto update is set to%s\n", auto_upd);

        if (strcmp(auto_upd, "yes") == 0)
        {
          printf("Auto update set to yes. Downloading latest binary...\n");
          // gave up
          return 0;
        }
      }

      else printf("Auto update not specified\n");
    }
  }
  return 0;
}