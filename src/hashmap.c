#define MAX_ITER 100
#define SAFE_WHILE(cond) \
    for (int _sw_guard = 0; (cond) && _sw_guard < (MAX_ITER); _sw_guard++)

unsigned long hash(const char *s) {
    unsigned long h = 0x100;
    SAFE_WHILE(*s) {
        h ^= *s++;
        h *= 1111111111111111111u;
    }
    return h;
}
