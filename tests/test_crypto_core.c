/**
 * tests/test_crypto_core.c
 * Harnais de test pour Cryptage_Core.c (V38.0.6) — étape 2.
 *
 * Étape 1 (validée sur votre machine, 11/11) : round-trip hex, round-trip
 * chiffrement, mauvais mot de passe.
 * Étape 2 (ce fichier, remplace entièrement le précédent) : cas limites
 * et négatifs sur le format .crypt.
 *
 * Structure réelle de l'en-tête AAD (24 octets), vérifiée par lecture
 * directe du code d'encrypt_data/decrypt_data (le commentaire de
 * #define AAD_LEN dans Cryptage.h dit "Version(4)+Reserved(16)+MemKiB(4)",
 * ce qui ne colle pas : la réalité du code est) :
 *   [0:4)   version (uint32 LE) = CURRENT_VERSION (370)
 *   [4:16)  réservé, mis à zéro (12 octets, non utilisé à ce jour)
 *   [16:20) longueur du texte (dé)chiffré (uint32 LE)
 *   [20:24) memory_cost_kib (uint32 LE)
 * suivi de salt (32), nonce (12), tag (16), puis le texte chiffré.
 * MIN_ENCRYPTED_SIZE = 24+32+12+16 = 84 octets (sans texte chiffré).
 *
 * Pas de framework externe à cette étape non plus : même macro CHECK
 * maison qu'à l'étape 1.
 */

#include "Cryptage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int tests_run = 0;
static int tests_failed = 0;

#define CHECK(cond, label) do { \
    tests_run++; \
    if (!(cond)) { \
        tests_failed++; \
        fprintf(stderr, "  [ECHEC] %s\n", label); \
    } else { \
        fprintf(stderr, "  [OK]    %s\n", label); \
    } \
} while (0)

/* ---- Étape 1 : tests fondamentaux (inchangés) ---- */

static void test_hex_roundtrip(void) {
    fprintf(stderr, "-- test_hex_roundtrip --\n");

    unsigned char original[5] = { 0x00, 0x1A, 0xFF, 0x7B, 0x42 };
    char* hex = bin_to_hex(original, sizeof(original), FALSE);
    CHECK(hex != NULL, "bin_to_hex renvoie une chaine non-NULL");

    unsigned char* back = NULL;
    size_t back_len = 0;
    int rc = hex_to_bin(hex, &back, &back_len, FALSE);
    CHECK(rc == 0, "hex_to_bin reussit sur la sortie de bin_to_hex");
    CHECK(back_len == sizeof(original), "la taille reconstituee correspond");
    CHECK(back != NULL && memcmp(back, original, sizeof(original)) == 0,
          "le contenu binaire reconstitue est identique a l'original");

    secure_free(hex);
    secure_free(back);
}

static void test_encrypt_decrypt_roundtrip(void) {
    fprintf(stderr, "-- test_encrypt_decrypt_roundtrip --\n");

    const char* plaintext = "Ceci est un texte de test.";
    const char* password  = "MotDePasseDeTest_123!";
    size_t ciphertext_len = 0;

    unsigned char* ciphertext = encrypt_data(NULL,
        (const unsigned char*)plaintext, strlen(plaintext),
        password, &ciphertext_len, DEFAULT_MEMORY_COST_KIB);
    CHECK(ciphertext != NULL, "encrypt_data reussit");

    unsigned char* decrypted = NULL;
    size_t decrypted_len = 0;
    int rc = decrypt_data(NULL, ciphertext, ciphertext_len, password,
        &decrypted, &decrypted_len, DEFAULT_MEMORY_COST_KIB);

    CHECK(rc == 0, "decrypt_data renvoie 0 (succes)");
    CHECK(decrypted_len == strlen(plaintext), "la taille dechiffree correspond");
    CHECK(decrypted != NULL &&
          memcmp(decrypted, plaintext, strlen(plaintext)) == 0,
          "le texte dechiffre est identique a l'original");

    secure_free(ciphertext);
    secure_free(decrypted);
}

