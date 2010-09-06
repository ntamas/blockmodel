static int test_counter = 0;
#define CHECK(func) {     \
    test_counter++;       \
    int _retval = func(); \
    if (_retval) {        \
        printf("#%d %s failed (return code = %d)\n", \
               test_counter, #func, _retval);        \
        return _retval;   \
    } \
}


