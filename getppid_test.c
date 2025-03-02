#include "user.h"
#include "types.h"

int main() {
    printf(1,"My Parent Process ID: %d\n", getppid());
    exit();
}
