/*
 * collect.c - collect contents of stdin before writing them out
 *
 * Copyright (C) 2022 Avalon Williams
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "config.h"

static void usage();
static void collect();
static void pbufs(int o);
static void cleanup();
static void die(const char *fn);

static int tf = -1;
static char buf[MEMMAX];
static char *bptr = buf;
static char *prog;

void
usage()
{
	fprintf(stderr, "usage: %s [-a] file\n", prog);
	exit(1);
}

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
	fprintf(stderr, "%s: %s: %s\n", prog, fn, strerror(errno));

	if (!strncmp(fn, "mkstemp", strlen("mkstemp")))
		cleanup();

	exit(errno);
}

int
main(int argc, char **argv)
{
	int f = O_WRONLY|O_CREAT;
	int o = 1;
	int opt;

	prog = argv[0];

	while ((opt = getopt(argc, argv, ":a")) != -1) {
		switch(opt) {
			case 'a':
				f |= O_APPEND;
				break;
			case '?':
				usage();
				break;
		}
	}

	/* only accept one argument for output */
	if (optind + 1 < argc)
		usage();

	if (optind < argc) {
		if ((o = open(argv[optind], f, 0666)) < 0) {
			fprintf(stderr, "%s: %s: %s\n",
			             prog, argv[optind], strerror(errno));
			return errno;
		}
	}

	collect();
	pbufs(o);
	cleanup();

	return 0;
}
