#include "command.h"
#include "initrd.h"
#include "mailbox.h"
#include "terminal.h"
#include "uart.h"

struct command commands[] = {{
                                 .name = "help",
                                 .help = "Show help message!\n",
                                 .func = help,
                             },
                             {
                                 .name = "lshw",
                                 .help = "Show some HW informations\n",
                                 .func = lshw,
                             },
                             {
                                 .name = "hello",
                                 .help = "Print \'hello world\'\n",
                                 .func = hello,
                             },
                             {
                                 .name = "reboot",
                                 .help = "Reboot the device.\n",
                                 .func = reboot,
                             },
                             {
                                 .name = "ls",
                                 .help = "List file name in FS.\n",
                                 .func = ls,
                             },
                             {
                                 .name = "cat",
                                 .help = "Show the content of target file.\n",
                                 .func = cat,
                             },

                             // ALWAYS The last item of the array!!!
                             {
                                 .name = "NULL", // The end of the array
                             }};

int help() {
  int i = 0;
  while (1) {
    if (!strcmp(commands[i].name, "NULL")) {
      break;
    }
    uart_puts(commands[i].name);
    uart_puts(": ");
    uart_puts(commands[i].help);
    i++;
  }
  return 0;
}

int cat() {
  char buf[256] = {0};
  char *tmp = buf;
  uart_puts("Name:\n");
  for (int i = 0; i < 255; i++) {
    *tmp = uart_getc();
    uart_putc(*tmp);
    if (*tmp == 127) {
      *tmp = 0;
      tmp--;
      *tmp = 0;
      tmp--;
      uart_send('\b');
      uart_send(' ');
      uart_send('\b');
    }
    if (*tmp == '\n') {
      *(tmp) = '\0';
      break;
    }
    tmp++;
  }
  initrd_cat(buf);
  return 0;
}

int ls() {
  initrd_list();
  return 0;
}

int hello() {
  uart_puts("Hello World!\n");
  return 0;
}

int invalid_command(const char *s) {
  uart_putc('`');
  uart_puts(s);
  uart_putc('`');
  uart_puts(" is invalid command! Please use `help` to list commands\n");
  return 0;
}

int lshw(void) {
  uart_puts("Board version\t: ");
  mbox[0] = 7 * 4;
  mbox[1] = MAILBOX_REQ;
  mbox[2] = TAG_BOARD_VER;
  mbox[3] = 4;
  mbox[4] = 0;
  mbox[5] = 0;
  mbox[6] = TAG_LAST;

  if (mailbox_config(CHANNEL_PT)) {
    uart_puth(mbox[5]);
  }
  uart_puts("\nDevice base Mem Addr\t: ");
  mbox[0] = 8 * 4;
  mbox[1] = MAILBOX_REQ;
  mbox[2] = TAG_ARM_MEM;
  mbox[3] = 8;
  mbox[4] = 0;
  mbox[5] = 0;
  mbox[6] = 0;
  mbox[7] = TAG_LAST;
  if (mailbox_config(CHANNEL_PT)) {
    uart_puth(mbox[5]);
    uart_puts("\nDevice Mem size\t: ");
    uart_puth(mbox[6]);
  }
  uart_putc('\n');
  return 0;
}

int reboot() {
  *PM_RSTC = PM_PASSWORD | 0x20; // Reset
  *PM_WDOG = PM_PASSWORD | 180;
  return 0;
}
