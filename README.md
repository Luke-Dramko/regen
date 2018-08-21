# About regen
regen (Regular Expression GENerator) generates a string that a regular expression could match. 

Regular expressions are tricky to visualize and debug - they're often "write once, revise never" due to their covluted syntax.  This program, a command line utility, tries to ease this problem by creating sample strings that the regular expression could match. For example, if the input regex is 

`([a-z]+\.)*[a-z]+@([a-z]+\.)+(com)|(gov)|(edu)|(org)`

some sample outputs might be

```
d.if.b@y.edu
s.w@f.gov
t.pjjjgdcg.vdw@j.v.ohcaf.com
rpdq.rep.auf.msg.t.e@dnu.edu
a@k.y.rwwlxx.ox.edu
```

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
The escape character (`\`) turns a character with special syntactic meaning  into a character class.  It can also be used on characters without special syntatic meaning, which has no effect (escapes for invisible characters may be implemented later).

The `^` character can be placed as the first character after `[` inside a character class to negate the character class.  That class will then inclulde all visible characters in the ascii character set except those listed after the `^`.<br>
_Examples_<br>
&nbsp;&nbsp;&nbsp;`[^.@$]` -> ;

`*` operates on a single character class (listed previously), and repeats it 0 or more times.<br>
_Example_<br>
&nbsp;&nbsp;&nbsp;`[a-z]*` -> abzzedg

`+` operates on a single character class and repeats it 1 or more times.<br>
_Example_<br>
&nbsp;&nbsp;&nbsp;`[0-9]+` -> 09983

`?` operates on a signle character class and may or may not include it in the ouput (50/50 chance).<br>
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
