#include "types.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[])
{
    int fd, i;
    char kb[1024];
    /* char ten_kb[1024*10]; */
    /* char cent_kb[1024*100]; */

    memset(kb, 'z', 1024);
    /* memset(ten_kb, 'y', 10240); */
    /* memset(cent_kb, 'x', 102400); */


    fd = open("bigfile", O_CREATE | O_RDWR);
    for (i = 0; i < 1024; i++) {
        write(fd, kb, sizeof kb);
        if (i == 5)
            printf(2, "Finished writing 6KB (direct)\n");
        if (i == 69)
            printf(2, "Finished writing 70KB (single indirect)\n");
        if ((i % 100) == 99)
            printf(2, "Finished writing %dKB\n", i+1);

    }

    printf(2, "Finished writing 1MB\n");

    close(fd);

    exit();
}
