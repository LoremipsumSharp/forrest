#!/usr/bin/env python
# -*- coding: utf-8 -*-

# LCS(longest common subsequence) problem
# see Introduction to Algorithms, p.350~355

# author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/

import sys

class color(object):
    BLACK     = "\033[0;30m"
    RED       = "\033[0;31m"
    GREEN     = "\033[0;32m"
    YELLOW    = "\033[0;33m"
    BLUE      = "\033[0;34m"
    MAGENTA   = "\033[0;35m"
    CYAN      = "\033[0;36m"
    WHITE     = "\033[0;37m"
    BLACK_B   = "\033[0;1;30m"
    RED_B     = "\033[0;1;31m"
    GREEN_B   = "\033[0;1;32m"
    YELLOW_B  = "\033[0;1;33m"
    BLUE_B    = "\033[0;1;34m"
    MAGENTA_B = "\033[0;1;35m"
    CYAN_B    = "\033[0;1;36m"
    WHITE_B   = "\033[0;1;37m"
    NOCOLOR   = "\033[0m"

    @staticmethod
    def s(c, str):
        c = 'color.%s' % c
        return eval(c) + str + color.NOCOLOR

    @staticmethod
    def i(c, i):
        return color.s(c, str(i))

def LCS_recursive(X, Y):
    if len(X) == 0 or len(Y) == 0:
        return 0, []
    elif X[-1] == Y[-1]:
        i, l = LCS_recursive(X[:-1], Y[:-1])
        return i+1, l+[X[-1]]
    else:
        i1, l1 = LCS_recursive(X[:-1], Y)
        i2, l2 = LCS_recursive(X, Y[:-1])
        if i1 > i2:
            return i1, l1
        else:
            return i2, l2

class grid(object):
    def __init__(self):
        self.len = 0
        self.dir = None
    def __repr__(self):
        if self.dir != None:
            return '%s %s        ' % (color.s('GREEN', self.dir), color.i('RED', self.len))
        else:
            return '%s %s        ' % (' ', color.i('RED', self.len))

def matrix_print(func):
    def wrapper(X, Y):
        l = func(X, Y)

        m, n = len(X), len(Y)
        print '    ',
        for j in range(-1,m):
            print color.s('WHITE', '%11d' % j),
        print '\n'
        for i in range(-1,n):       # -1..m-1
            print color.s('WHITE', '  %2d        ' % i),
            for j in range(-1,m):
                print l[i][j],
            print '\n'

        return l
    return wrapper


def LCS(X, Y):
    m, n = len(X), len(Y)

    @matrix_print
    def LCS0(X, Y):
        #           m
        #   ::::::::::::::::
        # n ::::::::::::::::
        #   ::::::::::::::::
        l = [ [grid() for i in range(m + 1) ] for j in range(n + 1) ]

        for i in range(-1,n):      # -1..n-1
            for j in range(-1,m):  # -1..m-1
                if i == -1 or j == -1:
                    l[i][j].len = 0
                elif X[j] == Y[i]:
                    l[i][j].len = l[i-1][j-1].len + 1
                    l[i][j].dir = '↖'
                else:
                    i1 = l[i-1][j].len
                    i2 = l[i][j-1].len
                    if (i2 > i1):
                        l[i][j].len = l[i][j-1].len
                        l[i][j].dir = '←'
                    else:
                        l[i][j].len = l[i-1][j].len
                        l[i][j].dir = '↑'
            #     print color.i('CYAN', l[i][j].len),
            # print

        return l

    def p_lcs(i, j):
        #print i, j, l[i][j]
        if l[i][j].len == 0:
            return
        elif l[i][j].dir == '↑':
            p_lcs(i-1, j)
        elif l[i][j].dir == '←':
            p_lcs(i, j-1)
        elif l[i][j].dir == '↖':
            p_lcs(i-1, j-1)
            assert X[j] == Y[i]
            print color.s('CYAN', X[j]),
        else:
            assert 0

    l = LCS0(X, Y)
    p_lcs(n-1, m-1)

if __name__ == '__main__':
    print sys.argv

    if 1:
        A,B,C,D = 'A','B','C','D'
        Y = [A, B, C, B, D, A, B]
        X = [B, D, C, A, B, A]
    else:  # Introduction to Algorithm, Ex-15.4-1 @ p.355
        X = ['1', '0', '0', '1', '0', '1', '0', '1']
        Y = ['0', '1', '0', '1', '1', '0', '1', '1', '0']

    print 'LCS of\n\t', X, '\nand\n\t', Y, '\n:'
    if len(sys.argv) == 1 or sys.argv[1] == '-r':
        print LCS_recursive(X, Y)
    elif sys.argv[1] == '-d':
        LCS(X, Y)
    else:
        print 'bad arg:', sys.argv[1:]
