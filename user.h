// userinfo.h
#ifndef USERINFO_H
#define USERINFO_H

#define BUFFER_SIZE 4096

typedef struct {
    int user_id;
    char username[BUFFER_SIZE];
    char full_name[BUFFER_SIZE];
    int role;
} UserInfo;

#endif // USERINFO_H
