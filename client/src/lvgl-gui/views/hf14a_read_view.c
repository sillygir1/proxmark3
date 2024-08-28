#include "hf14a_read_view.h"
#include "../../cmdhf14a.h"
#include "../../comms.h"
#include "../gui_common.h"
#include "view_list.h"
#include "view_manager.h"
#include <mifare.h>

static lv_obj_t *list;
bool keep_field;
bool ecp;
bool magsafe;
bool skip_RATS;

#define OPTIONS_NUM 5
static const char *options[OPTIONS_NUM] = {"Read", "Leave field ON", "Use ECP",
                                           "Magsafe", "Skip RATS"};

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  ViewManager *view_manager = lv_event_get_user_data(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    lv_obj_t *chb;
    if (strcmp(button_text, options[0]) == 0) {
      iso14a_card_select_t *card =
          hf14a_read(keep_field, skip_RATS, ecp, magsafe);
      if (card && card->uidlen > 0) {
        CardData *card_data = malloc(sizeof(*card_data));
        card_data->card = card;
        card_data->prev_view = VIEW_HF14AREAD;
        view_manager_switch_view(view_manager, VIEW_HF14ACARD, card_data);
      }
    } else {
      chb = lv_obj_get_child(obj, -1);
      if (lv_obj_has_state(chb, LV_STATE_CHECKED)) {
        lv_obj_clear_state(chb, LV_STATE_CHECKED);
      } else {
        lv_obj_add_state(chb, LV_STATE_CHECKED);
      }
      if (strcmp(button_text, options[1]) == 0)
        keep_field = lv_obj_has_state(chb, LV_STATE_CHECKED);
      else if (strcmp(button_text, options[2]) == 0)
        ecp = lv_obj_has_state(chb, LV_STATE_CHECKED);
      else if (strcmp(button_text, options[3]) == 0)
        magsafe = lv_obj_has_state(chb, LV_STATE_CHECKED);
      else if (strcmp(button_text, options[4]) == 0)
        skip_RATS = lv_obj_has_state(chb, LV_STATE_CHECKED);
    }
  } else if (code == LV_EVENT_KEY) {
    if (lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
      view_manager_switch_view(view_manager, VIEW_HF14A, NULL);
    }
  }
}

void hf14a_read_init(void *_view_manager, void *ctx) {
  set_mode_text("ISO 14443-A read");
  ViewManager *view_manager = _view_manager;
  list = lv_list_create(view_manager->obj_parent);
  lv_obj_set_style_radius(list, 0, LV_PART_MAIN);
  lv_obj_set_size(list, 240, 295);

  lv_obj_t *btn;
  for (uint8_t i = 0; i < OPTIONS_NUM; i++) {
    btn = lv_list_add_btn(list, LV_SYMBOL_LIST, options[i]);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
    if (i == 0)
      continue;
    lv_obj_t *chb = lv_checkbox_create(btn);
    lv_group_remove_obj(chb);
    lv_checkbox_set_text(chb, "");
    lv_obj_align(chb, LV_ALIGN_RIGHT_MID, 0, 0);
  }
}

void hf14a_read_exit() { lv_obj_del(list); }
