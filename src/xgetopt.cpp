#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;
#include "xgetopt.h"

char *xoptarg; // global argument pointer
int xoptind = 0; // global argv index

int xgetopt(int argc, char *argv[], const char *optstring) {
	static char *next = NULL;
	if (xoptind == 0)
		next = NULL;
	xoptarg = NULL;
	if (next == NULL || *next == '\0') {
		if (xoptind == 0)
			xoptind++;
		if (xoptind >= argc || argv[xoptind][0] != '-'
				|| argv[xoptind][1] == '\0') {
			xoptarg = NULL;
			if (xoptind < argc)
				xoptarg = argv[xoptind];
			return EOF;
		}
		if (strcmp(argv[xoptind], "--") == 0) {
			xoptind++;
			xoptarg = NULL;
			if (xoptind < argc)
				xoptarg = argv[xoptind];
			return EOF;
		}
		next = argv[xoptind];
		next++; // skip past -
		xoptind++;
	}
	char c = *next++;
	char *cp = strchr((char*)optstring, c);
	if (cp == NULL || c == ':')
		return '?';
	cp++;
	if (*cp == ':') {
		if (*next != '\0') {
			xoptarg = next;
			next = NULL;
		} else if (xoptind < argc) {
			xoptarg = argv[xoptind];
			xoptind++;
		} else {
			return '?';
		}
	}
	return c;
}
