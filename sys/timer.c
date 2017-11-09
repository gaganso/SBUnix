#include <sys/interrupts.h>
#include <sys/term.h>
#include <sys/timer.h>

volatile s_time g_clock;

void
timer_isr()
{
    // Hits 2425 times a second. Each hit ~2.5 ms
    g_clock.counter++;
    g_clock.milliseconds = (uint32_t)(g_clock.counter * 2) / 5;
    if (g_clock.counter == 2425) {
        g_clock.seconds++;
        g_clock.milliseconds = 0;
        g_clock.counter = 0;
        // Print every second
        term_set_time(g_clock.seconds);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

void
timer_setup()
{
    __asm__("mov $0x34, %al;"
            "out %al, $0x43;" // load control register
            "mov $492, %eax;" // 1193182 = 41*2*14551 ~= 492 * 2425 //
            "out %al, $0x40;"
            "mov %ah, %al;"
            "out %al, $0x40;");
}

void
sleep(uint32_t milliseconds)
{
    s_time initial_time = g_clock;
    s_time current_time = g_clock;
    uint64_t elapsed_time_ms = 0;
    bool wait = FALSE;
    do {
        current_time = g_clock;
        elapsed_time_ms =
          (current_time.seconds - initial_time.seconds) * 1000 +
          (current_time.milliseconds - initial_time.milliseconds);
        wait = (elapsed_time_ms < milliseconds);
    } while (wait);
}