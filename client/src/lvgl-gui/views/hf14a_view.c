#include "hf14a_view.h"
#include "view_list.h"
#include "view_manager.h"

static lv_obj_t *list;

#define ITEMS_NUM 5
static const char *menu_items[ITEMS_NUM] = {"Read tag", "Tag info",
                                            "Simulate tag", "Sniff", "Dummy 1"};
static const char *menu_icons[ITEMS_NUM] = {LV_SYMBOL_LIST, LV_SYMBOL_LIST,
                                            LV_SYMBOL_LIST, LV_SYMBOL_LIST,
                                            LV_SYMBOL_LIST};

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  ViewManager *view_manager = lv_event_get_user_data(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    // printf("%s\n", button_text);
    if (strcmp(button_text, menu_items[0]) == 0) {
      view_manager_switch_view(view_manager, VIEW_HF14AREAD, NULL);
    } else if (strcmp(button_text, menu_items[1]) == 0) {
      // view_manager_switch_view(view_manager, VIEW_HF14AINFO);
    } else if (strcmp(button_text, menu_items[2]) == 0) {
      // view_manager_switch_view(view_manager, VIEW_HF14ASIM);
    } else if (strcmp(button_text, menu_items[3]) == 1) {
      // view_manager_switch_view(view_manager, VIEW_HF14ASNIFF);
    }
  } else if (code == LV_EVENT_KEY) {
    if (lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
      view_manager_switch_view(view_manager, VIEW_MAIN_MENU, NULL);
    }
  }
}

void hf14a_init(void *_view_manager, void *ctx) {
  ViewManager *view_manager = _view_manager;
  list = lv_list_create(view_manager->obj_parent);
  lv_obj_set_style_radius(list, 0, LV_PART_MAIN);
  lv_obj_set_width(list, 240);
  lv_obj_set_height(list, 295);
  set_mode_text("HF 14a");
  lv_obj_t *btn;
  for (uint8_t i = 0; i < ITEMS_NUM; i++) {
    btn = lv_list_add_btn(list, menu_icons[i], menu_items[i]);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
  }
}

void hf14a_exit() { lv_obj_del(list); }
