#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "config.h"

static void collect();
static void pbufs(int o);
static void cleanup();
static void die(const char *fn);

static int tf = -1;
static char buf[MEMMAX];
static char *bptr = buf;

void
collect()
{
	char c;

	memset(buf, 0, MEMMAX);

	while ((c = getchar()) != EOF) {
		*bptr++ = c;

		if ((bptr - buf) >= MEMMAX) {
			if (tf < 0) {
				tf = mkstemp(tempfn);

				if (tf < 0)
					die("mkstemp");
			}

			if (write(tf, buf, MEMMAX) < 0)
				die("write");

			/* reset memory buffer after dumping to file */
			memset(buf, 0, MEMMAX);
			bptr = buf;
		}
	}

	if (ferror(stdin))
		die("read");
}

void
pbufs(int o)
{
	/* check if temp file wasn't used */
	if (tf < 0) {
		if (write(o, buf, bptr - buf) < 0)
			die("fwrite");
	} else {
		ssize_t s;

		/* write buffer to file so buf can be used for reading the temp file */
		if (write(tf, buf, bptr - buf) < 0)
			die("write");

		if (lseek(tf, 0, SEEK_SET) < 0)
			die("lseek");

		while ((s = read(tf, buf, MEMMAX)) > 0)
			if (write(o, buf, s) < 0)
				die("fwrite");
	}
}

void
cleanup()
{
	if (tf >= 0) {
		close(tf);
		unlink(tempfn);
	}
}

void
die(const char *fn)
{
	fprintf(stderr, "collect: %s: %s\n", fn, strerror(errno));

	if (!strncmp(fn, "mkstemp", strlen("mkstemp")))
		cleanup();

	exit(errno);
}

int
main(int argc, char **argv)
{
	int o = 1;

	if (argc > 1) {
		if ((o = open(argv[1], O_WRONLY|O_CREAT)) < 0) {
			fprintf(stderr, "collect: %s: %s\n",
			             argv[1], strerror(errno));
			return errno;
		}
	}

	collect();
	pbufs(o);
	cleanup();

	return 0;
}
