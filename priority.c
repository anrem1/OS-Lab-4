#include "types.h"
#include "stat.h"
#include "user.h"

int setpriority(int pid, int priority);
int getpriority(int pid);

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        printf(1, "Need one priority \n");
        exit();
    }

    int pid = getpid();
    int old_priority, new_priority;

    old_priority = getpriority(pid);
    if (old_priority == -1) {
        printf(1, "Failed to get current priority for PID %d\n", pid);
        exit();
    }

    printf(1, "Process %d: Current priority = %d\n", pid, old_priority);

    new_priority = atoi(argv[1]);
    if (setpriority(pid, new_priority) < 0) {
        printf(1, "Failed to set new priority for PID %d\n", pid);
        exit();
    }

    int confirmed_priority = getpriority(pid);
    if (confirmed_priority == -1) {
        printf(1, "Failed to get updated priority for PID %d\n", pid);
        exit();
    }

    printf(1, "Process %d: New priority = %d\n", pid, confirmed_priority);

    exit();
}
