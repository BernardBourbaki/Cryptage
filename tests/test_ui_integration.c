/**
 * tests/test_ui_integration.c
 * Tests d'intégration UI - étape 1 : EN_CHANGE.
 *
 * Contrairement au harnais du noyau (tests/test_crypto_core.c), ces
 * tests créent une vraie fenêtre Win32 (via la vraie MainWndProc de
 * Cryptage_UI.c) et lui envoient de vrais messages Win32 - pas
 * d'outillage externe (pywinauto, AutoIt...), juste l'API Win32 telle
 * que le programme lui-même l'utilise.
 *
 * Important : on n'utilise PAS create_main_window() ni g_AppContext de
 * Cryptage_Main.c (g_AppContext y est `static`, donc inaccessible d'ici,
 * et c'est très bien ainsi : aucune modification du code de production
 * n'est nécessaire). On enregistre nous-mêmes la classe de fenêtre et on
 * crée la fenêtre avec notre propre AppContext local - MainWndProc et
 * create_ui_controls n'y voient aucune différence.
 *
 * La fenêtre n'est jamais affichée (pas de ShowWindow) : WM_CREATE part
 * de façon synchrone pendant CreateWindowExA, donc tous les contrôles
 * existent déjà quand la fonction retourne, sans jamais rien afficher à
 * l'écran.
 */

