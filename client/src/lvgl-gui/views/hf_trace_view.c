#include "hf_trace_view.h"
#include "view_list.h"
#include "view_manager.h"

static lv_obj_t *list;
static uint8_t *trace;
static uint16_t trace_len;
static TraceData *trace_data;
static ViewManager *view_manager;

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

static int trace_draw() {
  tracelog_hdr_t *first_hdr = (tracelog_hdr_t *)(trace);
  uint16_t trace_pos = 0;
  char line[512];
  lv_obj_t *btn;

  memset(line, 0, 512);
  snprintf(line, 32, "Trace length: %u", trace_len);
  btn = lv_list_add_btn(list, LV_SYMBOL_LIST, line);
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);

  while ((trace_pos + TRACELOG_HDR_LEN) < trace_len) {
    memset(line, 0, 512);
    tracelog_hdr_t *hdr = (tracelog_hdr_t *)(trace + trace_pos);
    uint16_t data_len = hdr->data_len;
    uint8_t *frame = hdr->frame;
    for (uint8_t i = 0; i < data_len; i++) {
      char buff[4] = {0, 0, 0, 0};
      snprintf(buff, 4, "%02X ", frame[i]);
      strcat(line, buff);
    }
    char explanation[60] = {0};
    switch (trace_data->type) {
    case TYPE_ISO14443A:
      annotateIso14443a(explanation, sizeof(explanation), frame, data_len,
                        hdr->isResponse);
      break;
    }

    if (strlen(explanation) > 0)
      lv_list_add_text(list, explanation);
    btn = lv_list_add_btn(
        list, hdr->isResponse ? LV_SYMBOL_DOWNLOAD : LV_SYMBOL_UPLOAD, line);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);

    trace_pos += TRACELOG_HDR_LEN + data_len + TRACELOG_PARITY_LEN(hdr);
  }
}

void hf_trace_init(void *_view_manager, void *ctx) {
  view_manager = _view_manager;
  trace_data = ctx;
  switch (trace_data->type) {
  case TYPE_ISO14443A:
    set_mode_text("Trace ISO 14443-A");
    break;
  default:
    printf("Type %d not implemented yet\n", trace_data->type);
    view_manager_switch_view(view_manager, trace_data->prev_view, NULL);
    return;
  }
  list = lv_list_create(view_manager->obj_parent);
  lv_obj_set_style_radius(list, 0, LV_PART_MAIN);
  lv_obj_set_width(list, 240);
  lv_obj_set_height(list, 295);
  if (trace_download() != 0) {
    printf("Can't download trace!\n");
    view_manager_switch_view(view_manager, trace_data->prev_view, NULL);
    return;
  }
  trace_draw();
  lv_obj_t *btn;
  if (trace_len == 0) {
    btn = lv_list_add_btn(list, LV_SYMBOL_CLOSE, "No trace");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
    return;
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
