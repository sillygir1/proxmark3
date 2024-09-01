#include "hf_trace_view.h"
#include "view_list.h"
#include "view_manager.h"

static lv_obj_t *list;
static uint8_t *trace;
static uint16_t trace_len;
TraceData *trace_data;

static int trace_download() {
  // We be copying code from private functions
  if (trace)
    free(trace);
  trace_len = 0;
  trace = calloc(PM3_CMD_DATA_SIZE, sizeof(uint8_t));
  if (!trace) {
    printf("Trace memory allocation error");
    return 1;
  }

  PacketResponseNG resp;
  if (!GetFromDevice(BIG_BUF, trace, PM3_CMD_DATA_SIZE, 0, NULL, 0, &resp, 4000,
                     true)) {
    printf("Trace download timeout\n");
    free(trace);
    trace = NULL;
    return 1;
  }
  trace_len = resp.oldarg[2];
  if (trace_len > PM3_CMD_DATA_SIZE) {
    free(trace);
    trace = calloc(trace_len, sizeof(uint8_t));
    if (!trace) {
      printf("Trace memory allocation error");
      return 1;
    }
    if (!GetFromDevice(BIG_BUF, trace, trace_len, 0, NULL, 0, NULL, 2500,
                       false)) {
      printf("Trace download timeout\n");
      free(trace);
      trace = NULL;
      return 1;
    }
  }
  return 0;
}

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  ViewManager *view_manager = lv_event_get_user_data(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    // printf("%s\n", button_text);

  } else if (code == LV_EVENT_KEY) {
    if (lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
      view_manager_switch_view(view_manager, trace_data->prev_view, NULL);
    }
  }
}

void hf_trace_init(void *_view_manager, void *ctx) {
  ViewManager *view_manager = _view_manager;
  trace_data = ctx;

  set_mode_text("Trace");
  list = lv_list_create(view_manager->obj_parent);
  lv_obj_set_style_radius(list, 0, LV_PART_MAIN);
  lv_obj_set_width(list, 240);
  lv_obj_set_height(list, 295);
  if (trace_download() != 0) {
    printf("Can't download trace!\n");
    view_manager_switch_view(view_manager, trace_data->prev_view, NULL);
  }
  lv_obj_t *btn;
  if (trace_len == 0) {
    btn = lv_list_add_btn(list, LV_SYMBOL_CLOSE, "No trace");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
    return;
  }
  for (uint8_t i = 0; i < 100; i++) {
    btn = lv_list_add_btn(list, LV_SYMBOL_LIST, "Placeholder");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
  }
}

void hf_trace_exit() {
  if (trace) {
    free(trace);
    trace = NULL;
  }
  if (trace_data) {
    free(trace_data);
    trace_data = NULL;
  }
  lv_obj_del(list);
}
