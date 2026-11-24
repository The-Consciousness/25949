#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void format_mode(mode_t mode, char out[11]) {
    if (S_ISDIR(mode)) {
        out[0] = 'd';
    } else if (S_ISREG(mode)) {
        out[0] = '-';
    } else {
        out[0] = '?';
    }

    out[1] = (mode & S_IRUSR) ? 'r' : '-';
    out[2] = (mode & S_IWUSR) ? 'w' : '-';
    out[3] = (mode & S_IXUSR) ? 'x' : '-';

    out[4] = (mode & S_IRGRP) ? 'r' : '-';
    out[5] = (mode & S_IWGRP) ? 'w' : '-';
    out[6] = (mode & S_IXGRP) ? 'x' : '-';

    out[7] = (mode & S_IROTH) ? 'r' : '-';
    out[8] = (mode & S_IWOTH) ? 'w' : '-';
    out[9] = (mode & S_IXOTH) ? 'x' : '-';

    out[10] = '\0';
}

int print_entry(const char *path) {
    struct stat st;
    if (lstat(path, &st) == -1) {
        perror(path);
        return 1;
    }

    char mode_str[11];
    format_mode(st.st_mode, mode_str);

    char owner_buf[32];
    const char *owner;
    struct passwd *pw = getpwuid(st.st_uid);
    if (pw != NULL) {
        owner = pw->pw_name;
    } else {
        snprintf(owner_buf, sizeof(owner_buf), "%lu", (unsigned long)st.st_uid);
        owner = owner_buf;
    }

    char group_buf[32];
    const char *group;
    struct group *gr = getgrgid(st.st_gid);
    if (gr != NULL) {
        group = gr->gr_name;
    } else {
        snprintf(group_buf, sizeof(group_buf), "%lu", (unsigned long)st.st_gid);
        group = group_buf;
    }

    char size_buf[32];
    if (S_ISREG(st.st_mode)) {
        snprintf(size_buf, sizeof(size_buf), "%lld", (long long)st.st_size);
    } else {
        size_buf[0] = '\0';
    }

    char time_buf[32] = "";
    char *ct = ctime(&st.st_mtime);
    if (ct != NULL) {
        snprintf(time_buf, sizeof(time_buf), "%.24s", ct);
    }

    printf("%s %4lu %-12s %-12s %10s %-24s %s\n",
           mode_str,
           (unsigned long)st.st_nlink,
           owner,
           group,
           size_buf,
           time_buf,
           path);

    return 0;
}

int main(int argc, char *argv[]) {
    int exit_code = 0;

    if (argc < 2) {
        exit_code |= print_entry(".");
    } else {
        for (int i = 1; i < argc; i++) {
            exit_code |= print_entry(argv[i]);
        }
    }

    return exit_code;
}
