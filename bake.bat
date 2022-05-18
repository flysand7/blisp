@echo off
cl /Z7 code\main.c /Fo:blisp /Fe:blisp /link /incremental:no
:: clang code\main.c
