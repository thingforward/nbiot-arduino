/*
 *  Copyright (C) 2018  Digital Incubation & Growth GmbH
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *  This software is dual-licensed. For commercial licensing options, please
 *  contact the authors (see README).
 */

#include <string.h>

namespace Narrowband {

int split_csv_line(char *buf, size_t n, char *arr_res[], int n_max_arr, const char *p_expect_cmdstring) {
    if ( p_expect_cmdstring != NULL) {
        char *sep = strchr(buf,':');
        int i = 0, j = 0;
        char *p = 0;
        if (buf[0] == '+' && sep != 0) {
            *sep = '\0';
            j = (sep-buf);
            p = ++sep;

            if ( strcmp(buf, p_expect_cmdstring) != 0) {
                return -2;      // unexpected command
            }

            while (sep != 0 && i < n_max_arr && (size_t)j < n) {
                arr_res[i++] = p;
                sep = strchr(p,',');
                if ( sep != 0) {
                    *sep = '\0';
                    j = (sep-buf);
                    p = ++sep;
                }
            }

            return i;
        } else {
            return -1;      // format invalid
        }
    } else {
        char *sep = buf;
        char *p = buf;
        int i = 0, j = 0;
        j = (sep-buf);

        while (sep != 0 && i < n_max_arr && (size_t)j < n) {
            arr_res[i++] = p;
            sep = strchr(p,',');
            if ( sep != 0) {
                *sep = '\0';
                j = (sep-buf);
                p = ++sep;
            }
        }

        return i;
    }

    return 0;
}


}

