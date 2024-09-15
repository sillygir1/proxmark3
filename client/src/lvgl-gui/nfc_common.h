#pragma once
#include "../cmdhf14a.h"
#include "../comms.h"
#include "storage.h"
#include "view_manager.h"
#include <fpga.h>
#include <mifare.h>
#include <mifare/mifarehost.h>
#include <protocols.h>

typedef enum {
  TYPE_NONE = 0,
  TYPE_ISO14443A,
  TYPE_MIFARECLASSIC,

  TYPE_COUNT,
} CardType;

typedef struct {
  void *data;
  uint16_t prev_view;
} UserData;

iso14a_card_select_t *hf14a_read(bool keep_field, bool skip_RATS, bool ecp,
                                 bool magsafe);

uint16_t hf_get_magic_tag_type();

bool hf_set_magic_uid(uint8_t *uid, uint8_t uid_len, uint16_t magic_type);
