#ifndef __H_DIRENT
#define __H_DIRENT 1

#define MAXNAMLEN 128

struct dirent{
    char d_name[MAXNAMLEN+1];
    unsigned int d_fileno, nr0, nr1;
};

typedef struct DIR_s{
    unsigned int current_entry;     // d_fileno
    unsigned int pid;               // driver pid
    unsigned int box;               // driver box
    struct DIR_s * next;
    struct dirent dirent;
} DIR;

DIR * opendir(const char * dirname);
void closedir(DIR * dir);

struct dirent* readdir(DIR * dir);

#endif