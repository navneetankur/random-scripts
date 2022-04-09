#include <linux/input.h>
#include <linux/uinput.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

using namespace std;

void emit(int fd, int type, int code, int val)
{
   struct input_event ie;

   ie.type = type;
   ie.code = code;
   ie.value = val;
   /* timestamp values below are ignored */
   ie.time.tv_sec = 0;
   ie.time.tv_usec = 0;

   write(fd, &ie, sizeof(ie));
}

int main() {

    int fd = open("/dev/input/by-id/usb-413c_Dell_KB216_Wired_Keyboard-event-kbd", O_RDWR | O_NONBLOCK);
	if(fd < 0) {
		cout << "error.";
		return 1;
	}
	emit(fd, EV_KEY, KEY_LEFTCTRL, 1);
	emit(fd, EV_KEY, KEY_RIGHTCTRL, 1);
	emit(fd, EV_KEY, KEY_LEFTCTRL, 0);
	emit(fd, EV_KEY, KEY_RIGHTCTRL, 0);
	emit(fd, EV_SYN, SYN_REPORT, 0);
	close(fd);
	return 0;
}

