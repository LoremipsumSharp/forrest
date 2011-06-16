#!/usr/bin/env python
# -*- coding: utf-8 -*-

# CLRS Problem 15-2 Printing neatly

# author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/

import sys
import re

INFTY = float('inf')

paragraph = (
    """Python is a high-level, structured, open-source programming language that can be used for a wide variety of programming tasks. It is good for simple quick-and-dirty scripts, as well as complex and intricate applications. It is an interpreted programming language that is automatically compiled into bytecode before execution (the bytecode is then normally saved to disk, just as automatically, so that compilation need not happen again until and unless the source gets changed). It is also a dynamically typed language that includes (but does not require one to use) object oriented features and constructs. The most unusual aspect of Python is that whitespace is significant; instead of block delimiters (braces in the C family of languages), indentation is used to indicate where blocks begin and end.""",
    """Consider the problem of neatly printing a paragraph on a printer. The input text is a sequence of n words of lengths l1, l2, ..., ln, measured in characters. We want to print this paragraph neatly on a number of lines that hold a maximum of M characters each. Our criterion of "neatness" is as follows. If a given line contains words i through j, where i ≤ j, and we leave exactly one space between words, the number of extra space characters at the end of the line is M-j+i-∑lk, which must be nonnegative so that the words fit on the line. We wish to minimize the sum, over all lines except the last, of the cubes of the numbers of extra space characters at the ends of lines. Give a dynamic-programming algorithm to print a paragraph of n words neatly on a printer. Analyze the running time and space requirements of your algorithm.""",
    """Consider the problem of neatly printing a paragraph on a printer. Analyze the running time and space requirements of your algorithm.""")

p_control = ("""Python is a high-level, structured, open-source
programming language that can be used for a
wide variety of programming tasks. It is good
for simple quick-and-dirty scripts, as well
as complex and intricate applications. It is
an interpreted programming language that is
automatically compiled into bytecode before
execution (the bytecode is then normally
saved to disk, just as automatically, so that
compilation need not happen again until and
unless the source gets changed). It is also a
dynamically typed language that includes (but
does not require one to use) object oriented
features and constructs. The most unusual aspect
of Python is that whitespace is significant;
instead of block delimiters (braces in the C
family of languages), indentation is used to
indicate where blocks begin and end.""",
"""Consider the problem of neatly printing a
paragraph on a printer. The input text is a
sequence of n words of lengths l1, l2, ..., ln,
measured in characters. We want to print this
paragraph neatly on a number of lines that hold
a maximum of M characters each. Our criterion
of "neatness" is as follows. If a given line
contains words i through j, where i ≤ j, and we
leave exactly one space between words, the number
of extra space characters at the end of the line
is M-j+i-∑lk, which must be nonnegative so that
the words fit on the line. We wish to minimize
the sum, over all lines except the last, of the
cubes of the numbers of extra space characters
at the ends of lines. Give a dynamic-programming
algorithm to print a paragraph of n words neatly
on a printer. Analyze the running time and space
requirements of your algorithm.""",
"""Consider the problem of neatly printing a paragraph
on a printer. Analyze the running time and space
requirements of your algorithm.""")


colors = {'DEFAULT'   : "",
          'BLACK'     : "\033[0;30m",
          'RED'       : "\033[0;31m",
          'GREEN'     : "\033[0;32m",
          'YELLOW'    : "\033[0;33m",
          'BLUE'      : "\033[0;34m",
          'MAGENTA'   : "\033[0;35m",
          'CYAN'      : "\033[0;36m",
          'WHITE'     : "\033[0;37m",
          'GRAY'      : "\033[0;37m",
          'BLACK_B'   : "\033[0;1;30m",
          'RED_B'     : "\033[0;1;31m",
          'GREEN_B'   : "\033[0;1;32m",
          'YELLOW_B'  : "\033[0;1;33m",
          'BLUE_B'    : "\033[0;1;34m",
          'MAGENTA_B' : "\033[0;1;35m",
          'CYAN_B'    : "\033[0;1;36m",
          'WHITE_B'   : "\033[0;1;37m",
          'NOCOLOR'   : "\033[0m"}
# def cprint(*args, **kwargs):
#     for c in kwargs.keys():
#         sys.stdout.write(colors[c])
#         for x in kwargs[c]:
#             sys.stdout.write(str(x))
#         sys.stdout.write(colors['NOCOLOR'])

def cprint(*args, **kwargs):
    for p in args:
        sys.stdout.write(colors[p[0]])
        for x in p[1:]:
            sys.stdout.write(str(x))
        sys.stdout.write(colors['NOCOLOR'])

