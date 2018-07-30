1. Handle (let list-of-bindings body-1 body-2 ... body-n): the expressions body-1, body-2, ..., body-n are evaluated in order, and the value of the last one is returned.
2. Handle (if test consequent) variant that is missing alternate.
3. Correct +-* return type.
4. load
5. list.scm
6. math.scm
7. The ' shorthand for quote
8. +-*/ handles any number of arguments
9. cond handles clauses with zero or multiple expressions
10. letrec support multiple body expression
11. <= handles any number of arguments
12. Informative error message
13. Interface

Passed Knuth's test, officially a man-interpreter

Known Problem:
Interface works on Windows but not on Linux, has some weird symbol mixed in
lcm does not handle negative number
Probably there are some weird edge cases we didn't consider
