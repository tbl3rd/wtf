#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wtf.h"


int wtf_filename(const char *av0, void *out, const char *in)
{
    const int length = strlen(in);
    if (length > 0 && length < PATH_MAX) {
        strcpy((char *)out, in);
        return 1;
    }
    fprintf(stderr, "%s: not a good file: %s\n", av0, in);
    return 0;
}
int wtf_slongint(const char *av0, void *out, const char *in)
{
    char *end = 0;
    const long n = strtol(in, &end, 0);
    if (end != in && end == in + strlen(in)) {
        *((long *)out) = n;
        return 1;
    }
    fprintf(stderr, "%s: not a good int : %s\n", av0, in);
    return 0;
}
int wtf_sinteger(const char *av0, void *out, const char *in)
{
    long longint;
    if (wtf_slongint(av0, &longint, in)) {
        const int justint = (int)longint;
        if (justint == longint) {
            *((int *)out) = justint;
            return 1;
        }
    }
    return 0;
}
int wtf_someuuid(const char *av0, void *out, const char *in)
{
    static const char nil[] = "00000000-0000-0000-0000-000000000000";
    if (strlen(in) == sizeof nil - 1) {
        strcpy((char *)out, in);
        return 1;
    }
    fprintf(stderr, "%s: not a good UUID: %s\n", av0, in);
    return 0;
}


// Return a count of the occurances of separator in s.
//
static int wtf_count_separators(char separator, const char *s)
{
    int result = 0;
    const char *p = s;
    for (; *p; ++p) result += separator == *p;
    result += p != s;
    return result;
}

// Return p with arg parsed into an arg vector at separator characters.
// The separator divides arg into p.ac strings at p.av[], where p.av[i]
// points into p.heap for all i.
//
// Call wtf_parse_delete(&p) eventually.
//
typedef struct wtf_parse_t {
    int ac;
    char **av;
    char *heap;
} wtf_parse_t;
static wtf_parse_t wtf_parse_new(char separator, const char *arg)
{
    wtf_parse_t result = { .heap = strdup(arg) };
    assert(result.heap);
    result.ac = wtf_count_separators(separator, result.heap);
    result.av = calloc(1 + result.ac, sizeof *result.av);
    char *h = result.heap;
    int ac = 0;
    for (ac = 0; ac < result.ac; ++ac) {
        result.av[ac] = h;
        while (*h && separator != *h) ++h;
        *h++ = *"";
    }
    return result;
}
static void wtf_parse_delete(const wtf_parse_t *p)
{
    free(p->av);
    free(p->heap);
}


int wtf_validate(const wtf_validator_t *d, const char *arg)
{
    assert(d->vc == wtf_count_separators(*d->spec, d->spec + 1));
    const wtf_parse_t p = wtf_parse_new(*d->spec, arg);
    int result = p.ac == d->vc;
    if (result) {
        for (int i = 0; i < d->vc; ++i) {
            const wtf_valid_t v = d->vv[i];
            result = ((*v.f)(d->av0, v.out, p.av[i])) && result;
        }
    } else {
        fprintf(stderr, "%s: '%s' has %d fields instead of %d\n",
                d->av0, arg, p.ac, d->vc);
    }
    wtf_parse_delete(&p);
    if (!result) {
        fprintf(stderr, "%s: '%s' does not match '%s'\n",
                d->av0, arg, d->spec + 1);
    }
    return result;
}
