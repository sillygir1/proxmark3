#include "file_system.h"
#include "nfc_common.h"

void *fs_read_card(FileManagerData *fm_data) {
  if (!fm_data)
    return NULL;

  char *buff = malloc(sizeof(char) * 64);
  int n = storage_file_read(fm_data->dir, fm_data->filename, buff, 64);

  if (strncmp(ISO14443A_HEADER, buff, 14) == 0) {
    // Parse file as iso 14443-a
    iso14a_card_select_t *card = malloc(sizeof(*card));
    card->ats_len = 0;
    char *end; // For strtol
    char *p = buff + 15;
    char byte[3] = {0, 0, 0};
    card->uidlen = 0;
    while (card->uidlen < 10) {
      strncpy(byte, p, 2);
      if (byte[0] == '\n' || byte[1] == '\n') {
        p++;
        break;
      }
      card->uid[card->uidlen] = strtol(byte, &end, 16);
      p += 2;
      card->uidlen++;
    }
    for (uint8_t i = 0; i < 2; i++) {
      strncpy(byte, p, 2);
      card->atqa[1 - i] = strtol(byte, &end, 16);
      p += 2;
    }
    p++;
    strncpy(byte, p, 2);
    card->sak = strtol(byte, &end, 16);

    free(buff);
    return card;
  }

  free(buff);
  return NULL;
}

void *fs_save_card(void *_card, CardType type) {
  if (type == TYPE_ISO14443A) {
    iso14a_card_select_t *card = _card;
    char *filename = malloc(sizeof(char) * 64);
    memset(filename, 0, 64);

    char buff[256];
    for (uint8_t i = 0; i < card->uidlen; i++) {
      snprintf(buff, 3, "%02X", card->uid[i]);
      strcat(filename, buff);
    }
    char uid[15];
    memset(uid, 0, 15);
    strncpy(uid, filename, 2 * card->uidlen);
    strcat(filename, ".uid");
    printf("Saving to %s\n", filename);
    snprintf(buff, 256, "%s\n%s\n%02X%02X\n%02X\n", ISO14443A_HEADER, uid,
             card->atqa[1], card->atqa[0], card->sak);
    storage_file_write(ISO14443A_PATH, filename, buff);
    free(filename);
  }
}