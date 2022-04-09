#include <linux/input.h>
#include <linux/uinput.h>

#include <fcntl.h>
#include <string.h>

#include <stdio.h>
#include <unistd.h>

#define EV_PRESSED 1
#define EV_RELEASED 0
#define EV_REPEAT 2



int main() {

	int fd = 0;
	char *device = "/dev/input/by-id/usb-413c_Dell_KB216_Wired_Keyboard-event-kbd";
	struct input_event event;

	memset(&event, 0, sizeof(event));

	/* gettimeofday(&event.time, NULL); */
	if( (fd = open(device,  O_RDWR |  O_NONBLOCK )) < 0 )
	{
		printf("not opened "); // Read or Write to device
		return 0;
	}

	// Press the key down
	event.type = EV_KEY;
	event.value = EV_PRESSED;
	event.code = KEY_LEFTCTRL;
	write(fd, &event, sizeof(struct input_event));
	event.type = EV_KEY;
	event.value = EV_PRESSED;
	event.code = KEY_RIGHTCTRL;
	write(fd, &event, sizeof(struct input_event));

	// Release the key
	event.value = EV_RELEASED;
	event.code = KEY_LEFTCTRL;
	write(fd, &event, sizeof(struct input_event));
	event.value = EV_RELEASED;
	event.code = KEY_RIGHTCTRL;
	write(fd, &event, sizeof(struct input_event));

	//syn signal
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(fd, &event, sizeof(struct input_event));
	close(fd);

	return 0;
}