static void test_wrong_password_rejected(void) {
    fprintf(stderr, "-- test_wrong_password_rejected --\n");

    const char* plaintext = "Donnee sensible";
    size_t ciphertext_len = 0;

    unsigned char* ciphertext = encrypt_data(NULL,
        (const unsigned char*)plaintext, strlen(plaintext),
        "BonMotDePasse!1", &ciphertext_len, DEFAULT_MEMORY_COST_KIB);
    CHECK(ciphertext != NULL, "encrypt_data reussit (preparation du test negatif)");

    unsigned char* decrypted = NULL;
    size_t decrypted_len = 0;
    int rc = decrypt_data(NULL, ciphertext, ciphertext_len,
        "MauvaisMotDePasse!2", &decrypted, &decrypted_len,
        DEFAULT_MEMORY_COST_KIB);

    CHECK(rc == 1, "decrypt_data renvoie 1 (mauvais mot de passe)");
    CHECK(decrypted == NULL, "aucun texte n'est renvoye en cas d'echec");

    secure_free(ciphertext);
}

/* ---- Étape 2 : cas limites et négatifs ---- */

static void test_empty_plaintext(void) {
    /* DEFAUT #1 - CORRIGE (voir Corrections_v38.0.7.txt) :
     * secure_malloc() alloue desormais au moins 1 octet meme si size==0,
     * evitant l'echec de VirtualAlloc(0). decrypt_data() reussit donc a
     * present sur un texte chiffre vide (confirme sur le poste de Bernard). */
    fprintf(stderr, "-- test_empty_plaintext --\n");

    const char* password = "MotDePasse_Vide!1";
    size_t ciphertext_len = 0;

    unsigned char* ciphertext = encrypt_data(NULL,
        (const unsigned char*)"", 0, password, &ciphertext_len,
        DEFAULT_MEMORY_COST_KIB);
    CHECK(ciphertext != NULL, "encrypt_data accepte un texte de 0 octet");
    CHECK(ciphertext_len == MIN_ENCRYPTED_SIZE,
          "la sortie ne contient que l'en-tete (84 octets, 0 texte chiffre)");

    unsigned char* decrypted = NULL;
    size_t decrypted_len = 999; /* valeur sentinelle */
    int rc = decrypt_data(NULL, ciphertext, ciphertext_len, password,
        &decrypted, &decrypted_len, DEFAULT_MEMORY_COST_KIB);

    CHECK(rc == 0, "decrypt_data reussit sur un texte vide (defaut #1 corrige)");
    CHECK(decrypted_len == 0, "la taille dechiffree est bien 0");

    secure_free(ciphertext);
    secure_free(decrypted);
}

static void test_max_plaintext_size_accepted(void) {
    fprintf(stderr, "-- test_max_plaintext_size_accepted --\n");

    size_t size = MAX_PLAINTEXT_SIZE; /* 10 Mo pile */
    unsigned char* plaintext = (unsigned char*)malloc(size);
    CHECK(plaintext != NULL, "allocation du buffer de 10 Mo pour le test");
    if (!plaintext) return;
    memset(plaintext, 0xAB, size);

    const char* password = "MotDePasse_Max!1";
    size_t ciphertext_len = 0;
    unsigned char* ciphertext = encrypt_data(NULL, plaintext, size,
        password, &ciphertext_len, DEFAULT_MEMORY_COST_KIB);
    CHECK(ciphertext != NULL, "encrypt_data accepte exactement MAX_PLAINTEXT_SIZE");

    unsigned char* decrypted = NULL;
    size_t decrypted_len = 0;
    int rc = decrypt_data(NULL, ciphertext, ciphertext_len, password,
        &decrypted, &decrypted_len, DEFAULT_MEMORY_COST_KIB);

    CHECK(rc == 0, "decrypt_data reussit sur exactement MAX_PLAINTEXT_SIZE");
    CHECK(decrypted_len == size, "la taille dechiffree correspond a 10 Mo");
    CHECK(decrypted != NULL && memcmp(decrypted, plaintext, size) == 0,
          "le contenu de 10 Mo est restitue a l'identique");

    free(plaintext);
    secure_free(ciphertext);
    secure_free(decrypted);
}

