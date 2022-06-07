import os
from subprocess import check_output
import pytest
from pathlib import Path
import re
import itertools
import contextlib

this_script_dir = Path(__file__).parent.resolve()

input_files = set(Path(this_script_dir).rglob("*.c")) - set(
    Path(this_script_dir).rglob("*immortal.c")
)


def idfn(val: Path):
    return str(os.path.relpath(str(val)))


transformed_file_regex = re.compile('## "(.*)"')

# Adapted from from https://stackoverflow.com/a/5434936
def pairwise(iterable):
    "s -> (s0, s1), (s1, s2), (s2, s3), ..."
    a, b = itertools.tee(iterable)
    next(b, None)
    return itertools.zip_longest(a, b)


# Copied from https://stackoverflow.com/a/42441759
@contextlib.contextmanager
def working_directory(path):
    """Changes working directory and returns to previous on exit."""
    prev_cwd = Path.cwd()
    os.chdir(path)
    try:
        yield
    finally:
        os.chdir(prev_cwd)


@pytest.mark.parametrize(argnames="source_file_input", argvalues=input_files, ids=idfn)
def test_tool(source_file_input, snapshot):
    # We assume initial_cwd to be the build directory
    initial_cwd = Path.cwd().absolute()

    source_file_dir = Path(source_file_input).absolute().parent

    # Walk up from the source file dir until the directory of this script and
    # gather all the extra args in the extra_args files found along the way
    extra_args_list = []
    extra_args_dir = source_file_dir
    while this_script_dir.parent != extra_args_dir:
        try:
            extra_args = (
                Path(extra_args_dir / "extra_args")
                .read_text()
                .replace("\r\n", "\n")
                .split("\n")
            )
            extra_args = list(filter(None, extra_args))
            extra_args = [
                arg.replace("${current_dir}", str(extra_args_dir))
                for arg in extra_args
            ]
            extra_args_list.extend(extra_args)
            print(extra_args_list)
        except FileNotFoundError:
            pass
        extra_args_dir = extra_args_dir.parent

    extra_args_map = {"": []}
    for extra_arg_line in extra_args_list:
        split = extra_arg_line.split(":", 1)
        config = ""
        extra_arg = ""
        if len(split) == 2:
            config = split[0]
            extra_arg = split[1]
        elif len(split) == 1:
            extra_arg = split[0]
        else:
            assert 0, "Impossible"

        if config not in extra_args_map:
            extra_args_map[config] = []
        extra_args_map[config].append(extra_arg)

    print(extra_args_map)
    for config in extra_args_map:
        infix = "immortal"
        extra_args = extra_args_map[""]
        if config != "":
            infix = f"{config}.immortal"
            extra_args.extend(extra_args_map[config])

        out = ""
        with working_directory(source_file_dir):
            command = [
                initial_cwd / "immortalc",
                source_file_input,
                # Use initial_cwd as build path
                "-p",
                initial_cwd,
                "--stdout",
                "--infix",
                infix,
                "--force-output",
            ] + extra_args
            print(command)
            # Call the immortalc from the directory where the source file is.
            out = check_output(command)

        # Parse output of immortalc
        file_content_map = {}
        iter = transformed_file_regex.finditer(out.decode("utf-8"))
        for (match, next_match) in pairwise(iter):
            content_start = match.span()[1] + 1
            content_end = next_match.span()[0] if next_match is not None else len(out)
            content = out[content_start:content_end]
            file_content_map[match.group(1)] = content

        for file_path, content in file_content_map.items():
            path = Path(file_path)
            # Assert snapshot
            snapshot.snapshot_dir = path.parent.absolute()
            snapshot.assert_match(content, path.name)
