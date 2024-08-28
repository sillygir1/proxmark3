#include "../cmdhf14a.h"
#include "../comms.h"
#include "view_manager.h"
#include <mifare.h>

typedef struct {
  void *card;
  uint8_t prev_view;
} CardData;

iso14a_card_select_t *hf14a_read(bool keep_field, bool skip_RATS, bool ecp,
                                 bool magsafe);
