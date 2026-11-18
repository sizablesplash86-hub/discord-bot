#include "../config.h"

int read_cmds(void)
{
  DIR *cdr = opendir(cmds_dir);
  if (cdr == NULL)
  {
    printf("Failed to open cmds directory\n");
    return -1;
  }

  struct dirent *entry;
  while ((entry = readdir(cdr)) != NULL)
  {
    if (strcmp(entry->d_name, ".") == 0) continue;

    size_t len = strlen(entry->d_name);
    if (len > 4 && strcmp(entry->d_name + len - 4, ".app") == 0)
    {
      char cmd_path[1024];
      snprintf(cmd_path, sizeof(cmd_path), "%s/%s", cmds_dir, entry->d_name);
      printf("Opening %s...\n", cmd_path);
      if (cmd_file(cmd_path) != 0) 
      {
        printf("Error in app config\n");
        continue;
      }
    }
  }

  closedir(cdr);


  return 0;
}