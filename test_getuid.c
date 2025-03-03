#include "types.h"
#include "stat.h"
#include "user.h"

int main() {
    int uid = getuid();  // Call the new system call
    printf("User ID: %d\n", uid);
    exit();
}
