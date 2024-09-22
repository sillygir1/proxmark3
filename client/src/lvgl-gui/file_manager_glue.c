#include "file_manager_glue.h"

static lv_obj_t *list;
static ViewManager *view_manager;
static FileManagerData *fm_data;

void file_manager_event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    // printf("%s\n", button_text);
    if (strcmp(button_text, "Back") == 0) {
      goto leave;
    }
    fm_data->filename = button_text;

    if (strcmp(fm_data->filename, "..") == 0) {
      if (strcmp(fm_data->dir, "/") == 0)
        return;
      storage_dir_up(fm_data->dir);
      file_manager_update_list();
      return;
    }
    if (storage_is_dir(fm_data->dir, fm_data->filename)) {
      storage_dir_down(fm_data->dir, fm_data->filename);
      file_manager_update_list();
      return;
    }

    printf("a\n");
    CardType type = TYPE_NONE;
    if (fm_data->file_type == TYPE_NONE) {
      char *ext = storage_get_ext(fm_data->filename);
      if (!ext) {
        type == TYPE_NONE;
        return;
      }
      if (strcmp(ext, ISO14443A_EXT) == 0) {
        type = TYPE_ISO14443A;
      }
    } else {
      type = fm_data->file_type;
    }

    printf("b\n");
    switch (type) {
    case TYPE_ISO14443A:;
      UserData *card_data = calloc(1, sizeof(*card_data));
      card_data->data = fs_read_card(fm_data);
      card_data->prev_view = VIEW_FILE_MANAGER;
      view_manager_switch_view(view_manager, VIEW_HF14ACARD, card_data);
      break;
    default:
      // Do something idk
      break;
    }
  } else if (code == LV_EVENT_KEY &&
             lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
  leave:
    fm_data->leaving = true;
    view_manager_switch_view(view_manager, fm_data->prev_view, NULL);
  }
}

void file_manager_glue(FileManagerData *_fm_data, ViewManager *_view_manager) {
  list = file_manager_glue_stick();
  view_manager = _view_manager;
  fm_data = _fm_data;
}