#include "file_manager_view.h"
#include "view_list.h"
#include "view_manager.h"

#define DIR_LIST_LEN 32

static lv_obj_t *list;
static FileManagerData *fm_data;

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  ViewManager *view_manager = lv_event_get_user_data(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    printf("%s\n", button_text);
    fm_data->filename = button_text;

    switch (fm_data->type) {
    case TYPE_ISO14443A:;
      CardData *card_data = malloc(sizeof(*card_data));
      // card_data->card = fs_read_card();
      view_manager_switch_view(view_manager, VIEW_HF14ACARD, fm_data);
      break;
    }

  } else if (code == LV_EVENT_KEY) {
    if (lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
      fm_data->leaving = true;
      view_manager_switch_view(view_manager, fm_data->prev_view, NULL);
    }
  }
}

void file_manager_init(void *_view_manager, void *ctx) {
  ViewManager *view_manager = _view_manager;
  list = lv_list_create(view_manager->obj_parent);
  lv_obj_set_style_radius(list, 0, LV_PART_MAIN);
  lv_obj_set_width(list, 240);
  lv_obj_set_height(list, 295);
  set_mode_text("File manager");

  if (!fm_data)
    fm_data = ctx;
  fm_data->leaving = false;
  char *arr[DIR_LIST_LEN];

  lv_obj_t *btn;
  int n = storage_dir_list(fm_data->dir, arr, DIR_LIST_LEN);
  for (uint8_t i = 0; i < n; i++) {
    btn = lv_list_add_btn(list, LV_SYMBOL_SAVE, arr[i]);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
    free(arr[i]);
  }
}

void file_manager_exit() {
  if (fm_data && fm_data->leaving)
    free(fm_data);
  lv_obj_del(list);
}
