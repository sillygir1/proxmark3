#pragma once
#include "../cmdhf14a.h"
#include "../comms.h"
#include "storage.h"
#include "view_manager.h"
#include <mifare.h>

typedef enum {
  TYPE_NONE = 0,
  TYPE_ISO14443A,
  TYPE_MIFARECLASSIC,

  TYPE_COUNT,
} CardType;

typedef struct {
  void *card;
  uint16_t prev_view;
} CardData;

typedef struct {
  uint16_t prev_view;
  CardType type;
} TraceData;

iso14a_card_select_t *hf14a_read(bool keep_field, bool skip_RATS, bool ecp,
                                 bool magsafe);