def space_sum(p, M):
    ssum = 0
    lineno = 0
    lines = p.split('\n')
    for l in lines:
        lineno += 1
        spc_cnt = M - len(l)
        cprint(('WHITE', str(lineno).rjust(3), ' '), ('GREEN', l))
        if l is lines[-1]:
            print
        else:
            cprint(('YELLOW', '~'*spc_cnt, '[', str(spc_cnt).rjust(2), ']\n'))
            ssum += spc_cnt**3
    return ssum

def build_metadata(p):
    rec = re.compile(r'(\S*) (.*)')
    w = []
    l = []
    n = 0
    done = False
    while not done:
        m = rec.match(p)
        if m:
            s,p = m.groups()
        else:
            assert re.match(r'\S*', p)
            s = p
            done = True
        #print '%s__' % s
        w.append(s)
        l.append(len(s))
        n += 1

    return w,l,n

def stupid_neat(p, M):
    w,l,n = build_metadata(p)
    #cprint(BLUE=(n,) , DEFAULT=(' words: ', l))

    print '-'*M
    width = M
    formatted = ''
    for i in range(n):
        width += 1 + l[i]       # a space and a word
        if width > M:
            width = l[i]
            formatted += '\n'+w[i]
        else:
            formatted += ' '+w[i]

    formatted = formatted[formatted.find('\n')+1:]
    print formatted
    cprint(('RED', 'space sum: ', space_sum(formatted, M), '\n'))


# M - j + i -   ∑   l_k
#             i<=k<=j
def extras(l, i, j):
    s = 0
    for x in l[i:j+1]:
        s += x
    return M - j + i - s

#           / ∞                if extras[i,j]<0 (i.e., words i..j don't fit)
# lc[i,j] = | 0                 if j==n and extras[i,j]>=0 (last line costs 0)
#           \ (extras[i,j])**3  otherwise
def lc(l, n, i, j):
    ex = extras(l, i, j)
    if ex < 0:
        return INFTY
    elif j == n - 1:
        return 0
    else:
        return ex**3

