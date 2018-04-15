#ifndef _tasks_H_
#define _tasks_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file tasks.h.h
 *
 * Description
 *
 */

/*======= Includes ==========================================================*/

#include <stdint.h>

/*======= Public macro definitions ==========================================*/
/*======= Type Definitions and declarations =================================*/
/*======= Public variable declarations ======================================*/
/*======= Public function declarations ======================================*/

typedef void (*task_func_t)(void);

typedef struct task {
  task_func_t func;
} task_t;

void tasks_init(void);
void add_task_to_queue(task_func_t func);
void run_tasks(void);

#ifdef __cplusplus
}
#endif

#endif /* _tasks_H_ */
