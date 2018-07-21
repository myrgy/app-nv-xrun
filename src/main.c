#include <stdio.h>
#include <sys/io.h>
#include <unistd.h>

#define GMUX_PORT_SWITCH_DISPLAY 0x10
#define GMUX_PORT_SWITCH_DDC 0x28
#define GMUX_PORT_SWITCH_EXTERNAL 0x40
#define GMUX_PORT_DISCRETE_POWER 0x50
#define GMUX_PORT_VALUE 0xc2
#define GMUX_PORT_READ 0xd0
#define GMUX_PORT_WRITE 0xd4

#define GMUX_IOSTART 0x700

typedef unsigned char u8;

enum discrete_state { STATE_ON, STATE_OFF };
enum gpu_id { IGD, DIS };

static int gmux_index_wait_ready() {
  int i = 200;
  u8 gwr = inb(GMUX_IOSTART + GMUX_PORT_WRITE);

  while (i && (gwr & 0x01)) {
    inb(GMUX_IOSTART + GMUX_PORT_READ);
    gwr = inb(GMUX_IOSTART + GMUX_PORT_WRITE);
    usleep(100);
    i--;
  }

  return !!i;
}

static int gmux_index_wait_complete() {
  int i = 200;
  u8 gwr = inb(GMUX_IOSTART + GMUX_PORT_WRITE);

  while (i && !(gwr & 0x01)) {
    gwr = inb(GMUX_IOSTART + GMUX_PORT_WRITE);
    usleep(100);
    i--;
  }

  if (gwr & 0x01)
    inb(GMUX_IOSTART + GMUX_PORT_READ);

  return !!i;
}

static void index_write8(int port, u8 val) {
  gmux_index_wait_ready();
  outb(val, GMUX_IOSTART + GMUX_PORT_VALUE);
  gmux_index_wait_complete();
  outb((port & 0xff), GMUX_IOSTART + GMUX_PORT_WRITE);
}

static u8 index_read8(int port) {
  u8 val;
  gmux_index_wait_ready();
  outb((port & 0xff), GMUX_IOSTART + GMUX_PORT_READ);
  gmux_index_wait_complete();
  val = inb(GMUX_IOSTART + GMUX_PORT_VALUE);

  return val;
}

static void set_discrete_state(enum discrete_state state) {
  if (state == STATE_ON) { // switch on dGPU
    index_write8(GMUX_PORT_DISCRETE_POWER, 1);
    index_write8(GMUX_PORT_DISCRETE_POWER, 3);
  } else { // switch off dGPU
    index_write8(GMUX_PORT_DISCRETE_POWER, 1);
    index_write8(GMUX_PORT_DISCRETE_POWER, 0);
  }
}

static u8 get_discrete_state() { return index_read8(GMUX_PORT_DISCRETE_POWER); }

static void switchto(enum gpu_id id) {
  if (id == IGD) { // switch to iGPU
    index_write8(GMUX_PORT_SWITCH_DDC, 1);
    index_write8(GMUX_PORT_SWITCH_DISPLAY, 2);
    index_write8(GMUX_PORT_SWITCH_EXTERNAL, 2);
  } else { // switch to dGPU
    index_write8(GMUX_PORT_SWITCH_DDC, 2);
    index_write8(GMUX_PORT_SWITCH_DISPLAY, 3);
    index_write8(GMUX_PORT_SWITCH_EXTERNAL, 3);
  }
}



int main(int argc, char **argv) {
  if (iopl(3) < 0) {
    perror("No IO permissions");
    return 1;
  }

  // switchto(IGD);
  printf("Discrete state: 0x%X\n", get_discrete_state());
  set_discrete_state(STATE_OFF);
  printf("Discrete state: 0x%X\n", get_discrete_state());
  // set_discrete_state(STATE_ON);
  printf("Discrete state: 0x%X\n", get_discrete_state());
  // set_discrete_state(STATE_OFF);
  printf("Discrete state: 0x%X\n", get_discrete_state());

  return 0;
}
