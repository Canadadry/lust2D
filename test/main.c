#define MINITEST_IMPL
#include "minitest.h"
#include "ui_test.h"
#include "ui_add_test.h"

int main(int argc,char ** argv){
    TEST_GROUP(test_ui_add);
    TEST_GROUP(test_ui_compute);
    test_done();
    return 0;
}
