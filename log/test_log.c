#include "log.h"
#include <stdio.h>

int test_default()
{
    log_init();
    DEBUG("===test DEBUG===");
    ERROR("===test error===");
    log_shutdown();
}

int test_user()
{
    set_log_level(E_LOG_LEVEL_DEBUG);
    set_log_dir("log_dir");
    set_log_to_file(1);
    test_default();
}

int main()
{
    test_default();
    test_user();

    return 0;
}
