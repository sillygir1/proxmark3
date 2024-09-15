#include "hfmf_view.h"

#include "../file_manager_glue.h"
#include "hf_copyuid_view.h"
#include "view_list.h"
#include "view_manager.h"

static lv_obj_t *list;
static lv_obj_t *uid_label;
static lv_obj_t *magic_label;
static iso14a_card_select_t *card;

#define ITEMS_NUM 3
static const char *menu_items[ITEMS_NUM] = {"Read UID", "Write UID", "Back"};
static const char *menu_icons[ITEMS_NUM] = {
    LV_SYMBOL_DOWNLOAD, LV_SYMBOL_UPLOAD, LV_SYMBOL_BACKSPACE};

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  ViewManager *view_manager = lv_event_get_user_data(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    // printf("%s\n", button_text);
    if (strcmp(button_text, "Read UID") == 0) {
      card = hf14a_read(false, true, false, false);
      if (!card || !card->uidlen > 0)
        return;
      lv_label_set_text(uid_label, "UID: ");
      char buff[16];
      for (uint8_t i = 0; i < card->uidlen; i++) {
        snprintf(buff, 16, "%02X ", card->uid[i]);
        lv_label_ins_text(uid_label, LV_LABEL_POS_LAST, buff);
      }
    } else if (strcmp(button_text, "Write UID") == 0) {
      if (!hf_get_magic_tag_type()) {
        lv_label_set_text(magic_label, "No magic card detected");
        return;
      }
      lv_label_set_text(magic_label, "Magic card detected");

      // TODO write uid

    } else if (strcmp(button_text, "Back") == 0) {
      view_manager_switch_view(view_manager, VIEW_MAIN_MENU, NULL);
    }
  } else if (code == LV_EVENT_KEY &&
             lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
    view_manager_switch_view(view_manager, VIEW_MAIN_MENU, NULL);
  }
}

void hf_copyuid_init(void *_view_manager, void *ctx) {
  ViewManager *view_manager = _view_manager;
  list = lv_list_create(view_manager->obj_parent);
  lv_obj_set_style_radius(list, 0, LV_PART_MAIN);
  lv_obj_set_width(list, 240);
  lv_obj_set_height(list, 110);
  lv_obj_align(list, LV_ALIGN_BOTTOM_LEFT, 0, 0);
  set_mode_text("Copy UID");

  uid_label = lv_label_create(view_manager->obj_parent);
  lv_obj_set_size(uid_label, 220, 50);
  lv_obj_set_style_text_font(uid_label, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_set_style_text_line_space(uid_label, 5, LV_PART_MAIN);
  lv_obj_align(uid_label, LV_ALIGN_TOP_LEFT, 10, 10);
  lv_label_set_text(uid_label, "Read a tag first");

  magic_label = lv_label_create(view_manager->obj_parent);
  lv_obj_set_size(magic_label, 220, 50);
  lv_obj_set_style_text_font(magic_label, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_set_style_text_line_space(magic_label, 5, LV_PART_MAIN);
  lv_obj_align(magic_label, LV_ALIGN_TOP_LEFT, 10, 60);
  lv_label_set_text(magic_label, "");

  lv_obj_t *btn;
  for (uint8_t i = 0; i < ITEMS_NUM; i++) {
    btn = lv_list_add_btn(list, menu_icons[i], menu_items[i]);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
  }
}

void hf_copyuid_exit() {
  if (card)
    free(card);
  lv_obj_del(uid_label);
  lv_obj_del(magic_label);
  lv_obj_del(list);
}
