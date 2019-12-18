#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

/* execute arbitrary instructions */

#undef RET2P_RA_OFFSET

#define RET2P_RA_OFFSET 2 /* in `sizeof(void *)`s; skip past canary */

/* decode a hexadecimal character */
int hex_decodec(uint8_t *dest, char c) {
  if (dest == NULL) {
    return -EFAULT;
  }

  /* decode */

  c = tolower(c);

  if (c >= '0'
      && c <= '9') {
    *dest = c - '0';
  } else if (c >= 'a'
      && c <= 'f') {
    *dest = c - 'a' + 10;
  } else {
    /* not a hexadecimal character */

    return -EINVAL;
  }
  return 0;
}

/* decode a hexadecimal string */
int hex_decodes(uint8_t *dest, size_t *destlen, size_t destlim,
    const char *src) {
  size_t _destlen;
  size_t i;
  uint8_t nibble;
  int retval;
  size_t srclen;

  if (dest == NULL) {
    return -EFAULT;
  }

  if (src == NULL) {
    return -EFAULT;
  }
  srclen = strlen(src);

  if (destlim * 2 < srclen) {
    /* `lim` is too small */

    return -ENOMEM;
  }

  /* decode */

  memset(dest, '\0', destlim);

  for (_destlen = i = 0; i < srclen; i++, src++) {
    retval = hex_decodec(&nibble, *src);

    if (retval) {
      return retval;
    }

    if (!(i & 1)) {
      /* high nibble */

      nibble <<= 4;
    }
    *dest |= nibble;

    if (i & 1) {
      /* low nibble */

      dest++;
      _destlen++;
    }
  }

  if (destlen != NULL) {
    *destlen = _destlen;
  }
  return 0;
}

/* return to an arbitrary location */
static int ret2p(const char *p);

int main(int argc, char **argv) {
  unsigned int retval;
  uint8_t *textbuf;
  size_t textbuflen;

  if (argc != 2
      || !strlen(argv[1])) {
    printf("execute arbitrary instructions\n" \
      "Usage: %s HEX\n" \
      "HEX\n" \
      "\tnonempty instruction sequence, encoded as hexadecimal\n", argv[0]);
    return 1;
  }
  
  /* create the text buffer */

  textbuflen = strlen(argv[1]);
  textbuflen = (textbuflen % 2 ? textbuflen + 1 : textbuflen) / 2;
  textbuf = (uint8_t *) alloca(textbuflen);

  if (textbuf == NULL) {
    return -ENOMEM;
  }
  memset(textbuf, '\0', textbuflen);

  /* decode */

  retval = hex_decodes(textbuf, NULL, textbuflen, argv[1]);

  if (retval) {
    return retval;
  }

  /* prep return to `textbuf` */

  return ret2p(textbuf); /* `textbuf` doesn't need to be freed */
}

static int ret2p(const char *p) {
  if (p == NULL) {
    return -EFAULT;
  }
  
  /* return to `p` */

  ((char **) &p)[RET2P_RA_OFFSET] = (char *) p;
  return 0;
}

