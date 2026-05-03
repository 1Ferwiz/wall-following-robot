#include "fsm.h"

int main(void)
{
    // Init all modules
    FSM_Init();

    while (1)
    {
        FSM_Update();  // runs every loop (later add timing)
    }

    return 0;
}