static void test_plaintext_too_large_rejected(void) {
    fprintf(stderr, "-- test_plaintext_too_large_rejected --\n");

    size_t size = MAX_PLAINTEXT_SIZE + 1; /* juste au-dessus de la limite */
    unsigned char* plaintext = (unsigned char*)malloc(size);
    CHECK(plaintext != NULL, "allocation du buffer de 10 Mo + 1 pour le test");
    if (!plaintext) return;
    memset(plaintext, 0xCD, size);

    size_t ciphertext_len = 0;
    unsigned char* ciphertext = encrypt_data(NULL, plaintext, size,
        "PeuImporte!1", &ciphertext_len, DEFAULT_MEMORY_COST_KIB);
    CHECK(ciphertext == NULL,
          "encrypt_data rejette MAX_PLAINTEXT_SIZE + 1 (renvoie NULL)");

    free(plaintext);
    secure_free(ciphertext); /* sans effet si NULL, par securite */
}

static void test_tampered_tag_rejected(void) {
    fprintf(stderr, "-- test_tampered_tag_rejected --\n");

    const char* plaintext = "Texte protege par GCM";
    const char* password = "MotDePasse_Tag!1";
    size_t ciphertext_len = 0;

    unsigned char* ciphertext = encrypt_data(NULL,
        (const unsigned char*)plaintext, strlen(plaintext),
        password, &ciphertext_len, DEFAULT_MEMORY_COST_KIB);
    CHECK(ciphertext != NULL, "encrypt_data reussit (preparation)");

    /* Le tag GCM occupe les octets [AAD_LEN+SALT_LEN+NONCE_LEN,
     * AAD_LEN+SALT_LEN+NONCE_LEN+TAG_LEN) = [68, 84). On modifie
     * le premier de ces octets. */
    if (ciphertext) {
        ciphertext[AAD_LEN + SALT_LEN + NONCE_LEN] ^= 0x01;
    }

    unsigned char* decrypted = NULL;
    size_t decrypted_len = 0;
    int rc = decrypt_data(NULL, ciphertext, ciphertext_len, password,
        &decrypted, &decrypted_len, DEFAULT_MEMORY_COST_KIB);

    CHECK(rc == 1, "un tag GCM falsifie est rejete (code 1, comme un mauvais mot de passe)");
    CHECK(decrypted == NULL, "aucun texte n'est renvoye en cas de tag invalide");

    secure_free(ciphertext);
}

static void test_truncated_header_rejected(void) {
    fprintf(stderr, "-- test_truncated_header_rejected --\n");

    unsigned char short_buf[MIN_ENCRYPTED_SIZE - 1];
    memset(short_buf, 0, sizeof(short_buf));

    unsigned char* decrypted = NULL;
    size_t decrypted_len = 0;
    int rc = decrypt_data(NULL, short_buf, sizeof(short_buf), "PeuImporte!1",
        &decrypted, &decrypted_len, DEFAULT_MEMORY_COST_KIB);

    CHECK(rc == 2, "un tampon plus petit que MIN_ENCRYPTED_SIZE est rejete (code 2)");
    CHECK(decrypted == NULL, "aucun texte n'est renvoye pour un en-tete trop court");
}

static void test_wrong_version_rejected(void) {
    fprintf(stderr, "-- test_wrong_version_rejected --\n");

    const char* plaintext = "Texte avec version modifiee";
    const char* password = "MotDePasse_Version!1";
    size_t ciphertext_len = 0;

    unsigned char* ciphertext = encrypt_data(NULL,
        (const unsigned char*)plaintext, strlen(plaintext),
        password, &ciphertext_len, DEFAULT_MEMORY_COST_KIB);
    CHECK(ciphertext != NULL, "encrypt_data reussit (preparation)");

    if (ciphertext) {
        write_uint32_le(ciphertext, 999); /* CURRENT_VERSION = 370 attendu */
    }

    unsigned char* decrypted = NULL;
    size_t decrypted_len = 0;
    int rc = decrypt_data(NULL, ciphertext, ciphertext_len, password,
        &decrypted, &decrypted_len, DEFAULT_MEMORY_COST_KIB);

    CHECK(rc == 2, "une version != 370 est rejetee (code 2)");
    CHECK(decrypted == NULL, "aucun texte n'est renvoye pour une version invalide");

    secure_free(ciphertext);
}

