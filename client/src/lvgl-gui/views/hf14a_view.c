#include "hf14a_view.h"
#include "../view_manager.h"

static lv_obj_t *list;

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  ViewManager *view_manager = lv_event_get_user_data(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    printf("%s\n", button_text);
    // Switch view
  } else if (code == LV_EVENT_KEY) {
    if (lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
      view_manager_switch_view(view_manager, VIEW_MAIN_MENU);
    }
  }
}

void hf14a_init(void *_view_manager, lv_obj_t *obj_parent) {
  ViewManager *view_manager = _view_manager;
  list = lv_list_create(obj_parent);
  lv_obj_set_style_radius(list, 0, LV_PART_MAIN);
  lv_obj_set_width(list, 240);
  lv_obj_set_height(list, 295);
  set_mode_text("HF 14a");
  lv_obj_t *btn;
  btn = lv_list_add_btn(list, LV_SYMBOL_LIST, "Read tag");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
  btn = lv_list_add_btn(list, LV_SYMBOL_LIST, "Sniff");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
  btn = lv_list_add_btn(list, LV_SYMBOL_LIST, "Idk");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
  btn = lv_list_add_btn(list, LV_SYMBOL_LIST, "Idk 2");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
  btn = lv_list_add_btn(list, LV_SYMBOL_LIST, "Idk 3");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
}

void hf14a_exit() { lv_obj_del(list); }
