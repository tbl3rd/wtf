#ifndef INCLUDE_WTF_H
#define INCLUDE_WTF_H


// Parse and validate compound command-line options in the style of
// FFMPEG's stream specifiers.


// Return 0 and complain to stderr, or copy in to out and return 1.
// For wtf_filename(): char out[PATH_MAX];
// For wtf_slongint(): long *out;
// For wtf_sinteger(): int *out;
// For wtf_someuuid(): uuid_t out;
//
extern int wtf_filename(const char *av0, void *out, const char *in);
extern int wtf_slongint(const char *av0, void *out, const char *in);
extern int wtf_sinteger(const char *av0, void *out, const char *in);
extern int wtf_someuuid(const char *av0, void *out, const char *in);


// For v, return 1 if (*v.f)(av0, v.out, in), and copy in to v.out.
// Otherwise complain as av0 to stderr and return 0.
//
typedef struct wtf_valid_t {
    int (*f)(const char *av0, void *out, const char *in);
    void *out;
} wtf_valid_t;


// Parse arg according to spec and the vc validators at vv.
// Return 1 if all the validators wrote their outs.
// Otherwise complain to av0 and return 0.
//
// (spec+1) is a string of fields in arg divided by *spec.
//
// Example: The following code parses arg on '-'s into f, n, and u,
//          describing them as "filename", "count", and "uuid" in
//          error messages from "wtf" whenever there's a problem.
//
// char f[PATH_MAX]; long n; uuid_t u;
// const wtf_valid_t vs[] = {
//     { &wtf_filename, f }, { &wtf_slongint, &n }, { &wtf_someuuid, u }
// };
// const wtf_validator_t validator = {
//     .av0 = "wtf", .vc = sizeof vs / sizeof vs[0], .vv = vs,
//     .spec = "-" "filename-count-uuid"
// };
// assert(wtf_validate(&validator, arg));
//
typedef struct wtf_validator_t {
    const char *const av0;
    const int vc;
    const wtf_valid_t *vv;
    const char *const spec;
} wtf_validator_t;
extern int wtf_validate(const wtf_validator_t *d, const char *arg);


#endif // #ifndef INCLUDE_WTF_H
