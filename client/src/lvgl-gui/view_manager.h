#pragma once

#include "encoder.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lvgl/lvgl.h"
#include "views/view_list.h"

typedef struct {
  void (*init)(void *, lv_obj_t *);
  void (*exit)();
} View;

typedef struct {
  View *view[VIEW_COUNT];
  uint8_t current_view;
  lv_obj_t *obj_parent;
} ViewManager;

/// @brief Initialize view manager
/// @return Pointer to view manager struct
ViewManager *view_manager_init();

/// @brief Add view to the view manager
/// @param view_manager view manager struct
/// @param init init function of the view
/// @param input input function of the view
/// @param exit exit function of the view
/// @param number view's unique number
void view_manager_add_view(ViewManager *view_manager, int (*init)(),
                           void (*exit)(), ViewList number);

/// @brief Switch to view
/// @param view_manager view manager struct
/// @param number view's unique number
/// @return 0 if successfull
int view_manager_switch_view(ViewManager *view_manager, ViewList number);

/// @brief Remove view from view manager
/// @param view_manager view manager struct
/// @param number view's unique number
void view_manager_remove_view(ViewManager *view_manager, ViewList number);

/// @brief Free view manager
/// @param view_manager view manager struct
void view_manager_free(ViewManager *view_manager);
