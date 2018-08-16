#ifndef __NBUTILS_H
#define __NBUTILS_H

#include <Arduino.h>

namespace Narrowband {

int split_csv_line(char *buf, size_t n, char *arr_res[], int n_max_arr, const char *p_expect_cmdstring = NULL);

}

#endif