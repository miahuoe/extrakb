#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <string.h>

#include "keynames.h"
#include <linux/input.h>

typedef struct kstate {
	struct timeval time;
	double h;
	char state; /* 1 = Pressed, 2 = Hold, 0 = Released */
} kstate;

double timeval_diff(struct timeval *a, struct timeval *b)
{
	double D;
	struct timeval d;
	d.tv_sec = b->tv_sec - a->tv_sec;
	d.tv_usec = b->tv_usec - a->tv_usec;
	if (d.tv_usec < 0) {
		d.tv_sec--;
		d.tv_usec += 1000000;
	}
	D = (double)d.tv_sec;
	D += (((double)d.tv_usec)/1000000.0);
	return D;
}

char* basename(char *S)
{
	char *s = S;
	while (*s) s++;
	while (s != S && *s != '/') s--;
	s++;
	return s;
}

void usage(char *argv0)
{
	fprintf(stderr, "Usage: %s [options]...\n", basename(argv0));
	fprintf(stderr,
	"Options:\n"
	"    -i <event file>\n"
	"    (none)\n"
	);
}

int main(int argc, char *argv[])
{
	char *argv0, **next_arg, *arg;
	int ofd = 1, kbe = -1;
	ssize_t r;
	struct input_event E;
	char *loptn[2], *loptv;
	kstate S[KEY_MAX];

	argv0 = argv[0];
	argv++;
	while (*argv && **argv == '-') {
		next_arg = argv+1;
		++*argv;
		if (**argv == '-') {
			++*argv;
			loptn[0] = *argv;
			while (**argv != '=') ++*argv;
			loptn[1] = *argv;
			++*argv;
			loptv = *argv;

			arg = loptv;

			/* long option */
			switch (loptn[1]-loptn[0]) {
			case 5: if (!memcmp("input", loptn[0], 5)) goto input_file;
			default: break;
			}
		}
		else do {
			switch (**argv) {
			case 0:
				break;
			case '-':
				break;
			case 'i':
				arg = *next_arg;
				next_arg++;
				input_file:

				kbe = open(arg, O_RDONLY);
				if (kbe == -1) {
					dprintf(2, "error: '%s': %s\n",
						arg, strerror(errno));
					return 1;
				}
				break;
			case 'h':
				usage(argv0);
				return 0;
			default:
				break;
			}
			++*argv;
		} while (**argv);
		argv = next_arg;
	}

	memset(S, 0, sizeof(S));
	while (0 < (r = read(kbe, &E, sizeof(E)))) {
		if (E.type != EV_KEY) continue;
		S[E.code].state = E.value;
		switch (E.value) {
		case 0:
			S[E.code].h = timeval_diff(&S[E.code].time, &E.time);
			dprintf(ofd, "R %s %.2f\n", keynames[E.code], S[E.code].h);
			break;
		case 1:
			memcpy(&S[E.code].time, &E.time, sizeof(E.time));
			dprintf(ofd, "P %s\n", keynames[E.code]);
			break;
		case 2:
			dprintf(ofd, "H %s\n", keynames[E.code]);
			break;
		default:
			break;
		}
	}
	if (r) {
		dprintf(2, "%s\n", strerror(errno));
	}

	close(kbe);
	close(ofd);
	return 0;
}
