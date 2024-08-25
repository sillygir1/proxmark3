#pragma once

typedef enum {
  VIEW_NONE,
  VIEW_MAIN_MENU,
  VIEW_HF14A,
  VIEW_MIFARE_CLASSIC,

  VIEW_COUNT, // Always last element
} ViewList;

void views_init(void *_view_manager);