#        /   0                         if j==0
# c[j] = |
#        \   min  (c[i-1] + lc[i,j])   if j>0
#          1<=i<=j
#
# To understand the solution, it helps to write down the simple cases:
#     j:0  ''
#    
#     j:1  foo
#    
#     j:2  min <1> foo bar
#              <2> foo
#                  bar
#    
#     j:3  min <1> foo bar baz
#              <2> foo
#                  bar baz
#              <3> min <1> foo bar
#                          baz
#                      <2> foo
#                          bar
#                          baz
#    
#     j:4  min <1> foo bar baz blah
#              <2> foo 
#                  bar baz blah
#              <3> min <1> foo bar
#                          baz blah
#                      <2> foo
#                          bar
#                          baz blah
#              <4> min <1> foo bar baz
#                          blah
#                      <2> foo
#                          bar baz
#                          blah
#                      <3> min <1> foo bar
#                                  baz
#                                  blah
#                              <2> foo
#                                  bar
#                                  baz
#                                  blah
#
#     j:5  ...
#
# It's necessary to study a real sample. Say we have this paragraph:
#     Consider the problem of neatly printing a
#     paragraph on a printer. The input text is a
#     sequence of n words of lengths l1, l2, ..., ln,
#     measured in characters. We want to print this
#     paragraph neatly on a number of lines that hold a
#     maximum of M characters each. Our criterion of
#     "neatness" is as follows. If a given line contains
#     words i through j, where i ≤ j, and we leave
#     exactly one space between words, the number of
#     extra space characters at the end of the line is
#     M-j+i-∑lk, which must be nonnegative so that the
#     words fit on the line. We wish to minimize the
#     sum, over all lines except the last, of the cubes
#     of the numbers of extra space characters at the
#     ends of lines. Give a dynamic-programming
#     algorithm
# and we're about to append the following stuff:
#     to print a paragraph of n words neatly
#     on a printer. Analyze the running time and space
#     requirements of your algorithm.
#
# M = 50
# j = index of the first word in the yet-to-append text (which is ``to'')
#
# The key point is to realize that we can group the possibilities of the
# reformatted text (although there are a great number of them):
#     group 1:
#         C[j-1]                                             <- words 1..j-1
#     group 2:
#         C[j-2]                                             <- words 1..j-2
#         algorithm                                          <- last line
#     group 3:
#         C[j-3]                                             <- words 1..j-3
#         dynamic-programming algorithm                      <- last line
#     group 4:
#         C[j-4]                                             <- words 1..j-4
#         a dynamic-programming algorithm                    <- last line
#     group 5:
#         C[j-5]                                             <- words 1..j-5
#         Give a dynamic-programming algorithm               <- last line
#     group 6:
#         C[j-6]                                             <- words 1..j-6
#         lines. Give a dynamic-programming algorithm        <- last line
#     group 7:
#         C[j-7]                                             <- words 1..j-7
#         of lines. Give a dynamic-programming algorithm     <- last line
#
#     There's no C[j-8], because if we put one more word, the line will exceed
#     the limit 50: "ends of lines. Give a dynamic-programming algorithm"
#
#     Then a new word ``to'' is appended:
#     case 1:
#         C[j-1]                                             <- words 1..j-1
#         to                                                 <- last line
#     case 2:
#         C[j-2]                                             <- words 1..j-2
#         algorithm to                                       <- last line
#     case 3:
#         C[j-3]                                             <- words 1..j-3
#         dynamic-programming algorithm to                   <- last line
#     case 4:
#         C[j-4]                                             <- words 1..j-4
#         a dynamic-programming algorithm to                 <- last line
#     case 5:
#         C[j-5]                                             <- words 1..j-5
#         Give a dynamic-programming algorithm to            <- last line
#     case 6:
#         C[j-6]                                             <- words 1..j-6
#         lines. Give a dynamic-programming algorithm to     <- last line
#     case 7:
#         C[j-7]                                             <- words 1..j-7
#         of lines. Give a dynamic-programming algorithm to  <- last line
#
#     These are the only cases we need to consider.
#
#     Let's denote the cost of C[j] as c[j], then:
#                / c[j-1] + lc[j,  j]
#                | c[j-2] + lc[j-1,j]
#                | c[j-3] + lc[j-2,j]
#     c[j] = min | c[j-4] + lc[j-3,j]
#                | c[j-5] + lc[j-4,j]
#                | c[j-6] + lc[j-5,j]
#                \ c[j-7] + lc[j-6,j]
#
#     We don't care what c[j-1],...,c[j-7] are. They have been calculated
#     and, of course, stored into some table.
#
#     We need not bother worrying about c[j-1],...,c[j-7] because they must
#     be optimal, or, in other words, we must have chosen the optimal one
#     from the numerous possibilities in group X (where 1<=X<=7).
#
#     In the Instructor's Manual, c[j] is defined as
#            /   0                         if j==0
#     c[j] = |
#            \   min  (c[i-1] + lc[i,j])   if j>0
#              1<=i<=j
#     this is smart, since lc[0],...,lc[j-7] are all ∞, according to the
#     definition of lc[i,j].
#    
def neat(p, M):
    w,l,n = build_metadata(p)
    # print w, l, n, M
    j = 0
    c = []
    d = []
    for j in range(n+1):
        if j == 0:
            c.append(0)
            d.append(0)
        else:
            min_c = INFTY
            # Attention: c[0] is reserved, so we cannot use c[0] as the cost of
            #            words[0] any more! Instead, cost(words[0]) is stored
            #            in c[1], ..., cost(words[j]) is stored in c[j+1].
            #            Generally speaking, words[0..n-1] => c[1..n]
            # Therefore, always remember c[j] is the cost of words[j-1]:
            # c[j] =   min  (c[i-1] + lc[i-1,j-1])
            #        1<=i<=j
            for i in range(j,0,-1):
                x = c[i-1] + lc(l, n, i-1, j-1)
                if x == INFTY:
                    break
                if x < min_c:
                    min_c = x
                    if len(d) == j + 1:
                        d[j] = i-1
                    else:
                        assert(len(d) == j)
                        d.append(i-1)
            c.append(min_c)
            cprint(('GRAY', j), ('YELLOW_B', c[j]), ('GREEN', p.split(' ')[j-1], ' '))
    print d
    def print_neat(i):
        if i == 0:
            return ''
        elif i == -1:
            return print_neat(d[i]) + ' '.join(w[d[i]:])
        else:
            return print_neat(d[i]) + ' '.join(w[d[i]:i]) + '\n'
    s = print_neat(-1)
    cprint(('GREEN', 'space sum: ', space_sum(s, M), '\n'))

if __name__ == '__main__':
    M = 50
    test_case = 1
    print '-'*M, '\n'
    print paragraph[test_case]
    print
    stupid_neat(paragraph[test_case], M)
    cprint(('YELLOW', 'space sum: ', space_sum(p_control[test_case], M), '\n'))

    neat(paragraph[test_case], M)
    print
