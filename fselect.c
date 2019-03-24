#include <sys/types.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/select.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

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
	fprintf(stderr, "Usage: %s files...\n", argv0);
	fprintf(stderr,
	"  Monitor given files for input.\n"
	"  Write to stdout the path of the first file that is ready for reading.\n"
	"  Sleeps for 1 second to avoid catching user's own input.\n"
	);
}

#define NUM_FD 1024

int main(int argc, char *argv[])
{
	fd_set fds;
	int nfds = 0, r, i, e;
	int F[NUM_FD] = { 0 };
	char *FN[NUM_FD] = { 0 };

	if (argc == 1) {
		usage(basename(*argv));
		return 0;
	}
	close(0);
	++argv;
	argc--;
	sleep(1);
	/* Avoid catching user's input
	 * (like enter when running the command by hand)
	 */
	FD_ZERO(&fds);
	while (*argv) {
		if (-1 == (F[nfds] = open(*argv, O_RDONLY | O_NONBLOCK))) {
			e = errno;
			fprintf(stderr, "open(\"%s\"): %s\n", *argv, strerror(e));
			F[nfds] = 0;
		}
		else {
			FN[nfds] = *argv;
			FD_SET(F[nfds], &fds);
			nfds++;
		}
		++argv;
	}
	if (!nfds) {
		return 1;
	}
	r = select(nfds+1, &fds, 0, 0, 0);
	if (r == -1) {
		e = errno;
		fprintf(stderr, "select: %s\n", strerror(e));
		return 1;
	}
	if (!r) {
		return 1;
	}
	i = 0;
	while (i < nfds && !FD_ISSET(F[i], &fds)) i++;
	printf("%s\n", FN[i]);
	for (i = 0; i < nfds; i++) {
		close(F[i]);
	}
	FD_ZERO(&fds);
	return 0;
}
