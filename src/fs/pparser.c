#include "pparser.h"
#include "../kernel.h"
#include "../string/string.h"
#include "../memory/heap/kernel_heap.h"
#include "../memory/memory.h"
#include "../status.h"

/* validates the given path */
static int pathparser_path_valid_format(const char *path)
{
    int len = strnlen(path, SIMPOS_MAX_PATH);

    /* returns 1 if the path is in correct format */
    return (len >= 0 && isdigit(path[0]) && memcmp((void *)&path[1], (void *)":/", 2) == 0);
}

/* returns the drive number from the path, and repositions the path variable */
static int pathparser_get_drive_number(const char **path)
{
    if (!pathparser_path_valid_format(*path))
    {
        return -BADPATH;
    }

    int drive_no = tonumericdigit(*path[0]);

    /* add 3 bytes to remove the drive number from the path */
    *path += 3;

    return drive_no;
}

/* creates the root of the path with the given drive_number */
static struct path_root *pathparser_create_root(int drive_number)
{
    struct path_root *path_r = kernel_zalloc(sizeof(struct path_root));
    path_r->drive_no = drive_number;
    path_r->first = 0;
    return path_r;
}

/* extracts parts of the path */
static const char *pathparser_get_path_part(const char **path)
{
    char *result_path_part = kernel_zalloc(SIMPOS_MAX_PATH);
    int i = 0;
    while (**path != '/' && **path != 0x00)
    {
        result_path_part[i] = **path;
        *path += 1;
        i++;
    }

    /* skip the forward slash from the path to avoid
     * problems next time this function is called
     */
    if (**path == '/')
    {
        *path += 1;
    }

    /* free the allocated memory */
    if (i == 0)
    {
        kernel_free(result_path_part);
        result_path_part = 0;
    }

    return result_path_part;
}

/* parse path part */
struct path_part *pathparser_parse_path_part(struct path_part *last_part, const char **path)
{
    const char *path_part_str = pathparser_get_path_part(path);

    /* if nothing to parse */
    if (!path_part_str)
    {
        return 0;
    }

    /* create path part, and allocate memory */
    struct path_part *part = kernel_zalloc(sizeof(struct path_part));
    part->part = path_part_str;
    part->next = 0x00;

    /* if there was last_part, change its next attribute to point to this part */
    if (last_part)
    {
        last_part->next = part;
    }

    return part;
}

/* free path parts recursively */
void pathparser_free(struct path_root *root)
{
    /* points to the first part */
    struct path_part *part = root->first;

    while (part)
    {
        /* saves the address of the next part */
        struct path_part *next_part = part->next;
        /* frees current part */
        kernel_free((void *)part->part);
        kernel_free(part);
        /* assigns next_part as a new target for the next loop  */
        part = next_part;
    }

    /* frees the path root */
    kernel_free(root);
}

/*  */
struct path_root *pathparser_parse(const char *path, const char *current_directory_path)
{
    int res = 0;
    const char *tmp_path = path;
    struct path_root *path_root = NULL;

    /* extracts the drive number, and removes it from the tmp_path */
    res = pathparser_get_drive_number(&tmp_path);
    if (res < 0)
    {
        return path_root;
    }

    /* set ups root for the path */
    path_root = pathparser_create_root(res);
    if (!path_root)
    {
        return path_root;
    }

    /* parses the first part of the path */
    struct path_part *first_part = pathparser_parse_path_part(NULL, &tmp_path);
    if (!first_part)
    {
        return path_root;
    }
    path_root->first = first_part;

    /* parses the next part of path. has to be parsed outside
     * the loop because we must send first part as an argument
     */
    struct path_part *part = pathparser_parse_path_part(first_part, &tmp_path);

    /* parse rest of the path */
    while (part)
    {
        part = pathparser_parse_path_part(part, &tmp_path);
    }

    return path_root;
}