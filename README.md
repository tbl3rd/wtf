# wtf

A tiny DSL in C to parse and validate complicated options
such as the device (`,`) stream (`:`) specifiers in `ffmpeg`.

Parse `arg` according to `.spec` and the `vc` validators at `vv`.
Return `1` if all the validators wrote their outs.
Otherwise complain as `av0` and return `0`.

`(spec+1)` is a string of fields in `arg` divided by `*spec`.

Example: The following code parses arg on '`-`'s into `f`, `n`, and `u`,
         describing them as `"filename"`, `"count"`, and `"uuid"` in
         error messages from `"wtf"` whenever there's a problem.

```
    char f[PATH_MAX]; long n; uuid_t u;
    const wtf_valid_t vs[] = {
        { &wtf_filename, f }, { &wtf_slongint, &n }, { &wtf_someuuid, u }
    };
    const wtf_validator_t validator = {
        .av0 = "wtf", .vc = sizeof vs / sizeof vs[0], .vv = vs,
        .spec = "-" "filename-count-uuid"
    };
    assert(wtf_validate(&validator, arg));
```

You may need a `uuid_t` type.  See `wtf_someuuid()` in `wtf.c`.