static void test_invalid_mem_kib_rejected(void) {
    fprintf(stderr, "-- test_invalid_mem_kib_rejected --\n");

    const char* plaintext = "Texte avec mem_kib modifie";
    const char* password = "MotDePasse_MemKib!1";
    size_t ciphertext_len = 0;

    unsigned char* ciphertext = encrypt_data(NULL,
        (const unsigned char*)plaintext, strlen(plaintext),
        password, &ciphertext_len, DEFAULT_MEMORY_COST_KIB);
    CHECK(ciphertext != NULL, "encrypt_data reussit (preparation)");

    if (ciphertext) {
        /* mem_kib doit rester dans [4096, 1048576] ; 100 est hors bornes. */
        write_uint32_le(ciphertext + 20, 100);
    }

    unsigned char* decrypted = NULL;
    size_t decrypted_len = 0;
    int rc = decrypt_data(NULL, ciphertext, ciphertext_len, password,
        &decrypted, &decrypted_len, DEFAULT_MEMORY_COST_KIB);

    CHECK(rc == 2, "un memory_cost_kib hors [4096, 1048576] est rejete (code 2)");
    CHECK(decrypted == NULL, "aucun texte n'est renvoye pour un memory_cost_kib invalide");

    secure_free(ciphertext);
}

/* ---- Étape 3 : non-régression sur l'historique d'anomalies ----
 * Seule bin_to_hex() vit entièrement dans Cryptage_Core.c et se prête à
 * un test headless sans complication. Les autres anomalies historiques
 * (EN_CHANGE/MainWndProc, CreateThread+update_buttons, detect_file_type,
 * MAX_CRYPT_SIZE dans handle_decrypt) sont dans Cryptage_UI.c/UI_Common.c :
 * les deux premières exigent une vraie fenêtre (message Win32), les deux
 * autres entreraient en collision avec nos stubs si on liait
 * Cryptage_UI_Common.c directement. Elles sont reportées à la phase
 * "tests d'intégration UI" déjà prévue au programme. */

static void test_bin_to_hex_null_on_alloc_failure(void) {
    fprintf(stderr, "-- test_bin_to_hex_null_on_alloc_failure --\n");
    fprintf(stderr, "   (regression : en V38.0.3, le refactor force_lock avait fait\n"
                     "    disparaitre ce controle - restaure depuis par le commit b2b789a)\n");

    /* bin_len enorme => buffer_size (bin_len*3 + ...) largement au-dela de
     * tout espace d'adressage reel => secure_malloc/VirtualAlloc echoue.
     * Le pointeur bin=NULL n'est jamais dereference si bin_to_hex renvoie
     * bien NULL avant la boucle de conversion, comme attendu. */
    char* result = bin_to_hex(NULL, (size_t)1 << 60, FALSE);
    CHECK(result == NULL,
          "bin_to_hex renvoie NULL (et ne plante pas) si l'allocation echoue");
}

/* ---- Étape 4 : mutation systématique de l'en-tête (fuzzing léger) ----
 * Pas d'AFL++/libFuzzer à ce stade (toolchain supplémentaire, pas
 * forcément disponible sous MSYS2 MinGW64) : on altère ici chaque octet
 * des 84 premiers un par un et on vérifie l'absence de plantage plus un
 * code retour documenté (0 à 5) à chaque fois. Mémoire Argon2id réduite
 * à 4096 KiB (le minimum accepté) uniquement pour la vitesse du test :
 * la plupart des octets (salt/nonce/tag/reserve) ne sont pas filtrés par
 * les contrôles rapides (version/taille/mem_kib) et déclenchent donc une
 * vraie dérivation de clé à chaque itération. */

