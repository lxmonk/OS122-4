#include "types.h"
#include "user.h"
#include "fcntl.h"


void checkuntag(int fd,char* key)
{
    if (funtag(fd, key) == 0)
        printf(1, "untagging key = %s successful\n",key);
    else
        printf(2, "untagging key = %s failed \n",key);
}

void checkgettag(int fd,char* key)
{
    char buf[50];
    if (gettag(fd, key,buf) > 0)
        printf(1, "gettag  successful, key =  %s, val = %s\n",key,buf);
    else
        printf(2, "gettag failed,key = %s \n",key);
}

int main(int argc, char *argv[])
{
    int fd;

    //    int i;


    /* fd = open("./find", O_RDWR); */
    /* ftag(fd, "z", "x"); */
    /* checkgettag(fd,"z"); */
    /* ftag(fd, "c", "v"); */
    /* checkgettag(fd,"c"); */
    /* close(fd); */

    if ((fd = open("find", O_CREATE | O_RDWR)) < 0) {
        printf(2,"open failed\n");
        exit();
    }

    ftag(fd, "z", "x");
    checkgettag(fd,"z");
    ftag(fd, "c", "v");
    checkgettag(fd,"c");




    ftag(fd, "key1", "value1");
    ftag(fd, "key2", "value2");
    ftag(fd, "key3", "value3");
    ftag(fd, "key4", "value4");
    ftag(fd, "key5", "value5");
    ftag(fd, "key6", "value6");
    ftag(fd, "key7", "value7");

    checkgettag(fd,"key1");
    checkgettag(fd,"key2");
    checkgettag(fd,"key3");
    checkgettag(fd,"key4");
    checkgettag(fd,"key5");
    checkgettag(fd,"key6");
    checkgettag(fd,"key7");

    ftag(fd, "key3", "c3");
    checkgettag(fd,"key3");

    checkuntag(fd,"key1");
    checkuntag(fd,"key2");
    checkuntag(fd,"key3");
    /* checkuntag(fd,"key4"); */
    /* checkuntag(fd,"key5"); */
    /* checkuntag(fd,"key6"); */
    /* checkuntag(fd,"key7"); */

    checkgettag(fd,"key1");
    checkgettag(fd,"key2");
    checkgettag(fd,"key3");
    checkgettag(fd,"key4");
    checkgettag(fd,"key5");
    checkgettag(fd,"key6");
    checkgettag(fd,"key7");
    checkgettag(fd,"key3");


    close(fd);
    exit();
}
