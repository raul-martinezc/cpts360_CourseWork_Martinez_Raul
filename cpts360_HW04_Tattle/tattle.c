#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>    // this defines strdup() only *if* _XOPEN_SOURCE is set before it
#include <utmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <errno.h>

#define MAX_RECORDS 10000
#define MAX_USERS 128

struct login_record {
    char username[UT_NAMESIZE];
    char tty[UT_LINESIZE];
    time_t log_on;
    time_t log_off;
    char host[UT_HOSTSIZE];
};

struct login_record records[MAX_RECORDS];
int record_count = 0;

char *filter_users[MAX_USERS];
int num_filter_users = 0;
time_t filter_start = 0;
time_t filter_end = 0;

void error_exit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int compare_logins(const void *a, const void *b) {
    struct login_record *r1 = (struct login_record *)a;
    struct login_record *r2 = (struct login_record *)b;
    return (r1->log_on > r2->log_on) - (r1->log_on < r2->log_on);
}

int user_matches(const char *username) {
    if (num_filter_users == 0) return 1;
    for (int i = 0; i < num_filter_users; ++i) {
        if (strcmp(username, filter_users[i]) == 0)
            return 1;
    }
    return 0;
}

int time_overlaps(time_t start, time_t end) {
    if (filter_start == 0 && filter_end == 0) return 1;
    if (end == 0) end = time(NULL);
    return !(end < filter_start || start > filter_end);
}


void parse_users(const char *arg) {
    char *copy = strdup(arg);
    char *token = strtok(copy, ",");
    while (token) {
        if (num_filter_users >= MAX_USERS) {
            fprintf(stderr, "error: too many usernames provided (max %d)\n", MAX_USERS);
            free(copy);
            exit(EXIT_FAILURE);
        }

        char *dup = strdup(token);
        if (!dup) {
            fprintf(stderr, "error: strdup failed for user '%s'\n", token);
            free(copy);
            exit(EXIT_FAILURE);
        }

        filter_users[num_filter_users++] = dup;
        token = strtok(NULL, ",");
    }
    free(copy);
}


void parse_date_time(const char *date_str, const char *time_str) {
    struct tm tm_time = {0};
    time_t now = time(NULL);
    struct tm *local = localtime(&now);

    if (date_str) {
        strptime(date_str, "%m/%d/%y", &tm_time);
    } else {
        tm_time.tm_year = local->tm_year;
        tm_time.tm_mon = local->tm_mon;
        tm_time.tm_mday = local->tm_mday;
    }

    if (time_str) {
        strptime(time_str, "%H:%M", &tm_time);
        filter_start = mktime(&tm_time);
        filter_end = filter_start;
    } else {
        filter_start = mktime(&tm_time);
        filter_end = filter_start + 86399;
    }
}

void process_wtmp(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) error_exit("open");

    struct utmp entry;
    struct login_record *active_logins[MAX_RECORDS] = {0};

    while (read(fd, &entry, sizeof(entry)) == sizeof(entry)) {
        if (entry.ut_type == USER_PROCESS) {
            struct login_record *rec = &records[record_count++];
            strncpy(rec->username, entry.ut_user, UT_NAMESIZE);
            strncpy(rec->tty, entry.ut_line, UT_LINESIZE);
            rec->log_on = entry.ut_time;
            rec->log_off = 0;
            strncpy(rec->host, entry.ut_host, UT_HOSTSIZE);
            for (int i = 0; i < MAX_RECORDS; ++i) {
                if (!active_logins[i]) {
                    active_logins[i] = rec;
                    break;
                }
            }
        } else if (entry.ut_type == DEAD_PROCESS) {
            for (int i = 0; i < MAX_RECORDS; ++i) {
                if (active_logins[i] &&
                    strncmp(active_logins[i]->tty, entry.ut_line, UT_LINESIZE) == 0) {
                    active_logins[i]->log_off = entry.ut_time;
                    active_logins[i] = NULL;
                    break;
                }
            }
        } else if (entry.ut_type == BOOT_TIME) {
            for (int i = 0; i < MAX_RECORDS; ++i) {
                if (active_logins[i]) {
                    active_logins[i]->log_off = entry.ut_time;
                    active_logins[i] = NULL;
                }
            }
        }
    }

    close(fd);
}

void print_record(struct login_record *rec) {
    if (!user_matches(rec->username)) return;
    if (!time_overlaps(rec->log_on, rec->log_off)) return;

    char logon_buf[32], logoff_buf[64];
    struct tm *tm_info;

    tm_info = localtime(&rec->log_on);
    strftime(logon_buf, sizeof(logon_buf), "%m/%d/%y %H:%M", tm_info);

    if (rec->log_off == 0) {
        snprintf(logoff_buf, sizeof(logoff_buf), "(still logged in)");
    } else {
        tm_info = localtime(&rec->log_off);
        strftime(logoff_buf, sizeof(logoff_buf), "%m/%d/%y %H:%M", tm_info);
    }

    printf("%-10s %-8s %-17s %-17s %-20s\n", rec->username, rec->tty, logon_buf, logoff_buf, rec->host);
}

void print_all_records() {
    qsort(records, record_count, sizeof(struct login_record), compare_logins);
    printf("%-10s %-8s %-17s %-17s %-20s\n", "login", "tty", "log on", "log off", "from host");
    for (int i = 0; i < record_count; ++i) {
        print_record(&records[i]);
    }
}

int main(int argc, char *argv[]) {
    const char *filename = "/var/log/wtmp";
    const char *date_arg = NULL;
    const char *time_arg = NULL;

    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-f") && i + 1 < argc) {
            filename = argv[++i];
        } else if (!strcmp(argv[i], "-u") && i + 1 < argc) {
            parse_users(argv[++i]);
        } else if (!strcmp(argv[i], "-d") && i + 1 < argc) {
            date_arg = argv[++i];
        } else if (!strcmp(argv[i], "-t") && i + 1 < argc) {
            time_arg = argv[++i];
        } else {
            fprintf(stderr, "bad date or time or unknown argument: %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }

    parse_date_time(date_arg, time_arg);
    process_wtmp(filename);
    print_all_records();
    return 0;
}
