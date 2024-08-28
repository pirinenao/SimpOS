#ifndef PATHPARSER_H
#define PATHPARSER_H

/* structure for the root of the path */
struct path_root
{
    int drive_no;
    struct path_part *first;
};

/* structure for parts of the path such as: 1:/this/or/this/ */
struct path_part
{
    const char *part;
    struct path_part *next;
};

/* function prototypes */
struct path_root *pathparser_parse(const char *path, const char *current_directory_path);
void pathparser_free(struct path_root *root);

#endif