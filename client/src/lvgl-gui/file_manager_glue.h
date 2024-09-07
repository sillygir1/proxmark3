#pragma once

#include "file_manager.h"
#include "file_system.h"
#include "gui_common.h"
#include "view_manager.h"
#include "views/view_list.h"

void file_manager_event_handler(lv_event_t *e);

void file_manager_glue(FileManagerData *fm_data, ViewManager *view_manager);