#include "Cryptage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ID_INPUT_EDIT est defini directement dans Cryptage_UI.c
 * (#define ID_INPUT_EDIT 1003), pas expose via Cryptage.h. Redefini ici
 * a l'identique - a mettre a jour si jamais cette valeur change la-bas. */
#define ID_INPUT_EDIT 1003

/* Definie dans test_ui_wraps.c */
extern void set_force_create_thread_failure(BOOL v);

/* handle_encrypt/handle_decrypt/handle_import ne sont pas static dans
 * Cryptage_UI.c (donc appelables d'ici), mais leur prototype n'est
 * declare qu'a l'interieur de ce fichier - pas expose via Cryptage.h,
 * contrairement a MainWndProc/create_ui_controls/detect_file_type. */
void handle_encrypt(HWND hwnd, AppContext* ctx);
void handle_decrypt(HWND hwnd, AppContext* ctx);
void handle_import(HWND hwnd, AppContext* ctx);

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

static AppContext ui_ctx;

static HWND create_test_window(void) {
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEXA wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "CryptoMainClass";
    RegisterClassExA(&wc); /* si deja enregistree, l'echec est sans consequence */

    memset(&ui_ctx, 0, sizeof(AppContext));

    return CreateWindowExA(0, "CryptoMainClass", "Test Cryptage",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 900, 830,
        NULL, NULL, hInstance, &ui_ctx);
}

static void test_en_change_invalidates_encrypted(void) {
    fprintf(stderr, "-- test_en_change_invalidates_encrypted --\n");
    fprintf(stderr, "   (regression : correctif du commit ac6610b)\n");

    HWND hwnd = create_test_window();
    CHECK(hwnd != NULL, "fenetre de test creee");
    if (!hwnd) return;

    CHECK(ui_ctx.hInputEdit != NULL, "le controle Entree existe (WM_CREATE a bien tourne)");

    /* Simule l'etat "vient de chiffrer du texte" */
    ui_ctx.state.encrypted = TRUE;
    ui_ctx.state.file_type = FILE_TYPE_NONE; /* texte, ni image ni .crypt */

    /* Simule le message EN_CHANGE que Windows enverrait si l'utilisateur
     * modifiait le contenu de la zone Entree - on construit nous-memes
     * exactement le message que le vrai controle enverrait, pour tester
     * la logique de MainWndProc plutot que le comportement (deja garanti
     * par Windows) du controle Edit lui-meme. */
    SendMessageA(hwnd, WM_COMMAND, MAKEWPARAM(ID_INPUT_EDIT, EN_CHANGE),
                 (LPARAM)ui_ctx.hInputEdit);

    CHECK(ui_ctx.state.encrypted == FALSE,
          "EN_CHANGE invalide bien l'etat encrypted");

    /* Cas de garde associe : si le fichier est une image ou un .crypt
     * deja importe, EN_CHANGE ne doit PAS invalider encrypted (ce n'est
     * pas le contenu du champ Entree qui a produit ce resultat). */
    ui_ctx.state.encrypted = TRUE;
    ui_ctx.state.file_type = FILE_TYPE_IMAGE;
    SendMessageA(hwnd, WM_COMMAND, MAKEWPARAM(ID_INPUT_EDIT, EN_CHANGE),
                 (LPARAM)ui_ctx.hInputEdit);
    CHECK(ui_ctx.state.encrypted == TRUE,
          "EN_CHANGE ne touche pas a encrypted quand file_type == FILE_TYPE_IMAGE");

    DestroyWindow(hwnd);
}

/* ---- detect_file_type ----
 * Fonction pure (ne depend d'aucun controle Win32 reel) : pas besoin de
 * fenetre de test, juste un AppContext local zero. On verifie a la fois
 * la detection de chaque format et l'invariant du correctif V38.0.6 :
 * original_extension_len ne doit JAMAIS valoir 4 si original_extension
 * est NULL (voir le commentaire du code source a ce sujet).
 *
 * Non teste ici : le cas ou dup_extension()/secure_malloc echoue
 * reellement (original_extension resterait NULL, len passerait a 0).
 * dup_extension() est static dans Cryptage_UI_Common.c : --wrap n'a
 * aucun effet sur un appel resolu a l'interieur d'un seul fichier objet,
 * et la substitution -D de secure_malloc affecterait aussi tous les
 * autres appels de ce fichier. Vu la probabilite quasi nulle d'un echec
 * d'allocation de 4 octets, le rapport effort/valeur ne le justifie pas
 * ici (contrairement a CreateThread, dont l'echec est un scenario
 * realiste sous charge).
 */

static void test_detect_file_type(void) {
    fprintf(stderr, "-- test_detect_file_type --\n");

    AppContext local_ctx;

    /* JPEG */
    memset(&local_ctx, 0, sizeof(AppContext));
    {
        unsigned char jpeg[16] = { 0xFF, 0xD8, 0xFF, 0xE0, 0,0,0,0,0,0,0,0,0,0,0,0 };
        FileType t = detect_file_type(jpeg, sizeof(jpeg), &local_ctx);
        CHECK(t == FILE_TYPE_IMAGE, "JPEG detecte comme FILE_TYPE_IMAGE");
        CHECK(local_ctx.state.original_extension != NULL &&
              strcmp(local_ctx.state.original_extension, "jpg") == 0,
              "extension 'jpg' correctement enregistree");
        CHECK(local_ctx.state.original_extension_len == 4,
              "original_extension_len == 4 (coherent avec le pointeur non-NULL)");
    }

    /* PNG */
    memset(&local_ctx, 0, sizeof(AppContext));
    {
        unsigned char png[16] = { 0x89,'P','N','G','\r','\n',0x1A,'\n', 0,0,0,0,0,0,0,0 };
        FileType t = detect_file_type(png, sizeof(png), &local_ctx);
        CHECK(t == FILE_TYPE_IMAGE, "PNG detecte comme FILE_TYPE_IMAGE");
        CHECK(local_ctx.state.original_extension != NULL &&
              strcmp(local_ctx.state.original_extension, "png") == 0,
              "extension 'png' correctement enregistree");
        CHECK(local_ctx.state.original_extension_len == 4,
              "original_extension_len == 4 (coherent avec le pointeur non-NULL)");
    }

    /* BMP */
    memset(&local_ctx, 0, sizeof(AppContext));
    {
        unsigned char bmp[16] = { 'B','M', 0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
        FileType t = detect_file_type(bmp, sizeof(bmp), &local_ctx);
        CHECK(t == FILE_TYPE_IMAGE, "BMP detecte comme FILE_TYPE_IMAGE");
        CHECK(local_ctx.state.original_extension != NULL &&
              strcmp(local_ctx.state.original_extension, "bmp") == 0,
              "extension 'bmp' correctement enregistree");
        CHECK(local_ctx.state.original_extension_len == 4,
              "original_extension_len == 4 (coherent avec le pointeur non-NULL)");
    }

    /* .crypt valide (version 370, mem_kib dans les bornes) */
    memset(&local_ctx, 0, sizeof(AppContext));
    {
        unsigned char header[MIN_ENCRYPTED_SIZE];
        memset(header, 0, sizeof(header));
        write_uint32_le(header, CURRENT_VERSION);
        write_uint32_le(header + 20, 16384u);
        FileType t = detect_file_type(header, sizeof(header), &local_ctx);
        CHECK(t == FILE_TYPE_CRYPT, ".crypt valide detecte comme FILE_TYPE_CRYPT");
        CHECK(local_ctx.state.mem_kib == 16384u,
              "mem_kib recupere depuis l'en-tete .crypt");
    }

    /* Texte brut */
    memset(&local_ctx, 0, sizeof(AppContext));
    {
        const char* text = "Ceci est un texte tout a fait normal.\n";
        FileType t = detect_file_type((const unsigned char*)text, strlen(text), &local_ctx);
        CHECK(t == FILE_TYPE_TEXT, "texte brut detecte comme FILE_TYPE_TEXT");
    }

    /* Binaire non reconnu (aucune signature, octet de controle non autorise) */
    memset(&local_ctx, 0, sizeof(AppContext));
    {
        unsigned char garbage[8] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
        FileType t = detect_file_type(garbage, sizeof(garbage), &local_ctx);
        CHECK(t == FILE_TYPE_NONE, "binaire non reconnu classe FILE_TYPE_NONE");
    }

    /* Entree NULL / vide */
    memset(&local_ctx, 0, sizeof(AppContext));
    CHECK(detect_file_type(NULL, 0, &local_ctx) == FILE_TYPE_NONE,
          "donnees NULL classees FILE_TYPE_NONE");
    CHECK(detect_file_type((const unsigned char*)"x", 0, &local_ctx) == FILE_TYPE_NONE,
          "longueur 0 classee FILE_TYPE_NONE");
}

static void test_createthread_failure_reenables_ui(void) {
    fprintf(stderr, "-- test_createthread_failure_reenables_ui --\n");
    fprintf(stderr, "   (regression : correctif du commit ac6610b -\n"
                     "    CreateThread force en echec via --wrap)\n");

    HWND hwnd = create_test_window();
    CHECK(hwnd != NULL, "fenetre de test creee");
    if (!hwnd) return;

    SetWindowTextA(ui_ctx.hKeyEdit, "MotDePasse_Thread!1");
    SetWindowTextA(ui_ctx.hInputEdit, "Texte a chiffrer pour ce test");

    update_buttons(&ui_ctx);
    CHECK(IsWindowEnabled(ui_ctx.hEncryptBtn),
          "bouton Chiffrer actif avant l'operation");

    set_force_create_thread_failure(TRUE);
    handle_encrypt(hwnd, &ui_ctx);
    set_force_create_thread_failure(FALSE);

    CHECK(ui_ctx.state.operation_in_progress == FALSE,
          "operation_in_progress repasse a FALSE apres l'echec de CreateThread");
    CHECK(IsWindowEnabled(ui_ctx.hEncryptBtn),
          "bouton Chiffrer reactive apres l'echec de CreateThread (pas gele)");

    DestroyWindow(hwnd);
}

static void test_handle_decrypt_max_crypt_size_rejected(void) {
    fprintf(stderr, "-- test_handle_decrypt_max_crypt_size_rejected --\n");
    fprintf(stderr, "   (regression : correctif du commit 9aaec72 - le controle\n"
                     "    MAX_CRYPT_SIZE etait code mort, imbrique apres un return ;\n"
                     "    ~30 Mo de texte hexadecimal a construire, quelques secondes)\n");

    HWND hwnd = create_test_window();
    CHECK(hwnd != NULL, "fenetre de test creee");
    if (!hwnd) return;

    /* Contenu binaire arbitraire de MAX_CRYPT_SIZE + 1 octets : seule la
     * taille compte pour ce controle, qui intervient avant toute lecture
     * de version/mem_kib dans l'en-tete. */
    size_t size = MAX_CRYPT_SIZE + 1;
    unsigned char* bin = (unsigned char*)malloc(size);
    CHECK(bin != NULL, "allocation du buffer de test (10 Mo + 84 + 1 octets)");
    if (!bin) { DestroyWindow(hwnd); return; }
    memset(bin, 0xAB, size);

    char* hex = bin_to_hex(bin, size, FALSE);
    CHECK(hex != NULL, "conversion en hexadecimal reussie");
    free(bin);
    if (!hex) { DestroyWindow(hwnd); return; }

    SetWindowTextA(ui_ctx.hInputEdit, hex);
    secure_free(hex);

    ui_ctx.state.file_type = FILE_TYPE_NONE; /* force le chemin "hex colle", pas .crypt importe */
    ui_ctx.state.loaded_data = NULL;

    handle_decrypt(hwnd, &ui_ctx);

    CHECK(ui_ctx.state.decrypt_attempt_failed == TRUE,
          "le depassement de MAX_CRYPT_SIZE est rejete (le controle n'est plus du code mort)");

    DestroyWindow(hwnd);
}

int main(void) {
    test_en_change_invalidates_encrypted();
    test_createthread_failure_reenables_ui();
    test_detect_file_type();
    test_handle_decrypt_max_crypt_size_rejected();

    fprintf(stderr, "\n%d verification(s), %d echec(s)\n", tests_run, tests_failed);
    return tests_failed == 0 ? 0 : 1;
}
