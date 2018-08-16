#include <Arduino.h>

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

