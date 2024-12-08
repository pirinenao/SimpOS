#include "simpos.h"

extern int main(int agrc, char** argv);

/* start function which retrieves the process arguments, and calls the programs main function with those arguments */
void c_start()
{
    struct process_arguments arguments;
    simpos_process_get_arguments(&arguments);
    main(arguments.argc, arguments.argv);
}