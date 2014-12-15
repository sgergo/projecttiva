#include <stdint.h>
#include <stdbool.h>

#include "console.h"
#include "board.h"
#include "task.h"

int main()
{  
    board_init();
    console_printtext("\n\nTIVA template test\n\n");

    while(1)
    {    
        console_process();
        task_process();     
    }

    return 0;
}
