# Snapshot tests for Immortalc

When testing each source file, the test script looks also for a `extra_args` file in the same directory, in the parent directory, and so on until this `test/` folder is reached. If found, the arguments specified in the file are passed to Immortalc. Arguments should be line-separated.

It is also possible to specify "configuration specific" extra arguments using the syntax `"<configuration-name>:<extra arg>"`.

For example the following `extra_args` file

```txt
-H
.
optmz-exec-time:--optimize=execution-time
```
means that `-H .` args will be passed in all cases, while `--optimize=execution-time` will only be passed when testing the `optmz-exec-time` configuration. For each configuration, snapshot files with be generated with the name of the configuration as infix. So e.g. given `main.c` as input test file the snapshots `main.immortal.c` and `main.optmz-exec-time.immortal.c` will be generated. `main.immortal.c` will be the result of an `immortalc` invocation passing `-H .` as extra arguments, while `main.optmz-exec-time.immortal.c` will be the result of an `immortalc` invocation passing `-H . --optimize=execution-time`.

Finally, in `extra_args` it is possible to write `${current_dir}` to refer to the directory in which the `extra_args` file is located.

`immortalc` is always invoked having as cwd the directory of the source file and having the directory in which `pytest` is invoked as "build path" (see `-p` flag of `immortalc`), starting from where the `compile_commands.json` will be searched.

