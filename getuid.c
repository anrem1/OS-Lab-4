#include "types.h"
#include "user.h"

int main() {
    int uid = getuid();
    printf(1, "User ID: %d\n", uid);
    exit();
}
