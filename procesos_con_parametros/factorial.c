#include <gmp.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void factorial(mpz_t *result, uint64_t x) {
  if (x == 0) {
    mpz_mul_ui(*result, *result, 1);
    return;
  }

  factorial(result, x - 1);
  mpz_mul_ui(*result, *result, x);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Uso: factorial [número]\r\n");
    return 1;
  }

  char *num_input = argv[1];

  int64_t parsed_number = atoll(num_input);

  if (parsed_number == 0 && (strcmp("0", num_input) != 0)) {
    fprintf(stderr, "El factorial está indefinido para números negativos.\r\n");
    return 1;
  }

  if (parsed_number < 0) {
    fprintf(stderr, "El factorial está indefinido para números negativos.\r\n");
    return 2;
  }

  uint64_t number = parsed_number;

  mpz_t result;
  mpz_init_set_ui(result, 1);

  factorial(&result, number);

  gmp_printf("El factorial de %lu es %Zu\r\n", number, result);

	mpz_clear(result);

  return 0;
}
