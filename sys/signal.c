#include <sys/defs.h>
#include <sys/signal.h>
#include <sys/task.h>
#include <sys/tasklist.h>
int
signal_kill(pid_t pid, int signal)
{
    task_struct* parent;
    task_struct* task = tasklist_get_task(pid, task_any_state);
    if (task) {
        parent = tasklist_get_task(task->ppid, task_sleep_wait);
        if (parent) {
            parent->state = task_runnable;
        }
        task_destroy(task);
        return 0;
    }
    return -1;
}
