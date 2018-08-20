# About regen
regen (REgular expression GENerator) generates a string that a regular expression could match. 

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

