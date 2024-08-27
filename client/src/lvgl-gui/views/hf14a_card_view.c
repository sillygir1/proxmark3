#include "../../cmdhf14a.h"
#include "../../comms.h"
#include "hf14a_read_view.h"
#include "view_list.h"
#include "view_manager.h"
#include <mifare.h>

static lv_obj_t *label;
static lv_obj_t *list;

#define OPTIONS_NUM 2
static const char *options[OPTIONS_NUM] = {"Save", "Simulate"};

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  ViewManager *view_manager = lv_event_get_user_data(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    lv_obj_t *chb;
    if (strcmp(button_text, options[0]) == 0) {
      // Save
    } else if (strcmp(button_text, options[1]) == 0) {
      // Simulate
    }
  } else if (code == LV_EVENT_KEY) {
    if (lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
      view_manager_switch_view(view_manager, VIEW_HF14AREAD, NULL);
    }
  }
}

void hf14a_card_init(void *_view_manager, void *ctx) {
  set_mode_text("ISO 14443-A card");
  ViewManager *view_manager = _view_manager;
  iso14a_card_select_t *card = ctx;
  list = lv_list_create(view_manager->obj_parent);
  lv_obj_set_style_radius(list, 0, LV_PART_MAIN);
  lv_obj_set_size(list, 240, 80);
  lv_obj_align(list, LV_ALIGN_BOTTOM_LEFT, 0, 0);

  lv_obj_t *btn;
  for (uint8_t i = 0; i < OPTIONS_NUM; i++) {
    btn = lv_list_add_btn(list, LV_SYMBOL_LIST, options[i]);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
  }

  label = lv_label_create(view_manager->obj_parent);
  lv_obj_set_size(label, 220, 200);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_set_style_text_line_space(label, 5, LV_PART_MAIN);
  lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 10);

  if (!card->uidlen && card->uidlen > 7) {
    printf("No card found\n");
    return;
  }
  lv_label_set_text(label, "UID: ");
  char buff[16];
  for (uint8_t i = 0; i < card->uidlen; i++) {
    snprintf(buff, 16, "%02X ", card->uid[i]);
    lv_label_ins_text(label, LV_LABEL_POS_LAST, buff);
  }
  snprintf(buff, 16, "\nATQA: %02X %02X", card->atqa[1], card->atqa[0]);
  lv_label_ins_text(label, LV_LABEL_POS_LAST, buff);
  snprintf(buff, 16, "\nSAK: %02X", card->sak);
  lv_label_ins_text(label, LV_LABEL_POS_LAST, buff);
  if (!card->ats_len) {
    return;
  }
  lv_label_ins_text(label, LV_LABEL_POS_LAST, "\nATS: ");
  for (uint8_t i = 0; i < card->ats_len; i++) {
    snprintf(buff, 16, "%02X ", card->ats[i]);
    lv_label_ins_text(label, LV_LABEL_POS_LAST, buff);
  }
}

void hf14a_card_exit() {
  lv_obj_del(label);
  lv_obj_del(list);
}
