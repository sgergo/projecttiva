#include <stdint.h>
#include <stdbool.h>

#include "console.h"
#include "board.h"
#include "task.h"

int main()
{  
    board_init();

    while(1)
        task_process();     

    return 0;
}
