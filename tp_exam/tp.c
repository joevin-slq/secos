/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <segmentation.h>
#include <userland.h>
#include <syscall.h>
#include <pagination.h>

extern info_t *info;

void tp()
{
    segmentation();
    pagination();
    syscall();
    userland();
}
