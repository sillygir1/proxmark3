#include "hfmf_view.h"

#include "../file_manager_glue.h"
#include "hf14a_view.h"
#include "view_list.h"
#include "view_manager.h"

static lv_obj_t *list;

#define ITEMS_NUM 5
static const char *menu_items[ITEMS_NUM] = {"Info", "Dump", "Saved tags",
                                            "Trace", "Back"};
static const char *menu_icons[ITEMS_NUM] = {LV_SYMBOL_FILE, LV_SYMBOL_SAVE,
                                            LV_SYMBOL_DIRECTORY, LV_SYMBOL_LIST,
                                            LV_SYMBOL_BACKSPACE};

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  ViewManager *view_manager = lv_event_get_user_data(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    // printf("%s\n", button_text);
    if (strcmp(button_text, "Info") == 0) {
      // Info view
    } else if (strcmp(button_text, "Dump") == 0) {
      // Dump view
    } else if (strcmp(button_text, "Saved tags") == 0) {
      FileManagerData *fm_data = malloc(sizeof(*fm_data));
      fm_data->prev_view = VIEW_MFC_MENU;
      strcpy(fm_data->dir, MFC_PATH);
      fm_data->file_type = TYPE_MIFARECLASSIC;
      fm_data->event_handler = file_manager_event_handler;
      view_manager_switch_view(view_manager, VIEW_FILE_MANAGER, fm_data);
      file_manager_glue(fm_data, view_manager);
    } else if (strcmp(button_text, "Trace") == 0) {
      UserData *td = malloc(sizeof(*td));
      td->prev_view = VIEW_MFC_MENU;
      td->data = TYPE_MIFARECLASSIC;
      view_manager_switch_view(view_manager, VIEW_HFTRACE, td);
    } else if (strcmp(button_text, "Back") == 0) {
      view_manager_switch_view(view_manager, VIEW_MAIN_MENU, NULL);
    }
  } else if (code == LV_EVENT_KEY &&
             lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
    view_manager_switch_view(view_manager, VIEW_MAIN_MENU, NULL);
  }
}

void hfmf_init(void *_view_manager, void *ctx) {
  ViewManager *view_manager = _view_manager;
  list = lv_list_create(view_manager->obj_parent);
  lv_obj_set_style_radius(list, 0, LV_PART_MAIN);
  lv_obj_set_width(list, 240);
  lv_obj_set_height(list, 295);
  set_mode_text("Mifare Classic");
  lv_obj_t *btn;
  for (uint8_t i = 0; i < ITEMS_NUM; i++) {
    btn = lv_list_add_btn(list, menu_icons[i], menu_items[i]);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
  }
}

void hfmf_exit() { lv_obj_del(list); }
