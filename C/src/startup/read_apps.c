#include "../config.h"

int read_apps(void)
{
  DIR *apdr = opendir(apps_dir);
  if (apdr == NULL)
  {
    printf("Failed to open apps directory\n");
    return -1;
  }

  struct dirent *entry;
  while ((entry = readdir(apdr)) != NULL)
  {
    if (strcmp(entry->d_name, ".") == 0) continue;

    size_t len = strlen(entry->d_name);
    if (len > 4 && strcmp(entry->d_name + len - 4, ".app") == 0)
    {
      char app_path[1024];
      snprintf(app_path, sizeof(app_path), "%s/%s", apps_dir, entry->d_name);
      printf("Opening %s...\n", app_path);
      if (app_file(app_path) != 0) 
      {
        printf("Error in app config\n");
        continue;
      }
    }
  }

  closedir(apdr);

  return 0;
}