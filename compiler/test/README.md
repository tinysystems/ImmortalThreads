# Snapshot tests for Immortalc

When testing each source file, the test script looks also for a `extra_args` file in the same directory. If found, the arguments specified in the file are passed to Immortalc. Arguments should be line-separated.
Immortalc is always invoked having as cwd the directory of the source file.
