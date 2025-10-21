#include "args.h"
#include <stdarg.h>
#include <string.h>


HasArgResult __has_arg(int argc, char **argv, ...) {
    HasArgResult result = {0};
    va_list args;
    va_start(args, argv);

    const char *alias = NULL;
    while ((alias = va_arg(args, const char*))) {
        for (int i = 1; i < argc; i++) {
            const char * arg_val =argv[i];
            if (strcmp(arg_val, alias) == 0) {
                if(i<(argc-1)){
                    result.next=argv[i+1];
                }
                result.ok=1;
                va_end(args);
                return result;
            }
        }
    }
    va_end(args);
    return result;
}
