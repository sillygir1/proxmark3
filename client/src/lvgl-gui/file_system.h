#pragma once
#include "gui_common.h"
#include "storage.h"

#define ISO14443A_PATH "/root/pilk/uid/"
#define ISO14443A_HEADER "ISO14443A_CARD"

void *fs_save_card(void *_card, CardType type);
