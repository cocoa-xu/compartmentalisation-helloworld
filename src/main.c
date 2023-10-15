#define _GNU_SOURCE

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>

/**
* Wrappable function type.
*/
typedef void *(cmpt_fun_t)(void* arg);

#ifdef USE_LB_SEALED_CAP
/**
* LB-sealed
* Compartment handle type (opaque).
*/
typedef struct {
    void *data;
} cmpt_t;

// See lb.S
// https://git.morello-project.org/morello/morello-examples/-/blob/main/src/compartments/src/lb.S
extern void *cmpt_call(const cmpt_t *cmpt, void *arg);
extern void cmpt_entry(void *arg);
extern void cmpt_return();

#else

/**
* LPB-sealed
* Compartment handle type (opaque).
*/
typedef struct {
    void *data[2];
} cmpt_t;

// See lpb.S
// https://git.morello-project.org/morello/morello-examples/-/blob/main/src/compartments/src/lpb.S
extern void *cmpt_call(const cmpt_t *cmpt, void *arg);
extern void *cmpt_switch(void *arg);

#endif

static const cmpt_t *create_cmpt(cmpt_fun_t *target, unsigned stack_pages);

static void *fun(void *buffer)
{
    printf("inside...\n");
    printf("csp: %s\n", cap_to_str(NULL, cheri_csp_get()));
    int *data = buffer;
    int x = data[0];
    int y = data[1];
    int z = x + y;
    data[2] = z;
    return data;
}

int main(int argc, char *argv[])
{
    const cmpt_t *cmpt = create_cmpt(fun, 4 /* pages */);
    int buffer[3] = {2, 3, 0};

    printf("before...\n");
    printf("csp: %s\n", cap_to_str(NULL, cheri_csp_get()));

    int *res = cmpt_call(cmpt, buffer);

    printf("after...\n");
    printf("csp: %s\n", cap_to_str(NULL, cheri_csp_get()));

    printf("result: %d + %d = %d\n", res[0], res[1], res[2]);
    return 0;
}
