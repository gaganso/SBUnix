#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/syscall.h>
#include <sys/task.h>
#include <sys/term.h>
extern void syscall_isr_return(long);

long
syscall_exec(char* bin_name, char** argv, char** envp)
{
    task_exec_ring3(bin_name, argv, envp);
    return 0;
}

long
syscall_read(uint64_t fd, char* buf, uint64_t count)
{
    long ret = -1;
    if (fd == STDIN) {
        ret = term_read_from_buffer(buf, count);
    } else {
        // TODO add vfs read here
    }

    return ret;
}

long
syscall_write(uint64_t fd, char* buff, uint64_t count)
{
    uint8_t row, column, color, temp;
    long ret = -1;
    if (fd == STDERR || fd == STDOUT) {
        if (STDERR == fd) {
            term_get_cursor(&row, &column, &color);
            term_set_cursor(row, column, term_color_red);
        }
        ret = term_write(buff, count);
        if (STDERR == fd) {
            term_get_cursor(&row, &column, &temp);
            term_set_cursor(row, column, color);
        }
    }
    return ret;
}
long
syscall_wrapper(long syscall_num, long arg1, long arg2, long arg3)
{
    long ret_val = -1;
    switch (syscall_num) {
        case _SYS__open:
        /* For each case implement syscall, update ret_val */
        case _SYS__write:
            ret_val =
              syscall_write((uint64_t)arg1, (char*)arg2, (uint64_t)arg3);
            break;
        case _SYS__read:
            ret_val = syscall_read((uint64_t)arg1, (char*)arg2, (uint64_t)arg3);
            break;
        case _SYS__execve:
            ret_val = syscall_exec((char*)arg1, (char**)arg2, (char**)arg3);
            break;
        case _SYS__close:
        case _SYS__fork:
        case _SYS__wait4:
        case _SYS__exit:
        case _SYS__chdir:
        case _SYS__acces:
        case _SYS__pipe:
        case _SYS__dup:
        case _SYS__brk:
        default:
            kprintf("Call num %d, args %d, %d, %d, ret_val %d", syscall_num,
                    arg1, arg2, arg3, ret_val);
    }
    return ret_val;
}