#include "hf14a_sniff_view.h"
#include "../../cmdhf14a.h"
#include "../../comms.h"
#include "../gui_common.h"
#include "view_list.h"
#include "view_manager.h"

static lv_obj_t *list;
static lv_timer_t *timer;
static bool opt_card;
static bool opt_reader;
static bool sniffing;

#define OPTIONS_NUM 4
static const char *options[OPTIONS_NUM] = {"Sniff", "Trigger", "Card",
                                           "Reader 7-bit cmd"};

static void sniff_timer(lv_timer_t *_timer) {
  uint8_t param = (opt_card * 0x01) | (opt_reader * 0x02);
  clearCommandBuffer();
  SendCommandNG(CMD_HF_ISO14443A_SNIFF, (uint8_t *)&param, sizeof(uint8_t));

  PacketResponseNG resp;
  WaitForResponse(CMD_HF_ISO14443A_SNIFF, &resp);

  sniffing = false;
  set_mode_text("ISO 14443-A sniff");
  clear_input();
  lv_timer_pause(_timer);
}

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  ViewManager *view_manager = lv_event_get_user_data(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    lv_obj_t *chb;
    if (strcmp(button_text, options[0]) == 0) {
      if (sniffing)
        return;
      set_mode_text("Sniffing in process");
      sniffing = true;
      lv_timer_resume(timer);
    } else {
      chb = lv_obj_get_child(obj, -1);
      if (lv_obj_has_state(chb, LV_STATE_CHECKED)) {
        lv_obj_clear_state(chb, LV_STATE_CHECKED);
      } else {
        lv_obj_add_state(chb, LV_STATE_CHECKED);
      }
      if (strcmp(button_text, options[1]) == 0)
        opt_card = lv_obj_has_state(chb, LV_STATE_CHECKED);
      else if (strcmp(button_text, options[2]) == 0)
        opt_reader = lv_obj_has_state(chb, LV_STATE_CHECKED);
    }
  } else if (code == LV_EVENT_KEY) {
    if (lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
      view_manager_switch_view(view_manager, VIEW_HF14A, NULL);
    }
  }
}

void hf14a_sniff_init(void *_view_manager, void *ctx) {
  set_mode_text("ISO 14443-A sniff");
  sniffing = false;
  timer = lv_timer_create(sniff_timer, 500, NULL);

  ViewManager *view_manager = _view_manager;
  list = lv_list_create(view_manager->obj_parent);
  lv_obj_set_style_radius(list, 0, LV_PART_MAIN);
  lv_obj_set_size(list, 240, 295);

  lv_obj_t *btn;
  btn = lv_list_add_btn(list, LV_SYMBOL_LIST, options[0]);
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
  lv_list_add_text(list, options[1]);
  for (uint8_t i = 2; i < OPTIONS_NUM; i++) {
    btn = lv_list_add_btn(list, LV_SYMBOL_LIST, options[i]);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
    lv_obj_t *chb = lv_checkbox_create(btn);
    lv_group_remove_obj(chb);
    lv_checkbox_set_text(chb, "");
    lv_obj_align(chb, LV_ALIGN_RIGHT_MID, 0, 0);
  }
}

void hf14a_sniff_exit() {
  lv_timer_del(timer);
  lv_obj_del(list);
}