#define MUTATION_TEST_MEM_KIB 4096u

static void test_header_mutation_never_crashes(void) {
    fprintf(stderr, "-- test_header_mutation_never_crashes --\n");
    fprintf(stderr, "   (84 mutations, chacune avec derivation Argon2id reduite a 4 Mo :\n"
                     "    peut prendre plusieurs dizaines de secondes)\n");

    const char* plaintext = "Texte de reference pour mutation";
    const char* password = "MotDePasse_Mutation!1";
    size_t ciphertext_len = 0;

    unsigned char* original = encrypt_data(NULL,
        (const unsigned char*)plaintext, strlen(plaintext),
        password, &ciphertext_len, MUTATION_TEST_MEM_KIB);
    CHECK(original != NULL, "encrypt_data reussit (preparation)");
    if (!original) return;

    int unexpected_codes = 0;
    int successes_on_mutation = 0;

    for (size_t i = 0; i < MIN_ENCRYPTED_SIZE; i++) {
        unsigned char* mutated = (unsigned char*)malloc(ciphertext_len);
        memcpy(mutated, original, ciphertext_len);
        mutated[i] ^= 0xFF; /* inversion complete de l'octet i */

        unsigned char* decrypted = NULL;
        size_t decrypted_len = 0;
        int rc = decrypt_data(NULL, mutated, ciphertext_len, password,
            &decrypted, &decrypted_len, MUTATION_TEST_MEM_KIB);

        if (rc < 0 || rc > 5) {
            unexpected_codes++;
            fprintf(stderr, "   octet %zu : code INATTENDU %d\n", (size_t)i, rc);
        }
        if (rc == 0) {
            successes_on_mutation++;
            fprintf(stderr, "   octet %zu : dechiffrement REUSSI malgre la mutation (a examiner)\n", (size_t)i);
        }

        secure_free(decrypted);
        free(mutated);
    }

    CHECK(unexpected_codes == 0,
          "chaque octet mute produit un code documente (0 a 5), jamais autre chose");
    CHECK(successes_on_mutation == 0,
          "aucune mutation d'un octet d'en-tete ne produit un dechiffrement reussi");

    secure_free(original);
}

/* ---- Étape 4bis : hex_to_bin face à une entrée malformée ----
 * hex_to_bin() traite directement ce que l'utilisateur colle dans la
 * zone Entrée pour un déchiffrement sans fichier - la vraie surface
 * d'attaque "utilisateur", au sens propre. D'après le code (deux seuls
 * codes retour : 0 succès, -1 tout le reste), aucune des branches
 * d'echec ne touche *output/*out_len : on le verifie via des valeurs
 * sentinelles. */

static void test_hex_to_bin_malformed_input(void) {
    fprintf(stderr, "-- test_hex_to_bin_malformed_input --\n");

    unsigned char* out;
    size_t out_len;
    int rc;

    /* Caractere non-hexadecimal */
    out = (unsigned char*)0x1; out_len = 999;
    rc = hex_to_bin("12G4", &out, &out_len, FALSE);
    CHECK(rc == -1, "caractere non-hexadecimal ('G') rejete (-1)");
    CHECK(out == (unsigned char*)0x1 && out_len == 999,
          "sortie non modifiee sur ce rejet");

    /* Nombre impair de chiffres hexadecimaux */
    out = (unsigned char*)0x1; out_len = 999;
    rc = hex_to_bin("ABC", &out, &out_len, FALSE);
    CHECK(rc == -1, "nombre impair de chiffres hexadecimaux rejete (-1)");
    CHECK(out == (unsigned char*)0x1 && out_len == 999,
          "sortie non modifiee sur ce rejet");

    /* Pointeur d'entree NULL */
    out = (unsigned char*)0x1; out_len = 999;
    rc = hex_to_bin(NULL, &out, &out_len, FALSE);
    CHECK(rc == -1, "entree NULL rejetee (-1)");

    /* Chaine vide : cas documente comme un succes a 0 octet, pas une erreur */
    out = (unsigned char*)0x1; out_len = 999;
    rc = hex_to_bin("", &out, &out_len, FALSE);
    CHECK(rc == 0, "chaine vide acceptee (0 octet, ce n'est pas une erreur)");
    CHECK(out == NULL && out_len == 0,
          "sortie mise a NULL/0 pour une chaine vide");

    /* Chaine composee uniquement d'espaces : meme cas que la chaine vide */
    out = (unsigned char*)0x1; out_len = 999;
    rc = hex_to_bin("   \t\n  ", &out, &out_len, FALSE);
    CHECK(rc == 0, "chaine uniquement composee d'espaces acceptee (0 octet)");
    CHECK(out == NULL && out_len == 0,
          "sortie mise a NULL/0 pour une chaine d'espaces");

    /* Ponctuation quelconque (paste accidentel) */
    out = (unsigned char*)0x1; out_len = 999;
    rc = hex_to_bin("!@#$", &out, &out_len, FALSE);
    CHECK(rc == -1, "ponctuation quelconque rejetee (-1)");
}

