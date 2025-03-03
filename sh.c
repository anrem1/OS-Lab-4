// Shell implementation

#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "stat.h"
#include <stddef.h> // Added to define NULL

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

#define MAXARGS 10

struct cmd {
  int type;
};

struct execcmd {
  int type;
  char *argv[MAXARGS];
};

struct redircmd {
  int type;
  struct cmd *cmd;
  char *file;
  int mode;
  int fd;
};

struct pipecmd {
  int type;
  struct cmd *left;
  struct cmd *right;
};

struct listcmd {
  int type;
  struct cmd *left;
  struct cmd *right;
};

struct backcmd {
  int type;
  struct cmd *cmd;
};

int fork1(void);  // Fork but panics on failure.
void panic(char*);
struct cmd *parsecmd(char*);

void runcmd(struct cmd *cmd) {
  int p[2];
  struct backcmd *bcmd;
  struct execcmd *ecmd;
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if (cmd == NULL)
    return;

  switch (cmd->type) {
  case EXEC:
    ecmd = (struct execcmd*)cmd;
    if (ecmd->argv[0] == NULL)
      return;
    exec(ecmd->argv[0], ecmd->argv);
    printf(2, "exec %s failed\n", ecmd->argv[0]);
    break;

  case REDIR:
    rcmd = (struct redircmd*)cmd;
    close(rcmd->fd);
    if (open(rcmd->file, rcmd->mode) < 0) {
      printf(2, "open %s failed\n", rcmd->file);
      return;
    }
    runcmd(rcmd->cmd);
    break;

  case LIST:
    lcmd = (struct listcmd*)cmd;
    if (lcmd->left) {
      if (fork1() == 0)
        runcmd(lcmd->left);
      wait();
    }
    if (lcmd->right)
      runcmd(lcmd->right);
    break;

  case PIPE:
    pcmd = (struct pipecmd*)cmd;
    if (pipe(p) < 0)
      panic("pipe failed");
    if (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->left);
    }
    if (fork1() == 0) {
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->right);
    }
    close(p[0]);
    close(p[1]);
    wait();
    wait();
    break;

  case BACK:
    bcmd = (struct backcmd*)cmd;
    if (bcmd->cmd)
      if (fork1() == 0)
        runcmd(bcmd->cmd);
    break;
  }
}

int getcmd(char *buf, int nbuf) {
  printf(2, "$ ");
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  if(buf[0] == 0)
    return -1;
  return 0;
}

int main(void) {
  static char buf[100];
  int fd;

  while((fd = open("console", O_RDWR)) >= 0) {
    if(fd >= 3) {
      close(fd);
      break;
    }
  }

  while(getcmd(buf, sizeof(buf)) >= 0) {
    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' ') {
      buf[strlen(buf)-1] = 0;
      if(chdir(buf+3) < 0)
        printf(2, "cannot cd %s\n", buf+3);
      continue;
    }
    if(fork1() == 0)
      runcmd(parsecmd(buf));
    wait();
  }
  exit();
}

void panic(char *s) {
  printf(2, "%s\n", s);
  exit();
}

int fork1(void) {
  int pid = fork();
  if(pid == -1)
    panic("fork");
  return pid;
}
