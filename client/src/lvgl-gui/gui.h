#pragma once

#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lvgl/lvgl.h"
#include "lvgl/src/font/lv_symbol_def.h"
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "battery.h"
#include "encoder.h"

#define DISP_BUF_SIZE (128 * 1024)

typedef struct {
  bool *running;
  Data *enc_data;
  lv_group_t *input_group;
  lv_obj_t *battery_icon;
  lv_obj_t *mode_label;
  int adc_fd;
} ProxmarkData;

void gui_init();

void gui_loop();

// From <time.h>
int nanosleep(const struct timespec *req, struct timespec *rem);
