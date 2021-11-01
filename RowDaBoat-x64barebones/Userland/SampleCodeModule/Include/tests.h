#ifndef TESTS_H
#define TESTS_H

#include "Lib.h"
#include <stdlib.h>
#include "test_util.h"
#include <stdio.h>

void test_mm();
int main_test_sync(int argc, char ** argv);
int main_test_no_sync(int argc, char **argv);
int main_test_prior(int argc, char **argv);

void test_processes();

#endif