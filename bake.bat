@echo off
:: cl /Z7 code\main.c /Fo:blisp /Fe:blisp /link /incremental:no
clang code\main.c -oblisp.exe -g -D_CRT_SECURE_NO_WARNINGS -fsanitize=address
:: clang code\main.c
