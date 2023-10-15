#define _GNU_SOURCE

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>

#include "morello.h"

/**
* Wrappable function type.
*/
typedef void *(cmpt_fun_t)(void* arg);

#define STACK_PERMS (PERM_GLOBAL | READ_CAP_PERMS | WRITE_CAP_PERMS)

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

/**
 * Compartment descriptor
 */
typedef struct {
    void *entry;    // sentry for entry into compartment
    void *exit;     // sentry for return from compartment
    void *stack;    // callee's stack
    void *target;   // target function (sentry)
} cmpt_desc_t;

static const cmpt_t *create_cmpt(cmpt_fun_t *target, unsigned stack_pages)
{
    size_t pgsz = getpagesize();
    size_t sz = pgsz * stack_pages;

    void *stack = mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    // Note: setting bounds is going to be redundant here
    // once kernel returns bounded capability.
    stack = cheri_bounds_set(stack, sz);
    stack = cheri_offset_set(stack, sz);
    stack = cheri_perms_and(stack, STACK_PERMS);

    void *data = mmap(NULL, pgsz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    cmpt_desc_t *desc = (cmpt_desc_t *)cheri_bounds_set_exact(data, sizeof(cmpt_desc_t));
    desc->entry = cmpt_entry;
    desc->exit = cmpt_return;
    desc->stack = stack;
    if (!cheri_is_sealed(target)) {
        target = cheri_sentry_create(target);
    }
    desc->target = target;

    // Return read-only LB-sealed pointer to cap pair:
    return morello_lb_sentry_create(cheri_perms_and(desc, PERM_GLOBAL | READ_CAP_PERMS));
}

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

/**
 * Compartment descriptor
 */
typedef struct {
    void *stack;
    void *target;
} cmpt_desc_t;

static const cmpt_t *create_cmpt(cmpt_fun_t *target, unsigned stack_pages)
{
    size_t pgsz = getpagesize();
    size_t sz = pgsz * stack_pages;

    void *stack = mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    // Note: setting bounds is going to be redundant here
    // once kernel returns bounded capability.
    stack = cheri_bounds_set(stack, sz);
    stack = cheri_offset_set(stack, sz);
    stack = cheri_perms_and(stack, STACK_PERMS);

    void *data = mmap(NULL, pgsz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    cmpt_desc_t *desc = (cmpt_desc_t *)cheri_bounds_set_exact(data, sizeof(cmpt_desc_t));
    // Compartment descriptor:
    desc->stack = stack;
    desc->target = target;
    // Capability pair:
    cmpt_t *cmpt = (cmpt_t *)cheri_bounds_set_exact(data + sizeof(cmpt_desc_t), sizeof(cmpt_t));
    if (!cheri_is_sealed(target)) {
        target = cheri_sentry_create(target);
    }
    cmpt->data[0] = cheri_perms_and(desc, PERM_GLOBAL | READ_CAP_PERMS); // data capability
    cmpt->data[1] = cmpt_switch; // code capability
    // Return read-only LPB-sealed pointer to cap pair:
    return morello_lpb_sentry_create(cheri_perms_and(cmpt, PERM_GLOBAL | READ_CAP_PERMS));
}

#endif

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

#ifdef USE_LB_SEALED_CAP
    printf("using LB-sealed capability\n");
#else
    printf("using LPB-sealed capability\n");
#endif

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
