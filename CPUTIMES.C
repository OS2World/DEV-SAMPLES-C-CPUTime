/* time.c -- produce a 1-page summary of cpu times of various C constructs
 * To port to a new machine, change the following:
 *    BASEN:    Start around 10000, increase by *10 until no question marks
 *    Macro processor: define ANSIIMAC 1 for Ansii C, undef for old C
 *    WARNRANGE: print ? after CPU time if max clicks - min clicks >
 *           WARNRANGE * mean clicks
 *    Times: typical values of CLOCKS_PER_SEC: Vax=60  Cray=105296000
 *    For large values, change %5d in macro loop1, 99999 in minclicks;
 *  10 Oct 1990  Bentley, Kernighan, Van Wyk
 *  13 Jan 1991  ANSI C edit by Grosse;  avoid common ungetc bug in scanf;
 *    clean up junk file;
 *
 *  16 Apr 1994  adpated to OS/2 using both IBM and Microsoft compilers;
 *               replaced close() calls with fclose(), added stdlib.h to
 *               the include list - J. M. Hughes, jmh@noao.edu
 */

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>             /* jmh */

#define BASEN 1000000
#define WARNRANGE 0.4
#define quoted(TEXT) #TEXT

struct stacknode {
    int val;
    struct stacknode *next;
};
typedef struct stacknode *Stackp;
Stackp    stackroot;

void push(i)
int i;
{
    Stackp p;

    p = (Stackp) malloc(sizeof(struct stacknode));
    p->val = i;
    p->next = stackroot;
    stackroot = p;
}

int pop()
{
    int    i;

    i = stackroot->val;
    stackroot = stackroot->next;
    return i;
}

/* Main macros for experiments */


#define loop1(CODE) loop1ctr++; \
    for (i = 0; i < n; i++) { CODE; } \
    loop1next = clock(); \
    thisclicks = loop1next - loop1start; \
    sumclicks += thisclicks; \
    if (thisclicks < minclicks) minclicks = thisclicks; \
    if (thisclicks > maxclicks) maxclicks = thisclicks; \
    printf("%5d", (loop1next - loop1start)/1000); \
    loop1start = loop1next;

#define loop(CODE) printf("  %-30s", quoted(CODE)); \
    minclicks = 99999999; maxclicks = -1; sumclicks = 0; \
    loop1ctr = 0; \
    loop1start = clock(); \
    loop1(CODE) \
    loop1(CODE) \
    i0 = i1 + i2 + i3; \
    loop1(CODE) \
    i0 = i1 + i2 + i3 - i1 - i2 - i3; \
    loop1(CODE) \
    i0 = i1 + i2 + i3 + i1*i2 + i2*i3 + i1*i3; \
    loop1(CODE) \
    queststr = ""; \
    if (loop1ctr * (float)(maxclicks - minclicks) > WARNRANGE *  sumclicks) \
        queststr = "?"; \
    lastmics = sumclicks * 1000000.0 / ((float)(CLOCKS_PER_SEC) * n * loop1ctr); \
    printf("%10.2f%s\n", lastmics - basemics, queststr);

#define title(TEXT) printf("%s (n=%d)\n", TEXT, n);

/* The experiment */

int sum1(a) int a; { return a; }
int sum2(a, b) int a, b; { return a + b; }
int sum3(a, b, c) int a, b, c; { return a + b + c; }

