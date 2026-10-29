#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define BAR_WIDTH   40
#define CYCLE_SECS  60

int main(void)
{
    char bar[BAR_WIDTH + 1];
    uint32_t step = 0;

    /* Clear the screen once, then draw the static title. */
    printk("\033[2J");
    printk("\033[1;1H=== Hello from Zephyr ===\033[K");

    while (1) {
        uint32_t filled = (step * BAR_WIDTH) / CYCLE_SECS;
        uint32_t pct    = (step * 100) / CYCLE_SECS;

        for (uint32_t i = 0; i < BAR_WIDTH; i++) {
            bar[i] = (i < filled) ? '#' : '-';
        }
        bar[BAR_WIDTH] = '\0';

        /* Overwrite the same two lines each second. */
        printk("\033[2;1HUptime   : %u s\033[K", step);
        printk("\033[3;1HProgress : [%s] %3u%%\033[K", bar, pct);

        if (step >= CYCLE_SECS) {
            break;
        }

        k_sleep(K_SECONDS(1));
        step++;
    }

    /* Bar is full: show the finished state and stop updating. */
    printk("\033[4;1H\033[32mDone.\033[0m\033[K\n");

    while (1) {
        k_sleep(K_FOREVER);
    }
    return 0;
}
