#include "view_list.h"
#include "../view_manager.h"
#include "hf14a_read_view.h"
#include "hf14a_view.h"
#include "main_menu_view.h"

void views_init(void *_view_manager) {
  ViewManager *view_manager = _view_manager;

  // Add all views here
  view_manager_add_view(view_manager, &main_menu_init, &main_menu_exit,
                        VIEW_MAIN_MENU);
  view_manager_add_view(view_manager, &hf14a_init, &hf14a_exit, VIEW_HF14A);
  view_manager_add_view(view_manager, &hf14a_read_init, &hf14a_read_exit,
                        VIEW_HF14AREAD);
}