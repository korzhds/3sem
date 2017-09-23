#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <malloc.h>


int main() {
  char ** a;
  char s[256] = "";
  pid_t pid;
  printf("Enter argumets or 'quit' for exit\n");
  fgets(s, 255, stdin);
  int status;
  while(strcmp(s, "quit\n") != 0) {
    int k = 0;
    int j = 0, i = 0;
    a = (char **) malloc(sizeof(char *));
    while (s[i] != '\n') {
      a = (char **) realloc(a, (k + 1) * sizeof(char *));
      a[k] = (char *) malloc(256);
      j = 0;
      while (s[i] == ' ')
        i++;
      while ((s[i] != ' ') && (s[i] != '\n')) {
        a[k][j] = s[i];
        i++;
        j++;
      }
      a[k][j] = '\0';
      k++;
    }
    a = (char **) realloc(a, (k + 1) * sizeof(char *));
    a[k] = NULL;
    switch(pid = fork()) {
      case 0:
        execvp(a[0], a);
        printf("Error in execv\n");
        exit(2);
        break;
      case -1:
        printf("Error in fork\n");
        exit(42);
        break;
      default:
        waitpid(pid, &status, 0);
        printf("Ret code: %d\n", WEXITSTATUS(status));
    }
    for (int m = 0; m < k; m++) {
      free(a[m]);
    }
    free(a);
    printf("Enter argumets or 'quit' for exit\n");
    fgets(s, 255, stdin);
  }
  return 0;
}
