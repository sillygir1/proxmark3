#include "gui_common.h"

iso14a_card_select_t *hf14a_read(bool keep_field, bool skip_RATS, bool ecp,
                                 bool magsafe) {
  uint32_t cm = ISO14A_CONNECT;
  if (keep_field)
    cm |= ISO14A_NO_DISCONNECT;
  if (skip_RATS)
    cm |= ISO14A_NO_RATS;
  if (ecp || magsafe) {
    iso14a_polling_parameters_t *polling_parameters = NULL;
    iso14a_polling_parameters_t parameters =
        iso14a_get_polling_parameters(ecp, magsafe);
    cm |= ISO14A_USE_CUSTOM_POLLING;
    polling_parameters = &parameters;
    SendCommandMIX(CMD_HF_ISO14443A_READER, cm, 0, 0,
                   (uint8_t *)polling_parameters,
                   sizeof(iso14a_polling_parameters_t));
  } else {
    SendCommandMIX(CMD_HF_ISO14443A_READER, cm, 0, 0, NULL, 0);
  }

  PacketResponseNG resp;
  WaitForResponseTimeout(CMD_ACK, &resp, 2500);
  DropField();
  iso14a_card_select_t *card = malloc(sizeof(*card));
  memcpy(card, (iso14a_card_select_t *)resp.data.asBytes,
         sizeof(iso14a_card_select_t));
  return card;
}