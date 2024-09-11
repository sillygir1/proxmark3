#pragma once

#include "file_manager.h"
#include "nfc_common.h"
#include "storage.h"

#define PATH_MAXLEN 128

#define DEFAULT_PATH "/root/pilk/"
#define ISO14443A_PATH "/root/pilk/uid/"
#define ISO14443A_EXT ".uid"
#define ISO14443A_HEADER "ISO14443A_CARD"
#define MFC_PATH "/root/pilk/mfc/"

// typedef struct {
//   int file_type;
//   char *filename;
//   char dir[PATH_MAXLEN];
//   uint16_t prev_view;
//   bool leaving;
// } FileManagerData;

void *fs_read_card(FileManagerData *fm_data);

void *fs_save_card(void *_card, CardType type);
