#include "types.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[])
{
    int fd;

    fd = open("./cat", O_RDWR);

    if (ftag(fd, "key", "value") == 0)
        printf(1, "tagging went ok\n");
    else
        printf(2, "tagging failed\n");

    if (funtag(fd, "key") == 0)
        printf(1, "untagging successful\n");
    else
        printf(2, "untagging failed \n");

    close(fd);
    exit();

}
