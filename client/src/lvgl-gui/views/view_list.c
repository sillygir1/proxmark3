#include "view_list.h"
#include "file_manager_view.h"
#include "hf14a_card_view.h"
#include "hf14a_read_view.h"
#include "hf14a_view.h"
#include "main_menu_view.h"
#include "view_manager.h"

void views_init(void *_view_manager) {
  ViewManager *view_manager = _view_manager;

  // Add all views here
  view_manager_add_view(view_manager, &main_menu_init, &main_menu_exit,
                        VIEW_MAIN_MENU);
  view_manager_add_view(view_manager, &hf14a_init, &hf14a_exit, VIEW_HF14A);
  view_manager_add_view(view_manager, &hf14a_read_init, &hf14a_read_exit,
                        VIEW_HF14AREAD);
  view_manager_add_view(view_manager, &hf14a_card_init, &hf14a_card_exit,
                        VIEW_HF14ACARD);
  view_manager_add_view(view_manager, &file_manager_init, &file_manager_exit,
                        VIEW_FILE_MANAGER);
}
