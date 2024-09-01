#pragma once
#include "gui_common.h"
#include "storage.h"

#define ISO14443A_PATH "/root/pilk/uid/"
#define ISO14443A_HEADER "ISO14443A_CARD"

typedef struct {
  CardType type;
  char *filename;
  char *dir;
  uint16_t prev_view;
  bool leaving;
} FileManagerData;

void *fs_read_card(FileManagerData *fm_data);

void *fs_save_card(void *_card, CardType type);
