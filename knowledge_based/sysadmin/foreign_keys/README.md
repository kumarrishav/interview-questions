# Foreign Keys

There are two files, `fileA` and `fileB`. `fileA` consists of a set of key-value pairs, one per line, where the key is the first field in that line, and the value is the second. For example, `fileA` could be:

```
AD22 15484
AB22 9485
AD23 10945
```

`fileB` is divided into sections. Each section starts with a line of the form `'SECTION_NAME':`. Then, the lines that follow have entries of the form:

```
PIN_NUMBER='(0,0,0,0,0,...,0,X,0,0,0,0,0,0,0...)';
```

Where `X` is a key that is in `fileA`.

Write a script that, given `fileA` and `fileB`, echoes `fileB` and appends a line of the form `PIN_DELAY='Y';` after each `PIN_NUMBER` line, where `Y` is the value in `fileA` for the key on that line of `fileB`.

# Example

If `fileA` contains:

```
AD22 15484
AB22 9485
AD23 10945
```

And `fileB` contains:

```
'DXN_0':
PIN_NUMBER='(AD22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)';
'DXP_0':
PIN_NUMBER='(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,AD23,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)';
'VREFN_0':
PIN_NUMBER='(AB22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)';
```

Then the output should be:

```
'DXN_0':
PIN_NUMBER='(AD22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)';
PIN_DELAY='15484';
'DXP_0':
PIN_NUMBER='(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,AD23,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)';
PIN_DELAY='10945';
'VREFN_0':
PIN_NUMBER='(AB22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)';
PIN_DELAY='9485';
```

# Solution

Both files `solution1.sh` and `solution2.sh` implement a one-liner shell command that solves the problem. It combines `perl` and `awk`. `solution1.sh` is a little off limits when it comes to readability, due to the excessive quote escapes; `solution2.sh` achieves the same thing in a more readable way.
