#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>

// Initialize GMP variables and state
gmp_randstate_t state;

void init_gmp() {
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL)); // Seed with current time
}

void clear_gmp() {
    gmp_randclear(state);
}
void generateSafePrimeAndGenerator(mpz_t p, mpz_t g, gmp_randstate_t state) {

    mpz_t q, p_sub_1_div2, g_x ;
    mpz_inits(q, p_sub_1_div2, g_x, NULL);

    // Generate a 'safe' prime p = 2q + 1
    do {
        mpz_urandomb(q, state, 2048); // Generate a random number of bit_size - 1
        mpz_nextprime(q, q); // Find the next prime starting at q
        mpz_mul_ui(p, q, 2); // p = 2q
        mpz_add_ui(p, p, 1); // p = 2q + 1
    } while (mpz_probab_prime_p(p, 25) == 0); // Ensure p is prime


    // Calculate p_sub_1_div2 = (p-1)/2 to use in generator selection
    mpz_sub_ui(p_sub_1_div2, p, 1);
    mpz_divexact_ui(p_sub_1_div2, p_sub_1_div2, 2);

    // Select a generator g that is safe from QR/QNR attacks
    do {
        mpz_urandomm(g, state, p); // Generate random g < p
        mpz_powm(g_x, g, p_sub_1_div2, p); // g^((p-1)/2) mod p
    
    } while (mpz_cmp_ui(g_x, 1) == 0 && mpz_cmp_ui(g, 1) == 0); 

     mpz_powm_ui(g, g, 2, p); // g^2 mod p

    mpz_clears(q, p_sub_1_div2, g_x, NULL);
}

void setPrivateKey(mpz_t x) {
    // Set the private key x to the specified value
    mpz_set_str(x, "1234567890123456789012345678901234567890", 10);
}

void generatePublicKey(mpz_t p, mpz_t g, mpz_t x, mpz_t h) {
    // h = g^x mod p
    mpz_powm(h, g, x, p);
}

void encrypt(mpz_t c1, mpz_t c2, mpz_t m, mpz_t p, mpz_t g, mpz_t h, gmp_randstate_t state) {
    mpz_t r, hr;
    mpz_init(r);
    mpz_init(hr);

    mpz_urandomm(r, state, p); // r is random in Z_p
    mpz_powm(c1, g, r, p); // c1 = g^r mod p
    mpz_powm(hr, h, r, p); // hr = h^r mod p
    mpz_mul(c2, m, hr); // c2 = m * hr mod p
    mpz_mod(c2, c2, p); // Ensure c2 is within Z_p

    mpz_clear(r);
    mpz_clear(hr);
}

int main() {
    init_gmp();

    mpz_t p, g, x, h, m, c1, c2, decrypted_m;
    mpz_inits(p, g, x, h, m, c1, c2, decrypted_m, NULL);

    generateSafePrimeAndGenerator(p, g, state);
    setPrivateKey(x);
    generatePublicKey(p, g, x, h);


    FILE *inputFile = fopen("./input", "r");
    if (!inputFile) {
        fprintf(stderr, "Failed to open input file.\n");
        exit(EXIT_FAILURE);
    }

    // Read the message m from the input file
    if (!mpz_inp_str(m, inputFile, 10)) { // Assuming the number in inputFile is in base 10
        fprintf(stderr, "Failed to read message m from input file.\n");
        fclose(inputFile);
        exit(EXIT_FAILURE);
    }

    gmp_printf("Message m read from file: %Zd\n", m);

    // Close the input file as it's no longer needed
    fclose(inputFile);


    FILE *outputFile = fopen("./output", "w");
    if (!outputFile) {
        perror("Failed to open output file");
        exit(1);
    }

    char *c1_str, *c2_str, *p_str;
    for (int i = 0; i < 3; i++) {
        encrypt(c1, c2, m, p, g, h, state);

        // Convert GMP integers to strings
        c1_str = mpz_get_str(NULL, 10, c1);
        c2_str = mpz_get_str(NULL, 10, c2);
        p_str = mpz_get_str(NULL, 10, p);

        // Write to the file using fprintf
        fprintf(outputFile, "%s,%s,%s\n", c1_str, c2_str, p_str);

        // Free the allocated strings
        free(c1_str);
        free(c2_str);
        free(p_str);
    }


    fclose(outputFile);

    //decrypt(decrypted_m, c1, c2, p, x);

    mpz_clears(p, g, x, h, m, c1, c2, decrypted_m, NULL);
    clear_gmp();

    return EXIT_SUCCESS; 
}
