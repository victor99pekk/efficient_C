/* Test program for EDAG01 Efficient C at Lund University
 * 
 * See intopt.h, add your implementation in intopt.c, and type make.
 *
 * The input in test will be run until there is a timeout or a failed test.
 *
 */

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

#include "error.h"
#include "intopt.h"

#define N			(20)		/* max m and n. */

char			cwd[BUFSIZ];		/* dir where we started in main. */
uint64_t		pass[N+1];		/* pass[i] = passed cases for m = n = i. */
bool			fail;			/* stopped due to failed case. */
char*			progname;		/* name of this program. */
unsigned		test_time = 120;	/* default 120 s. */
static const double	eps = 1e-6;		/* epsilon. */
static int		verbose;		/* print verbose output. */

static void make_score();

static void timeout(int s)
{
	make_score();
}

static void cd(char* dir)
{
	if (chdir(dir) < 0)
		error("cd to \"%s\" failed", dir);
}

static void check(void)
{
	int		r;
	int		m;
	int		n;
	int		i;
	int		j;
	int		k;
	int		s;
	double**	a;
	double*		b;
	double*		c;
	double*		x;
	double		z;
	double		y;
	double		z_sol;
	FILE*		in;
	FILE*		sol;
	
	in = fopen("i", "r");
	if (in == NULL)
		return;

	r = fscanf(in, "%d", &m);
	r = fscanf(in, "%d", &n);

	a = calloc(m+n, sizeof(double*));
	b = calloc(m+n, sizeof(double));
	c = calloc(n+1, sizeof(double));
	x = calloc(n+m+1, sizeof(double));

	for (i = 0; i < n; i += 1)
		r = fscanf(in, "%lf", &c[i]);

	for (i = 0; i < m; i += 1) {
		a[i] = calloc(n+1, sizeof(double));
		for (j = 0; j < n; j += 1)
			r = fscanf(in, "%lf", &a[i][j]);
	}

	for (; i < n+m; i += 1)
		a[i] = calloc(n+1, sizeof(double));

	for (i = 0; i < m; i += 1)
		r = fscanf(in, "%lf", &b[i]);
	
	fclose(in);

	z = intopt(m, n, a, b, c, x);
	
	sol = fopen("intopt.sol", "r");

	if (sol == NULL)
		goto dealloc;

	s = fscanf(sol, "z = %lf", &z_sol);

	fclose(sol);

	if (s != 1)
		goto dealloc;

	if (verbose >= 1) 
		printf("z = %lf       z_sol = %lf\n", z, z_sol);

	if ((isfinite(z) == 0) ^ (isfinite(z_sol) == 0)) {
		fail = 1;
		make_score();
	} else if ((isnan(z) != 0) & (isnan(z_sol) != 0))
		pass[n] += 1;
	else if (fabs(z-z_sol) <= eps)
		pass[n] += 1;
	else {
		fail = 1;
		make_score();
	}
	
dealloc:
	for (i = 0; i < n+m; i += 1)
		free(a[i]);
	free(a);
	free(b);
	free(c);
	free(x);
}

static void search(void)
{
	DIR*		dir;
	struct dirent*	entry;
	char		wd[BUFSIZ];

	dir = opendir(".");

	if (dir == NULL)
		error("cannot open .");

	while ((entry = readdir(dir)) != NULL) {

		if (getcwd(wd, sizeof wd) == NULL)
			error("getcwd failed");
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		else if (strcmp(entry->d_name, "i") == 0)
			check();
		else if (isdigit(entry->d_name[0])) {
			cd(entry->d_name);
			search();
		}
	}
	cd("..");

	closedir(dir);
}

static void eval(int n)
{
	char		dir[10];

	sprintf(dir, "%d", n);
	
	cd(dir);
	search();
}

static void make_score()
{
	uint64_t	i;
	uint64_t	d;
	uint64_t	s;
	FILE*		fp;
	char		dir[BUFSIZ];
	char		cmd[BUFSIZ];
	char*		p;

	if (fail) {
		getcwd(dir, sizeof dir);
		p = dir;
		printf("failed in %s\n", dir);
		if (strcmp(dir, cwd) != 0) {
			snprintf(cmd, sizeof cmd, "cp %s/i %s/intopt.sol %s", p, p, cwd);

			if (system(cmd) == 0) {
				printf("copied input file \"i\" and \"intopt.sol\" to %s\n",  cwd);
				printf("to test them, run a.out -i\n");
			}
		}
	}

	s = 0;

	printf("\n--- make score -------------------------\n");

	for (i = 2; i <= N; i += 1) {
		d = pass[i];
		s += d;
		printf("n = %2" PRIu64 ", pass = %8" PRIu64 " value = %8" PRIu64 "\n", i, pass[i], d);
	}

	printf("score = %" PRIu64 " for t = %u s\n", s, test_time);

	cd(cwd);

	fp = fopen("score", "w");
	if (fp == NULL)
		error("cannot open score for writing");
	fprintf(fp, "%" PRIu64 "\n", s);
	fclose(fp);

	exit(0);
}

static void usage()
{
	fprintf(stderr, "%s: usage: %s [-t seconds]\n", progname, progname);
	exit(1);
}

int main(int argc, char** argv)
{
	int		i;

	progname	= argv[0];

	signal(SIGALRM, timeout);

	if (getcwd(cwd, sizeof cwd) == NULL)
		error("getcwd failed");

	for (i = 1; i < argc; i += 1) {
		switch (argv[i][1]) {
		case 'i':
			printf("will test only \"i\"\n");
			verbose += 1;
			check();
			return 0;

		case 't':
			if (argv[i+1] == NULL || sscanf(argv[i+1],  "%u", &test_time) != 1)
				usage();
			i += 1;
			break;

		case 'v':
			verbose += 1;
			break;

		default:
			usage();
		}
	}
					
	alarm(test_time);

	cd("test");

	for (i = 2; i <= N; i += 1)
		eval(i);

	make_score();

	return 0;
}
