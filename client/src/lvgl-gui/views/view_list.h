#pragma once

typedef enum {
  VIEW_NONE = 0,
  VIEW_MAIN_MENU,
  VIEW_HF14A,
  VIEW_HF14AREAD,
  VIEW_HF14ACARD,
  VIEW_HF14ASNIFF,
  VIEW_HF14ASIM,
  VIEW_MIFARE_CLASSIC,

  VIEW_COUNT, // Always last element
} ViewList;

void views_init(void *_view_manager);
