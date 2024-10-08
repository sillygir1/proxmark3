#include "hf14a_view.h"
#include "../file_manager_glue.h"
#include "view_list.h"
#include "view_manager.h"
#include "view_manager_extra.h"

static lv_obj_t *list;
static ViewManager *view_manager;

#define ITEMS_NUM 5
static const char *menu_items[ITEMS_NUM] = {"Read tag", "Saved tags", "Sniff",
                                            "Trace", "Back"};
static const char *menu_icons[ITEMS_NUM] = {LV_SYMBOL_FILE, LV_SYMBOL_DIRECTORY,
                                            LV_SYMBOL_EYE_OPEN, LV_SYMBOL_LIST,
                                            LV_SYMBOL_BACKSPACE};

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    // printf("%s\n", button_text);
    if (strcmp(button_text, "Read tag") == 0) {
      view_manager_switch_view(view_manager, VIEW_HF14AREAD, NULL);
    } else if (strcmp(button_text, "Saved tags") == 0) {
      FileManagerData *fm_data = calloc(1, sizeof(*fm_data));
      fm_data->prev_view = VIEW_HF14A;
      strcpy(fm_data->dir, ISO14443A_PATH);
      fm_data->file_type = TYPE_ISO14443A;
      fm_data->event_handler = file_manager_event_handler;
      view_manager_switch_view(view_manager, VIEW_FILE_MANAGER, fm_data);
      file_manager_glue(fm_data, view_manager);
    } else if (strcmp(button_text, "Sniff") == 0) {
      view_manager_switch_view(view_manager, VIEW_HF14ASNIFF, NULL);
    } else if (strcmp(button_text, "Trace") == 0) {
      UserData *td = calloc(1, sizeof(*td));
      td->prev_view = VIEW_HF14A;
      td->data = TYPE_ISO14443A;
      view_manager_switch_view(view_manager, VIEW_HFTRACE, td);
    } else if (strcmp(button_text, "Back") == 0) {
      view_manager_switch_view(view_manager, VIEW_MAIN_MENU, NULL);
    }
  } else if (code == LV_EVENT_KEY) {
    if (lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
      view_manager_switch_view(view_manager, VIEW_MAIN_MENU, NULL);
    }
  }
}

void hf14a_init(void *_view_manager, void *ctx) {
  view_manager = _view_manager;
  set_mode_text("ISO 14443-A");

  ViewManagerList vm_list = {
      menu_items, (const void **)menu_icons, ITEMS_NUM, NULL, 0, event_handler};
  list = view_manager_list_init(view_manager, &vm_list);
}

void hf14a_exit() { lv_obj_del(list); }
