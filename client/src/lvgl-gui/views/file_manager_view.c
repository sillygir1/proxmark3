#include "file_manager_view.h"
#include "view_list.h"
#include "view_manager.h"

#define DIR_LIST_LEN 32

static lv_obj_t *list;
static ViewManager *view_manager;
static FileManagerData *fm_data;
static bool skip_dirs;

static void update_list();

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    // printf("%s\n", button_text);
    fm_data->filename = button_text;

    if (strcmp(fm_data->filename, "..") == 0) {
      storage_dir_up(fm_data->dir);
      update_list();
      return;
    }
    if (storage_is_dir(fm_data->dir, fm_data->filename)) {
      storage_dir_down(fm_data->dir, fm_data->filename);
      update_list();
      return;
    }

    CardType type = TYPE_NONE;
    if (fm_data->type == TYPE_NONE) {
      char *ext = storage_get_ext(fm_data->filename);
      if (!ext) {
        type == TYPE_NONE;
        return;
      }
      if (strcmp(ext, ISO14443A_EXT) == 0) {
        type = TYPE_ISO14443A;
      }
    } else {
      type = fm_data->type;
    }

    switch (type) {
    case TYPE_ISO14443A:;
      UserData *card_data = malloc(sizeof(*card_data));
      card_data->data = fs_read_card(fm_data);
      card_data->prev_view = VIEW_FILE_MANAGER;
      view_manager_switch_view(view_manager, VIEW_HF14ACARD, card_data);
      break;
    default:
      // Do something idk
      break;
    }
  } else if (code == LV_EVENT_KEY) {
    if (lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
      fm_data->leaving = true;
      view_manager_switch_view(view_manager, fm_data->prev_view, NULL);
    }
  }
}

static void update_list() {
  printf("Updating list\n");
  char *arr[DIR_LIST_LEN];
  set_mode_text(fm_data->dir);
  lv_obj_clean(list);
  lv_obj_t *btn;
  int n = storage_dir_list(fm_data->dir, arr, DIR_LIST_LEN, skip_dirs);
  if (n == 0) {
    btn = lv_list_add_btn(list, LV_SYMBOL_CLOSE, "No saved files");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_KEY, view_manager);
  } else {
    for (uint8_t i = 0; i < n; i++) {
      if (!storage_is_dir(fm_data->dir, arr[i]))
        continue;
      btn = lv_list_add_btn(list, LV_SYMBOL_DIRECTORY, arr[i]);
      lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
      free(arr[i]);
    }
    for (uint8_t i = 0; i < n; i++) {
      if (storage_is_dir(fm_data->dir, arr[i]))
        continue;
      btn = lv_list_add_btn(list, LV_SYMBOL_SAVE, arr[i]);
      lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, view_manager);
      free(arr[i]);
    }
  }
}

void file_manager_init(void *_view_manager, void *ctx) {
  view_manager = _view_manager;
  list = lv_list_create(view_manager->obj_parent);
  lv_obj_set_style_radius(list, 0, LV_PART_MAIN);
  lv_obj_set_width(list, 240);
  lv_obj_set_height(list, 295);

  if (!fm_data)
    fm_data = ctx;
  fm_data->leaving = false;

  skip_dirs = true;
  if (fm_data->type == TYPE_NONE) {
    skip_dirs = false;
  }

  update_list();
}

void file_manager_exit() {
  if (fm_data && fm_data->leaving) {
    free(fm_data);
    fm_data = NULL;
  }
  lv_obj_del(list);
}
