#include "gui.h"
#include "views/view_list.h"

static bool running;
lv_timer_t *battery_timer;
ProxmarkData *proxmark_data;

static void encoder_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
  Input input;
  if (proxmark_data->enc_data->inputQueue[INPUT_DEVICE_ENCODER]->QueueSize >
      0) {
    input = input_queue_read(
        proxmark_data->enc_data->inputQueue[INPUT_DEVICE_ENCODER]);
    // printf("CB: encoder %s\n", input.key ? "down" : "up");
    switch (input.key) {
    case INPUT_KEY_UP:
      data->enc_diff = -1;
      break;
    case INPUT_KEY_DOWN:
      data->enc_diff = 1;
      break;
    default:
      printf("Input reading error\n");
      break;
    }
  }
}

static void buttons_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
  Input input;
  if (proxmark_data->enc_data->inputQueue[INPUT_DEVICE_BUTTONS]->QueueSize >
      0) {
    input = input_queue_read(
        proxmark_data->enc_data->inputQueue[INPUT_DEVICE_BUTTONS]);
    // printf("CB: buttons %s\n", input.key == INPUT_KEY_BACK ? "back" :
    // "enter");
    switch (input.key) {
    case INPUT_KEY_BACK:
      data->key = LV_KEY_ESC;
      break;
    case INPUT_KEY_OK:
      data->key = LV_KEY_ENTER;
      break;
    default:
      printf("Input reading error\n");
      break;
    }
    data->state = input.type;
  }
}

void draw_status_bar() {
  proxmark_data->battery_icon = lv_label_create(lv_scr_act());

  lv_label_set_text(proxmark_data->battery_icon, "0\% \xEF\x89\x82");
  lv_obj_set_align(proxmark_data->battery_icon, LV_ALIGN_TOP_RIGHT);
  lv_obj_set_style_pad_right(proxmark_data->battery_icon, 5, LV_PART_MAIN);

  lv_obj_set_y(proxmark_data->battery_icon, 5);

  proxmark_data->mode_label = lv_label_create(lv_scr_act());
  lv_label_set_text(proxmark_data->mode_label, "USB RNDIS");
  lv_obj_set_align(proxmark_data->mode_label, LV_ALIGN_TOP_LEFT);
  lv_obj_set_style_pad_left(proxmark_data->mode_label, 5, LV_PART_MAIN);

  lv_obj_set_y(proxmark_data->mode_label, 5);
}

void update_charge() {
  if (!proxmark_data->adc_fd) {
    printf("ADC not initialized!\n");
    return;
  }
  char text[64];
  float voltage = read_voltage(proxmark_data->adc_fd);
  char symbol[4];
  if (voltage > 4.1) {
    snprintf(symbol, 4, "%s", LV_SYMBOL_BATTERY_FULL);
  } else if (voltage > 3.9) {
    snprintf(symbol, 4, "%s", LV_SYMBOL_BATTERY_3);
  } else if (voltage > 3.7) {
    snprintf(symbol, 4, "%s", LV_SYMBOL_BATTERY_2);
  } else if (voltage > 3.5) {
    snprintf(symbol, 4, "%s", LV_SYMBOL_BATTERY_1);
  } else {
    snprintf(symbol, 4, "%s", LV_SYMBOL_BATTERY_EMPTY);
  }
  snprintf(text, 64, "%.01fV %s", voltage, symbol);
  if (proxmark_data->battery_icon)
    lv_label_set_text(proxmark_data->battery_icon, text);
}

void set_mode_text(char *text) {
  lv_label_set_text(proxmark_data->mode_label, text);
}

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  // Main_menu *main_menu = lv_event_get_user_data(e);

  if (code == LV_EVENT_CLICKED) {

  } else if (code == LV_EVENT_KEY) {
    if (lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
      // Do nothing on the main screen
      printf("Back button pressed!\n");
    }
  }
}

void battery_timer_cb(lv_timer_t *timer) {
  if (proxmark_data->adc_fd && proxmark_data->battery_icon)
    update_charge();
}

static void sig_handler(int _) { running = false; }

void gui_init() {
  signal(SIGINT, sig_handler);

  printf("Gui initialization!\n");

  /*LittlevGL init*/
  lv_init();

  /*Linux frame buffer device init*/
  fbdev_init();

  /*A small buffer for LittlevGL to draw the screen's content*/
  static lv_color_t buf1[DISP_BUF_SIZE];
  static lv_color_t buf2[DISP_BUF_SIZE];

  /*Initialize a descriptor for the buffer*/
  static lv_disp_draw_buf_t disp_buf;
  lv_disp_draw_buf_init(&disp_buf, buf1, buf2, DISP_BUF_SIZE);

  /*Initialize and register a display driver*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.draw_buf = &disp_buf;
  disp_drv.flush_cb = fbdev_flush;
  disp_drv.hor_res = 240;
  disp_drv.ver_res = 320;
  lv_disp_drv_register(&disp_drv);

  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Loading");
  lv_obj_center(label);
  lv_timer_handler();
  lv_obj_del(label);

  /*Initialize and register an input drivers*/
  static lv_indev_drv_t encoder_drv;
  lv_indev_drv_init(&encoder_drv);
  encoder_drv.type = LV_INDEV_TYPE_ENCODER;
  encoder_drv.read_cb = encoder_cb;
  lv_indev_t *encoder_indev = lv_indev_drv_register(&encoder_drv);

  static lv_indev_drv_t buttons_drv;
  lv_indev_drv_init(&buttons_drv);
  buttons_drv.type = LV_INDEV_TYPE_KEYPAD;
  buttons_drv.read_cb = buttons_cb;
  lv_indev_t *buttons_indev = lv_indev_drv_register(&buttons_drv);

  proxmark_data = malloc(sizeof(*proxmark_data));
  proxmark_data->running = &running;

  proxmark_data->input_group = lv_group_create();
  lv_group_set_default(proxmark_data->input_group);
  lv_indev_set_group(encoder_indev, proxmark_data->input_group);
  lv_indev_set_group(buttons_indev, proxmark_data->input_group);
  proxmark_data->enc_data = malloc(sizeof(*proxmark_data->enc_data));
  proxmark_data->enc_data->running = &running;

  encoder_grab(proxmark_data->enc_data);

  proxmark_data->adc_fd = adc_init();
  if (proxmark_data->adc_fd < 0)
    printf("Adc init issue");

  battery_timer = lv_timer_create(battery_timer_cb, 2000, NULL);

  draw_status_bar();
  update_charge();

  proxmark_data->view_manager = view_manager_init(VIEW_COUNT);
  lv_obj_set_scrollbar_mode(proxmark_data->view_manager->obj_parent,
                            LV_SCROLLBAR_MODE_OFF);
  views_init(proxmark_data->view_manager);
  view_manager_switch_view(proxmark_data->view_manager, VIEW_MAIN_MENU, NULL);
}

void gui_loop() {
  struct timespec rem, req = {0, 100 * 1000 * 1000};
  while (running) {
    lv_timer_handler();

    nanosleep(&req, &rem);
  }
  printf("Exiting...\n");
  encoder_release(proxmark_data->enc_data);
  lv_timer_del(battery_timer);
  view_manager_free(proxmark_data->view_manager);
  close(proxmark_data->adc_fd);
  free(proxmark_data->enc_data);
  free(proxmark_data);
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void) {
  static uint64_t start_ms = 0;
  if (start_ms == 0) {
    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);
    start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
  }

  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  uint64_t now_ms;
  now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

  uint32_t time_ms = now_ms - start_ms;
  return time_ms;
}