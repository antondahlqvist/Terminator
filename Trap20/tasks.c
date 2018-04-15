/**
 * @file tasks.c
 *
 * Description
 *
 */

/*======= Includes ==========================================================*/

#include "tasks.h"
#include "cfifo.h"

/*======= Local Macro Definitions ===========================================*/
/*======= Type Definitions ==================================================*/
/*======= Local function prototypes =========================================*/
/*======= Local variable declarations =======================================*/

static cfifo_t *queue;

/*======= Global function implementations ===================================*/

void tasks_init(void)
{
  CFIFO_CREATE_STATIC(queue, sizeof(task_t), 5);
}

void add_task_to_queue(task_func_t func)
{
  task_t next_task;
  next_task.func = func;
  cfifo_put(queue, &next_task);
}
void run_tasks(void)
{
  struct task next_task;
  while (cfifo_get(queue, &next_task) == CFIFO_SUCCESS) {
    next_task.func();
  }

}

/*======= Local function implementations ====================================*/

