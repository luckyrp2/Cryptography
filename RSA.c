#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simplified RSA encryption simulation
void rsa_encrypt(const mpz_t m, mpz_t c, const mpz_t e, const mpz_t n) {
    mpz_powm(c, m, e, n); // c = m^e mod n
}

// Simplified RSA decryption simulation
void rsa_decrypt(const mpz_t c, mpz_t m, const mpz_t d, const mpz_t n) {
    mpz_powm(m, c, d, n); // m = c^d mod n
}

// Function to calculate RSA parameters and write them to the output file
void perform_rsa_and_write_output(const mpz_t m, const mpz_t c_prime, const mpz_t d_prime, const mpz_t p_prime, const mpz_t q_prime) {
    mpz_t c, e, d, n, m_prime;
    char output_str[1024]; // Adjust size as needed

    mpz_inits(c, e, d, n, m_prime, NULL);

    // Simulate generating RSA keys (n, e, d)
    mpz_mul(n, p_prime, q_prime); // n = p' * q'
    mpz_set_ui(e, 65537); // Common choice for e
    mpz_set(d, d_prime); // Using provided d' for simplicity

    // Encrypt m to get c
    rsa_encrypt(m, c, e, n);

    // Decrypt c' to get m'
    rsa_decrypt(c_prime, m_prime, d_prime, n);

    // Write output to file
    FILE *fp = fopen("./output", "w+");
    if (!fp) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }
    gmp_sprintf(output_str, "%Zd,%Zd,%Zd,%Zd,%Zd", c, e, d, n, m_prime);
    fprintf(fp, "%s", output_str);
    fclose(fp);

    mpz_clears(c, e, d, n, m_prime, NULL);
}

// Function to read five large numbers from a file
int read_large_numbers(const char *filename, mpz_t numbers[], int count) {
    FILE *fp;
    char buffer[4096]; // Buffer size increased to accommodate large numbers
    int numRead = 0;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return -1;
    }

    // Assuming numbers are stored in one line, separated by commas
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        char* token = strtok(buffer, ",");
        while (token != NULL && numRead < count) {
            mpz_set_str(numbers[numRead], token, 10); // Base 10 for decimal numbers
            numRead++;
            token = strtok(NULL, ",");
        }
    }

    fclose(fp);
    return (numRead == count) ? 0 : -2; // Ensure all numbers were read
}

int main() {
    const int NUM_COUNT = 5;
    mpz_t numbers[NUM_COUNT];
    int i;

    for (i = 0; i < NUM_COUNT; i++) {
        mpz_init(numbers[i]);
    }

    if (read_large_numbers("input", numbers, NUM_COUNT) != 0) {
        fprintf(stderr, "Failed to read numbers from the file.\n");
        for (i = 0; i < NUM_COUNT; i++) {
            mpz_clear(numbers[i]);
        }
        return EXIT_FAILURE;
    }

    perform_rsa_and_write_output(numbers[0], numbers[1], numbers[2], numbers[3], numbers[4]);

    for (i = 0; i < NUM_COUNT; i++) {
        mpz_clear(numbers[i]);
    }

    return EXIT_SUCCESS;
}
