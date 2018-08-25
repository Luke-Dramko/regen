regen is a command line tool.  It requires a single argument: a regular expression.  There are other optional arguments and flags.

`$ regen '[a-z]+'`

regen supports a second command line argument.  The regular expression will be written out to a file with the name of the second command line argument.

`$ regen '[a-z]+' out.txt`

## regen Flags
`-d` By default, regen uses the current system time as a seed for random number generation.  The `-d` flag turns this off.

`-f` must be followed by a string, which is used as a file name to write the output to.  If a file name is provided as a second argument, `-f` overrides.  If `-f` is used before the second argument, a second argument is invalid.

`-l` must be followed by an integer.  It specifies the number of example strings to be written out.

`-o` If the file specified by -f or the second argument already exists, overwrite it without asking.

`-p` the * and + operators use random numbers to determine how many times to reproduce the given segment.  Choose a number between 0 and .98.  A value closer to 1 will result in more characters for * and +.  The devault value is .90 and the maximum value is 0.98.

Flags can be in any order.  Gouping of flags (`-od`) is not supported.

Examples:<br>
`$ regen -d '[a-z]+' out.txt -o -l 10`

`$ regen '[A-Z]*[^A-Z]{2,6}' -l 35 -f examples.txt`

`$ regen -d -p .60 '(.{4})?[A-Z]|(na)'`

A summary of this information can be found with the command

`$ regen --help`

## Compatable Syntax
Regular expression syntax varies slightly by language.  `regen` interprets the following regular expression syntax:
(Note that the shown output to the examples may be one of many possibilities)

`[]` define a character class.  The shortcuts `a-z`, `A-Z`, and `0-9` are supported.  The only characters that need to be escaped in a character class are `\`, `[`, and `]`.<br>
_Examples_<br>
&nbsp;&nbsp;&nbsp;`[abc.&(]`<br>
&nbsp;&nbsp;&nbsp;`[a-z]`<br>
&nbsp;&nbsp;&nbsp;`[A-Z~1+]`<br>
&nbsp;&nbsp;&nbsp;`[a-zA-Z0-9]`<br>
A single character with no special syntactic meaning is interpreted as a character class.<br>
The escape character (`\`) turns a character with special syntactic meaning into a character class.  It can also be used on characters without special syntatic meaning, which has no effect (escapes for invisible characters may be implemented later).

The `^` character can be placed as the first character after `[` inside a character class to negate the character class.  That class will then inclulde all visible characters in the ascii character set except those listed after the `^`.<br>
_Examples_<br>
&nbsp;&nbsp;&nbsp;`[^.@$]` -> ;

`*` operates on a single character class (listed previously), and repeats it 0 or more times.<br>
_Example_<br>
&nbsp;&nbsp;&nbsp;`[a-z]*` -> abzzedg

`+` operates on a single character class and repeats it 1 or more times.<br>
_Example_<br>
&nbsp;&nbsp;&nbsp;`[0-9]+` -> 09983

`?` operates on a single character class and may or may not include it in the ouput (50/50 chance).<br>
_Example_<br>
&nbsp;&nbsp;&nbsp;`[abc]?` -> a

`{n}` operates on a single character class and repeats it exactly _n_ times.<br>
_Example_<br>
&nbsp;&nbsp;&nbsp;`a{5}` -> aaaaa _(In this case, this is the only possible result)_

`{n:m} or {n,m}` operate on a single character class and repeat it between n and m times, inclusive.<br>
_Example_<br>
&nbsp;&nbsp;&nbsp;`[a-z]{3:5}` -> adrq

`|` operates on two or more character classes and select one of them to include in the output string.<br>
_Example_<br>
&nbsp;&nbsp;&nbsp;`[a-z]|[A-Z]|[0-9]` -> G

`.` can be any of the visible characters in the ascii character set.<br>
_Example_<br>
&nbsp;&nbsp;&nbsp;`.` -> ~

`(` and `)` are used for grouping to make more complicated structures.<br>
_Example_<br>
&nbsp;&nbsp;&nbsp;`\*(bacon\*)+` -> \*bacon\*bacon\*bacon\*
