#include "splitter.h"

Splitter* init_splitter(Splitter* splitter,const char* str, char separator) {
    if (splitter != NULL) {
        splitter->str = str;
        splitter->start = str;
        splitter->separator = separator;
    }
    return splitter;
}

SplitterSlice next_slice(Splitter* splitter) {
    SplitterSlice result = {NULL, 0};

    if (splitter == NULL || splitter->start == NULL) {
        return result;
    }

    const char* str = splitter->start;
    const char* separator_pos = strchr(str, splitter->separator);

    if (separator_pos == NULL) {
        result.data = str;
        result.len = strlen(str);
        splitter->start = NULL;
    } else {
        result.data = str;
        result.len = separator_pos - str;
        splitter->start = separator_pos + 1;
    }

    return result;
}
