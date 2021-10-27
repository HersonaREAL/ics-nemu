#include <stdio.h>
#include <fixedptc.h>

#define TEST1(a,b,sym,func) \
        do { \
            char buf[4096]; \
            fixedpt g = fixedpt_rconst((a)); \
            fixedpt k = fixedpt_rconst((b));\
            fixedpt_str(g,buf,-2) ; printf("%s",buf);\
            printf(" "#sym" "); \
            fixedpt_str(k,buf,-2) ; printf("%s",buf);\
            printf(" = "); \
            fixedpt_str(func(g,k),buf,-2); \
            printf("%s\n",buf);\
        }while(0);

#define TEST2(a,sym,func) \
        do { \
            char buf[4096]; \
            fixedpt g = fixedpt_rconst((a)); \
            printf(""#sym"("); \
            fixedpt_str(g,buf,-2) ; printf("%s",buf);\
            printf(") = "); \
            fixedpt_str(func(g),buf,-2); \
            printf("%s\n",buf);\
        }while(0);

#define TEST3(a,b,sym,func) \
        do { \
            char buf[4096]; \
            fixedpt g = fixedpt_rconst((a)); \
            int k = b;\
            fixedpt_str(g,buf,-2) ; printf("%s",buf);\
            printf(" "#sym" "); \
            printf("%d",k);\
            printf(" = "); \
            fixedpt_str(func(g,k),buf,-2); \
            printf("%s\n",buf);\
        }while(0);
    
void multest() {
    printf("\033[32m==== multest ====\033[0m\n");
    TEST1(1.2, 1.2, *, fixedpt_mul)
    TEST1(1.2, -1.2, *, fixedpt_mul)
    TEST1(2, 4, *, fixedpt_mul)
    TEST1(-0.1, 11, *, fixedpt_mul)
    TEST1(12.12, -1, *, fixedpt_mul)
    TEST1(0.5, 22, *, fixedpt_mul)
    TEST1(77, 1.2, *, fixedpt_mul)

    printf("\033[31m--mul int--\033[0m\n");
    TEST3(22.2, 2, *, fixedpt_muli)
    TEST3(0.5, 3, *, fixedpt_muli)
    TEST3(-0.4, 11, *, fixedpt_muli)
    TEST3(22.2, 100, *, fixedpt_muli)
    printf("\n");
}

void divtest() {
    printf("\033[32m==== divtest ====\033[0m\n");
    TEST1(6.6, 3.3, /, fixedpt_div)
    TEST1(1.44, 1.2, /, fixedpt_div)
    TEST1(1.44, -1.2, /, fixedpt_div)
    TEST1(777.7, 6.6, /, fixedpt_div)
    TEST1(-9.9, -3.3, /, fixedpt_div)
    TEST1(-5, 2, /, fixedpt_div)
    TEST1(5, 2, /, fixedpt_div)

    printf("\033[31m--div int--\033[0m\n");
    TEST3(22.2, 2, /, fixedpt_divi)
    TEST3(22.2, -2, /, fixedpt_divi)
    TEST3(22.2, -1, /, fixedpt_divi)
    TEST3(22.2, 3, /, fixedpt_divi)
    TEST3(22.2, -10, /, fixedpt_divi)
    TEST3(0.42, 2, /, fixedpt_divi)
    TEST3(0.42, -7, /, fixedpt_divi)
    printf("\n");
}

void floortest() {
    printf("\033[32m==== floortest ====\033[0m\n");
    TEST2(1.2, floor, fixedpt_floor)
    TEST2(0.2, floor, fixedpt_floor)
    TEST2(10.2, floor, fixedpt_floor)
    TEST2(0.0, floor, fixedpt_floor)
    TEST2(0, floor, fixedpt_floor)
    TEST2(-0.0, floor, fixedpt_floor)
    TEST2(-0.1, floor, fixedpt_floor)
    TEST2(-1.1, floor, fixedpt_floor)
    TEST2(-10.1, floor, fixedpt_floor)
    TEST2(-1.0, floor, fixedpt_floor)
    TEST2(-2.0, floor, fixedpt_floor)
    TEST2(10.0, floor, fixedpt_floor)
    printf("\n");
}

void ceiltest() {
    printf("\033[32m==== ceiltest ====\033[0m\n");
    TEST2(0.2, ceil, fixedpt_ceil)
    TEST2(1.2, ceil, fixedpt_ceil)
    TEST2(2.2, ceil, fixedpt_ceil)
    TEST2(2.5, ceil, fixedpt_ceil)
    TEST2(100.1, ceil, fixedpt_ceil)
    TEST2(0, ceil, fixedpt_ceil)
    TEST2(0.0, ceil, fixedpt_ceil)
    TEST2(-0.0, ceil, fixedpt_ceil)
    TEST2(-0.2, ceil, fixedpt_ceil)
    TEST2(-1.2, ceil, fixedpt_ceil)
    TEST2(-2.2, ceil, fixedpt_ceil)
    TEST2(-2.5, ceil, fixedpt_ceil)
    TEST2(1.0, ceil, fixedpt_ceil)
    TEST2(1, ceil, fixedpt_ceil)
    TEST2(-1, ceil, fixedpt_ceil)
    TEST2(2, ceil, fixedpt_ceil)
    TEST2(-2.0, ceil, fixedpt_ceil)
    printf("\n");
}

void abstest() {
    printf("\033[32m==== abstest ====\033[0m\n");
    TEST2(-2.0, abs, fixedpt_abs)
    TEST2(2.0, abs, fixedpt_abs)
    TEST2(-3, abs, fixedpt_abs)
    TEST2(-3, abs, fixedpt_abs)
    TEST2(0, abs, fixedpt_abs)
    TEST2(0.0, abs, fixedpt_abs)
    TEST2(-0.0, abs, fixedpt_abs)
    TEST2(1.111, abs, fixedpt_abs)
    TEST2(-1.111, abs, fixedpt_abs)
    TEST2(0.01, abs, fixedpt_abs)
    TEST2(-0.01, abs, fixedpt_abs)
    printf("\n");
}

int main() {
    multest();
    divtest();
    floortest();
    ceiltest();
    abstest();
}