/* ---- Étape 4ter : is_password_strong ---- */

static void test_is_password_strong(void) {
    fprintf(stderr, "-- test_is_password_strong --\n");

    CHECK(is_password_strong("Abc123!@") != 0,
          "majuscule+minuscule+chiffre+symbole (8 car.) accepte");
    CHECK(is_password_strong("Abcdefg1") == 0, "sans symbole, rejete");
    CHECK(is_password_strong("abcdefg!") == 0, "sans majuscule, rejete");
    CHECK(is_password_strong("ABCDEFG!") == 0, "sans minuscule, rejete");
    CHECK(is_password_strong("Abcdefg!") == 0, "sans chiffre, rejete");
    CHECK(is_password_strong("Ab1!") == 0, "trop court (< 8 caracteres), rejete");

    char exactly_max[MAX_PASSWORD_LEN + 1];
    memset(exactly_max, 'a', MAX_PASSWORD_LEN);
    exactly_max[0] = 'A'; exactly_max[1] = '1'; exactly_max[2] = '!';
    exactly_max[MAX_PASSWORD_LEN] = '\0';
    CHECK(is_password_strong(exactly_max) != 0,
          "exactement MAX_PASSWORD_LEN (64) caracteres, accepte");

    char too_long[MAX_PASSWORD_LEN + 2];
    memset(too_long, 'a', MAX_PASSWORD_LEN + 1);
    too_long[0] = 'A'; too_long[1] = '1'; too_long[2] = '!';
    too_long[MAX_PASSWORD_LEN + 1] = '\0';
    CHECK(is_password_strong(too_long) == 0,
          "MAX_PASSWORD_LEN + 1 caracteres, rejete");

    /* Limitation documentee (README, depuis V38.0.1) : isupper/islower/
     * ispunct ne reconnaissent que l'ASCII en locale C (jamais changee
     * via setlocale dans ce projet). "É" (0xC3 0x89 en UTF-8) n'est donc
     * jamais compte comme majuscule. Comportement CONNU et volontaire,
     * pas un defaut - on le fige pour etre alerte s'il change un jour. */
    CHECK(is_password_strong("\xC3\x89" "cbdef1!") == 0,
          "[limitation documentee UTF-8] 'E' accentue non reconnu comme majuscule");
}

int main(void) {
    test_hex_roundtrip();
    test_encrypt_decrypt_roundtrip();
    test_wrong_password_rejected();

    test_empty_plaintext();
    test_max_plaintext_size_accepted();
    test_plaintext_too_large_rejected();
    test_tampered_tag_rejected();
    test_truncated_header_rejected();
    test_wrong_version_rejected();
    test_invalid_mem_kib_rejected();

    test_bin_to_hex_null_on_alloc_failure();

    test_header_mutation_never_crashes();

    test_hex_to_bin_malformed_input();

    test_is_password_strong();

    fprintf(stderr, "\n%d verification(s), %d echec(s)\n", tests_run, tests_failed);
    return tests_failed == 0 ? 0 : 1;
}
