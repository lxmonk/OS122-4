#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"


int usage(void) {
    printf(2, "Usage: find <path> <options> <tests>\n");
    exit();
}

char*
namefmt(char *path)
{
    static char buf[DIRSIZ+1];
    char *p;

    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    if(strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
    return buf;
}

static char fname[512];
static int size;
static char size_modifier;
static char type;
static int follow;
static char key[10];
static char val[30];


int qualifies(int fd,struct stat st, char *name) {
    char buf[30];

    if ((fname[0] != 0) && (strcmp(fname, name) != 0))
        return 0;
    if (size != -1) {
        switch(size_modifier) {
        case 0: if (st.size != size) return 0;
            break;
        case '+' :  if (st.size <= size) return 0;
            break;
        case '-' :  if (st.size > size) return 0;
            break;
        default: break;
        }
    }
    switch (type) {
    case 'd': if (st.type != T_DIR) return 0; break;
    case 'f': if (st.type != T_FILE) return 0; break;
    case 's': if ((st.type != T_FILE) || (st.symlink == 0)) return 0; break;
    default : break;
    }
    if ((key[0] != 0) && (val[0] != 0)) {
        DEBUG_PRINT(6,"key = %s , val = %s",key,val);
        if (gettag(fd,key,buf) > 0) {
            DEBUG_PRINT(6,"key = %s , val = %s, buf = %s",key,val,buf);
            if (strcmp(val,"?") && strcmp(val,buf))
                return 0;
        }  else
            {
                DEBUG_PRINT(6,"return 0. key = %s , val = %s, buf = %s",key,val,buf);
                return 0;
            }

    }
    return 1;
}

int find(char* path, char *name) {
    char buf[512], *p;
    char sympath[512];
    int fd;
    struct dirent de;
    struct stat st;

    DEBUG_PRINT(7, "path = %s, follow = %d", path, follow);

    if ((!follow) && (readlink(path, sympath, (uint)50) != -1)) {
        /* "manually perform 'qualifies' for the file" */
        DEBUG_PRINT(7, "local 'qualifies': readlink result = %d",
                    readlink(path, sympath, (uint)50));
        if ((fname[0] != 0) && (strcmp(fname, name) != 0))
            return 0;
        if (size != -1) {
            switch(size_modifier) {
            case 0: if (0 != size) return 0;
                break;
            case '+' :  if (0 <= size) return 0;
                break;
            case '-' :  if (0 > size) return 0;
                break;
            default: break;
            }
        }
        switch (type) {
        case 'd': return 0;
        case 'f': return 0;
        default : break;
        }

        printf(1, "%s\n", path);
        return 0;
    }

    if((fd = open(path, 0)) < 0){
        printf(2, "find: cannot open %s\n", path);
        return -1;
    }

    DEBUG_PRINT(9, "fd ok", 999);

    if(fstat(fd, &st) < 0){
        printf(2, "find: cannot stat %s\n", path);
        close(fd);
        return -1;
    }

    DEBUG_PRINT(9, "stat ok", 999);

    switch(st.type){
    case T_FILE:
        DEBUG_PRINT(5, "it's a file.name = %s", name);
        if (qualifies(fd,st, name)) {
            DEBUG_PRINT(5, "file qualifies, name = %s", name);
            printf(1, "%s\n", path);
        }
        break;

    case T_DIR:
        DEBUG_PRINT(5, "it's a directory.", 999);
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf(1, "find: path too long\n");
            break;
        }
        if (readlink(path, sympath, sizeof(sympath)) != -1) {/* it is a symlink */
            DEBUG_PRINT(8, "it's a link. sympath = %s", sympath);
            strcpy(buf, sympath);
        } else {
            strcpy(buf, path);
        }

        if (qualifies(fd,st, namefmt(path))) {
            DEBUG_PRINT(5, "DIR qualifies, path = %s", path);
            printf(1, "%s\n", buf);
        }
        p = buf+strlen(buf);
        *p++ = '/';

        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;

            if (de.name[0] == '.') /* don't loop yourself to death
                                      with '.' and '..' */
                continue;
            if (follow && readlink(buf, sympath, 50) != -1) {/* it is a symlink */
                DEBUG_PRINT(8, "it's a link according to readlink", 999);
                find(sympath, namefmt(sympath));
            } else {
                DEBUG_PRINT(8, "NOT a link according to readlink. buf = %s",
                            buf);
                find(buf, de.name);
            }
        }
        break;
    }
    close(fd);
    return 0;
}

int main(int argc, char *argv[])
{

    int i;
    char *dlimiter;
    fname[0] = 0;
    size = -1;			/* initial (null) value */
    size_modifier = 0;
    type = 0;
    follow = 0;
    key[0] = 0;
    val[0] = 0;

    if (argc == 1)
        usage();

    if (!(strcmp(argv[1], "-help")) || !(strcmp(argv[2], "-help")))
        usage();

    if (!(strcmp(argv[2], "-follow")))
        follow = 1;		/* follow symlinks */

    for (i = 2 + follow; i < argc; i += 2) {
        if (!(strcmp(argv[i], "-name")))
            strcpy(fname, argv[i+1]);
        else if (!(strcmp(argv[i], "-size"))){
            switch(argv[i+1][0]) {
            case '+': size_modifier = '+'; size = atoi(&argv[i+1][1]); break;
            case '-': size_modifier = '-'; size = atoi(&argv[i+1][1]); break;
            default:  size_modifier = 0; size = atoi(&argv[i+1][0]); break;
            }
        } else if (!(strcmp(argv[i], "-type"))) {
            switch (argv[i+1][0]) {
            case 'd': type = 'd'; break;
            case 'f': type = 'f'; break;
            case 's': type = 's'; break;
            default: printf(2, "illegal file type: %s\n", argv[i+1]); return -1;
            }
        } else if (!(strcmp(argv[i], "-tag"))) {
            dlimiter=strchr(argv[i+1],'=');
            *dlimiter = 0;
            strcpy(key,argv[i+1]);
            strcpy(val,dlimiter+1);
            *dlimiter = '=';
            DEBUG_PRINT(6,"tag is: argv[i+1] = %s, key = %s, val = %s ",argv[i+1],key,val);
        }


    }
    DEBUG_PRINT(9, "fname = %s, size = %d, size_modifier = %s, type = %s",
                fname, size, size_modifier, type);
    find(argv[1], namefmt(argv[1]));
    exit();
}
