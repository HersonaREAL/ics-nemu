#include <stdint.h>
#include <stdio.h>
#include <NDL.h>

int main() {
    uint32_t begin, end;
    NDL_Init(0);
    begin = NDL_GetTicks();
    while(1) {
        end = NDL_GetTicks();
        if (end - begin >= 500) {
            printf("Your life wasted 0.5 seconds,haha!\n");
            begin = end;
        }
    }
    NDL_Quit();
    return 0;
}