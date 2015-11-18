#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int get_pids_by_name(const char * procname, int foundpid[])
{
    DIR *dir;
    struct dirent *d;
    int pid, i;
    char *s;
    int pnlen;

    i = 0;
    foundpid[0] = 0;
    pnlen = strlen(procname);

    /* Open the /proc directory. */
    dir = opendir("/proc");
    if (!dir) {
        printf("cannot open /proc");
        return -1;
    }

    /* Walk through the directory. */
    while ((d = readdir(dir)) != NULL) {
        char exe [PATH_MAX+1];
        char path[PATH_MAX+1];
        int len;
        int namelen;

        /* See if this is a process */
        if ((pid = atoi(d->d_name)) == 0) continue;

        snprintf(exe, sizeof(exe), "/proc/%s/exe", d->d_name);
        if ((len = readlink(exe, path, PATH_MAX)) < 0)
            continue;
        path[len] = '\0';

        /* Find ProcName */
        s = strrchr(path, '/');
        if (s == NULL) continue;
            s++;

        /* we don't need small name len */
        namelen = strlen(s);
        if (namelen < pnlen) continue;

        if (!strncmp(procname, s, pnlen)) {
            /* to avoid subname like search proc tao but proc taolinke matched */
            if (s[pnlen] == ' ' || s[pnlen] == '\0') {
                foundpid[i] = pid;
                i++;
            }
        }
    }

    foundpid[i] = 0;
    closedir(dir);

    return  0;
}

int main(int argc, char *argv[])
{
    const char * name = NULL;
    int pids[1024] = {0};
    int vid = -1;
    int sig = -1;

    static const char *shortopts = "v:s:p:n:";
    while(1) {
        int opt = getopt(argc, argv, shortopts);
        if (-1 == opt) {
            break;
        }
        switch (opt) {
            case 'v':
                vid = atoi(optarg);
                break;
            case 's':
                sig = atoi(optarg);
                break;
            case 'p':
                pids[0] = atoi(optarg);
                break;
            case 'n':
                name = optarg;
                if (get_pids_by_name(name, pids)) {
                    return 0;
                }
                break;
            default:
                break;
        }
    }
    printf("vid=%d sig=%d\n", vid, sig);
    if (-1 == vid || -1 == sig) {
        fprintf(stderr, "Error : invalid params\n");
        return -1;
    }

    union sigval value;
    value.sival_int = vid;
    int i = 0;
    for (; 0 != pids[i]; i++) {
        printf("pid = %d\n", pids[i]);
        if (0 != sigqueue(pids[i], sig, value)) {
            fprintf(stderr, "Error : send error\n");
            return -2;
        }
    }
    return 0;
}