main()
{
    int    loop1start, loop1next, loop1ctr;
    float    lastmics, basemics;
    int    minclicks, maxclicks, sumclicks, thisclicks;
    int    i, n, basen;
    int    i0, i1, i2, i3, i4;
    float    f0, f1, f2, f3;
    int    *v;
    char    *queststr;
    char    s[100];
    char    fname[20];
    FILE    *fp;
    /* The following strings are variables because some macro processors
      don't handle quoted strings in quoted arguments.
      They are not just pointers to string literals because some
      scanf's write on their input. */
    char    s0123456789[20];
    char    sa123456789[20];
    char    s12345[20];
    char    s123_45[20];
    char    sd[20];
    char    sdn[20];
    char    sf[20];
    char    sf62[20];
    strcpy(s0123456789,"0123456789");
    strcpy(sa123456789,"a123456789");
    strcpy(s12345,"12345");
    strcpy(s123_45,"123.45");
    strcpy(sd,"%d");
    strcpy(sdn,"%d\n");
    strcpy(sf,"%f");
    strcpy(sf62,"%f6.2");

    setbuf(stdout, (char *) 0);    /* No buffering to watch output */
    printf("  Operation                         Clicks for each trial ");
    printf("   Mics/N\n");

    basen = BASEN;
    n = basen;
    title("Null Loop")
    i0 = i1 = i2 = i3 = 5;
    f0 = f1 = f2 = f3 = 5.0;
    basemics = 0.0;
    loop({})
    basemics = lastmics;

    title("Int Operations");
    i1 = i2 = i3 = 5;
    loop(i1++)
    loop(i1 = i2)
    loop(i1 = i2 + i3)
    loop(i1 = i2 - i3)
    loop(i1 = i2 * i3)
    loop(i1 = i2 / i3)
    loop(i1 = i2 % i3)

    title("Float Operations");
    f1 = f2 = f3 = 5.0;
    loop(f1 = f2)
    loop(f1 = f2 + f3)
    loop(f1 = f2 - f3)
    loop(f1 = f2 * f3)
    loop(f1 = f2 / f3)

    title("Numeric Conversions");
    f1 = 123456.789;
    i1 = 123456;
    loop(i1 = f1)
    loop(f1 = i1)

    title("Integer Vector Operations");
    v = (int *) malloc(n * sizeof(int));
    for (i = 0; i < n; i++)
        v[i] = 0;
    loop(v[i] = i)
    loop(v[v[i]] = i)
    loop(v[v[v[i]]] = i)
    free(v);

    title("Control Structures");
    i1 = i2 = i3 = 5;
    loop(if (i == 5) i1++)
    loop(if (i != 5) i1++)
    loop(while (i < 0) i1++)
    loop(i1 = sum1(i2))
    loop(i1 = sum2(i2, i3))
    loop(i1 = sum3(i2, i3, i4))

    n = basen/100;
    strcpy(fname, "junk");
    title("Input/Output");
    strcpy(s, "1234\n");
    fp = fopen(fname, "w");
    loop(fputs(s, fp))
    fclose(fp);                             /* jmh */ 
    fp = fopen(fname, "r");
    loop(fgets(s, 9, fp))
    fclose(fp);                             /* jmh */ 
    fp = fopen(fname, "w");
    loop(fprintf(fp, sdn, i))
    fclose(fp);                             /* jmh */ 
    fp = fopen(fname, "r");
    loop(fscanf(fp, sd, &i1))
    fclose(fp);                             /* jmh */ 
    unlink(fname);

    n = basen/100;
    title("Malloc");
    loop(free(malloc(8)))
    loop(push(i))
    loop(i1 = pop())

    n = basen/10;
    title("String Functions");
    loop(strcpy(s, s0123456789))
    loop(i1 = strcmp(s, s))
    loop(i1 = strcmp(s, sa123456789))

    n = basen/100;
    title("String/Number Conversions");
    loop(i1 = atoi(s12345))
    loop(sscanf(s12345, sd, &i1))
    loop(sprintf(s, sd, i))
    loop(f1 = atof(s123_45))
    loop(sscanf(s123_45, sf, &f1))
    loop(sprintf(s, sf62, 123.45))

    n = basen/100;
    title("Math Functions");
    loop(i1 = rand())
    f2 = 5.0;
    loop(f1 = log(f2))
    loop(f1 = exp(f2))
    loop(f1 = sin(f2))
    loop(f1 = sqrt(f2))
}
