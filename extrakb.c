/*
MIT License

Copyright (c) 2019 Michał Czarnecki

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _DEFAULT_SOURCE
	#define _DEFAULT_SOURCE
#endif

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "keynames.h"
#include <linux/input.h>

typedef struct kstate {
	struct timeval time;
	double h;
	char state; /* 1 = Pressed, 2 = Held, 0 = Released */
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

void err(const char *fmt, ...)
{
	va_list a;

	va_start(a, fmt);
	vdprintf(2, fmt, a);
	va_end(a);
	exit(EXIT_FAILURE);
}

char* basename(char *S)
{
	char *s = S;
	while (*s) s++;
	while (S != s && *s != '/') s--;
	if (*s == '/') s++;
	return s;
}

void usage(char *argv0)
{
	fprintf(stderr, "Usage: %s [options]...\n", basename(argv0));
	fprintf(stderr,
	"Options:\n"
	"    -i <event file> Read input from <event file>.\n"
	"    -h              Display this help message.\n"
	);
}

#define NO_ARG do { ++*argv; if (!(mid = **argv)) argv++; } while (0)

char *ARG(char ***argv)
{
	char *r = 0;

	(**argv)++;
	if (***argv) { /* -oARG */
		r = **argv;
		(*argv)++;
	}
	else { /* -o ARG */
		(*argv)++;
		if (**argv && ***argv != '-') {
			r = **argv;
			(*argv)++;
		}
	}
	return r;
}

char* EARG(char ***argv)
{
	char *a;
	a = ARG(argv);
	if (!a) {
		err("ERROR: Expected argument.\n");
	}
	return a;
}

int main(int argc, char *argv[])
{
	char *argv0;
	int ofd = 1, kbe = -1;
	_Bool mid = 0;
	ssize_t r;
	struct input_event E;
	kstate S[KEY_MAX];
	(void)argc;

	argv0 = *argv;
	++argv;
	while ((mid && **argv) || (*argv && **argv == '-')) {
		if (!mid) ++*argv;
		mid = 0;
		if ((*argv)[0] == '-' && (*argv)[1] == 0) {
			argv++;
			break;
		}
		switch (**argv) {
		case 'i':
			kbe = open(EARG(&argv), O_RDONLY);
			if (kbe == -1) {
				err("open: %s\n", strerror(errno));
				return 1;
			}
			break;
		case 'h':
			usage(argv0);
			NO_ARG;
			return 0;
		default:
			usage(argv0);
			return 1;
		}
	}

	memset(S, 0, sizeof(S));
	while (-1 != (r = read(kbe, &E, sizeof(E)))) {
		if (E.type != EV_KEY) continue;
		S[E.code].state = E.value;
		switch (E.value) {
		case 0:
			S[E.code].h = timeval_diff(&S[E.code].time, &E.time);
			dprintf(ofd, "R %x %s %.3f\n",
				E.code, keynames[E.code], S[E.code].h);
			break;
		case 1:
			memcpy(&S[E.code].time, &E.time, sizeof(E.time));
			dprintf(ofd, "P %x %s\n", E.code, keynames[E.code]);
			break;
		case 2:
			dprintf(ofd, "H %x %s\n", E.code, keynames[E.code]);
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
