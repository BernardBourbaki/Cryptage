/**
 * Cryptage.h
 * Header principal - Version 3803
 * (c) Bernard DÉMARET - 2026
 */

#ifndef CRYPTAGE_H
#define CRYPTAGE_H

/* ========================================
 * INCLUDES SYSTÈME
 * ======================================== */

#include <winsock2.h>  // IMPORTANT : avant windows.h
#include <windows.h>
#include <wincrypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/kdf.h>
#include <openssl/core_names.h>  // V38.0.2 : constantes OSSL_KDF_PARAM_* pour Argon2id

/* ========================================
 * CONSTANTES CRYPTOGRAPHIQUES
 * ======================================== */

// Tailles des éléments cryptographiques
#define SALT_LEN 32         // Longueur du sel Argon2id
#define NONCE_LEN 12        // Longueur du nonce AES-GCM
#define TAG_LEN 16          // Longueur du tag d'authentification
#define KEY_LEN 32          // Longueur de la clé AES-256

// Paramètres Argon2id
#define DEFAULT_MEMORY_COST_KIB 16384  // 16 Mo par défaut
#define TIME_COST 3                    // 3 itérations
#define PARALLELISM 1                  // 1 thread

// Compatibilité noms alternatifs (pour Cryptage_Core.c)
#define ARGON2_T_COST TIME_COST
#define ARGON2_PARALLELISM PARALLELISM
#define DERIVED_KEY_LEN KEY_LEN

// Longueur des données additionnelles authentifiées (AAD)
#define AAD_LEN 24  // Version(4) + Reserved(16) + MemKiB(4)

// Offsets dans l'AAD
#define VERSION_OFFSET 0          // Offset de la version
#define PLAINTEXT_LEN_OFFSET 16   // Offset de la longueur du plaintext
#define MEMORY_OFFSET 20          // Offset du paramètre mémoire
// Note : offsets 4-15 sont réservés (zéros), non utilisés à ce jour

// Codes d'extension d'images (dans la zone réservée AAD)
#define EXT_NONE 0
#define EXT_JPG 1
#define EXT_PNG 2
#define EXT_BMP 3

// Formatage hexadécimal
#define HEX_COLUMNS 16  // 16 octets par ligne

// Limites
#define MAX_PASSWORD_LEN 64
#define MAX_TEXT_LEN (10 * 1024 * 1024)  // 10 Mo

// V37.3 : limite explicite du texte en clair (alias de MAX_TEXT_LEN, conservé
// pour compatibilité), et limite distincte pour le fichier .crypt sur disque,
// qui ajoute 84 octets d'en-tête. Avant ce correctif, un fichier .crypt issu
// d'un texte de 10 Mo pile ne pouvait plus être réimporté par le programme
// qui venait de le créer - load_file_secure() appliquait la même limite que
// encrypt_data() sans tenir compte de cet en-tête.
#define MAX_PLAINTEXT_SIZE MAX_TEXT_LEN
#define CRYPT_OVERHEAD (AAD_LEN + SALT_LEN + NONCE_LEN + TAG_LEN)
#define MAX_CRYPT_SIZE (MAX_PLAINTEXT_SIZE + CRYPT_OVERHEAD)

// V37.3 : taille des buffers de chemin de fichier (UTF-8). 260 (MAX_PATH en
// unités UTF-16) ne suffit plus une fois converti en UTF-8, où un caractère
// accentué peut occuper jusqu'à 4 octets contre 2 en UTF-16.
#define MAX_FILENAME_BUFFER 1024

/* ========================================
 * STRUCTURE DES DONNÉES CHIFFRÉES V37
 * ======================================== */

/**
 * Format du fichier crypté :
 *
 * [AAD - 24 octets]
 *   - Version (4 octets, little-endian) : 370
 *   - Réservé (12 octets) : extensibilité future, initialisés à zéro
 *   - Longueur du plaintext (4 octets, little-endian)
 *   - MemKiB (4 octets, little-endian) : paramètre mémoire Argon2id
 *
 * [SALT - 32 octets]
 *   - Sel aléatoire pour Argon2id
 *
 * [NONCE - 12 octets]
 *   - Nonce aléatoire pour AES-GCM
 *
 * [TAG - 16 octets]
 *   - Tag d'authentification AES-GCM
 *
 * [CIPHERTEXT - longueur variable]
 *   - Données chiffrées
 */

#define CURRENT_VERSION 370  // Format inchangé V37 → V38
#define VERSION CURRENT_VERSION

/* ========================================
 * MESSAGES WINDOWS PERSONNALISÉS
 * ======================================== */

#define WM_USER_PROGRESS (WM_USER + 1)
#define WM_USER_COMPLETE (WM_USER + 2)

/* ========================================
 * INCLUDE DES STRUCTURES D'ÉTAT
 * ======================================== */

#include "Cryptage_State.h"

/* ========================================
 * DÉCLARATIONS DES FONCTIONS CRYPTOGRAPHIQUES
 * (Cryptage_Core.c)
 * ======================================== */

/**
 * Initialise OpenSSL en mode portable
 */
BOOL init_portable_openssl(void);

/**
 * Chiffre des données avec AES-256-GCM + Argon2id
 */
unsigned char* encrypt_data(HWND hwnd, const unsigned char* plaintext,
    size_t plaintext_len, const char* password,
    size_t* ciphertext_len, unsigned int mem_kib);

/**
 * Déchiffre des données avec AES-256-GCM + Argon2id
 *
 * @return 0 en cas de succès, 1 si mot de passe incorrect, -1 en cas d'erreur
 */
int decrypt_data(HWND hwnd, const unsigned char* ciphertext,
    size_t ciphertext_len, const char* password,
    unsigned char** plaintext, size_t* plaintext_len,
    unsigned int mem_kib);

/* ========================================
 * UTILITAIRES CRYPTOGRAPHIQUES
 * ======================================== */

/**
 * Vérifie la robustesse d'un mot de passe
 * Critères : 8-64 caractères, maj+min+chiffre+symbole
 *
 * NOTE V38.0.1 : cette fonction analyse les octets individuellement
 * via les fonctions C standard isupper/islower/isdigit/ispunct, qui
 * ne reconnaissent correctement que les caractères ASCII (0-127).
 * Les mots de passe UTF-8 multi-octets (cyrillique, CJK, etc.) peuvent
 * être faussement rejetés comme "faibles" même s'ils satisfont les
 * critères sémantiques. L'usage de gestionnaires de mots de passe
 * générant du ASCII (KeePass, Bitwarden, etc.) est recommandé.
 */
BOOL is_password_strong(const char* password);

/**
 * Lit un entier 32 bits en little-endian
 */
uint32_t read_uint32_le(const unsigned char* buf);

/**
 * Écrit un entier 32 bits en little-endian
 */
void write_uint32_le(unsigned char* buf, uint32_t value);

/* ========================================
 * GESTION MÉMOIRE SÉCURISÉE
 * (Cryptage_Core.c)
 * ======================================== */

/**
 * Initialise le système de gestion mémoire sécurisée
 */
void secure_mem_init(void);

/**
 * Nettoie le système de gestion mémoire sécurisée
 */
void secure_mem_cleanup(void);

/**
 * Alloue de la mémoire sécurisée (non swappable)
 */
void* secure_malloc(HWND hwnd, size_t size, BOOL force_lock);

/**
 * Libère de la mémoire sécurisée
 */
void secure_free(void* ptr);

/**
 * Efface puis libère de la mémoire sécurisée
 */
void secure_clean_and_free(void* ptr, size_t size);

/**
 * Récupère le texte d'un contrôle Edit de manière sécurisée
 */
char* secure_get_edit_text(HWND hEdit, HWND hwnd, const char* error_title,
    size_t max_len);

/**
 * Définit le texte d'un contrôle Edit de manière sécurisée
 */
void secure_set_edit_text(HWND hEdit, const char* text, size_t text_len);

/* ========================================
 * CONVERSION HEXADÉCIMAL
 * (Cryptage_Core.c)
 * ======================================== */

/**
 * Convertit des données binaires en hexadécimal
 */
char* bin_to_hex(const unsigned char* bin, size_t bin_len, BOOL force_lock);

/**
 * Convertit une chaîne hexadécimale en binaire
 */
int hex_to_bin(const char* hex, unsigned char** bin, size_t* bin_len, BOOL force_lock);

/**
 * Vérifie si une chaîne est hexadécimale valide
 */
BOOL is_valid_hex(const char* hex);

/* ========================================
 * GESTION DES FICHIERS
 * (Cryptage_Core.c)
 * ======================================== */

/**
 * Charge un fichier de manière sécurisée
 */
BOOL load_file_secure(const char* filename, unsigned char** data,
    size_t* len, HWND hwnd, BOOL text_mode);

/**
 * Vérifie les opérations sur fichiers
 */
BOOL check_file_operations(FILE* fp, const char* operation, HWND hwnd);

/**
 * V37.3 : Ouvre un fichier dont le chemin est fourni en UTF-8, en passant
 * par l'API Windows large (_wfopen) pour supporter tout chemin Unicode,
 * pas seulement ceux représentables dans la page de code ANSI du système.
 * Utilisée par load_file_secure() et par les fonctions save_*_file_secure()
 * (Cryptage_UI_Common.c) - un seul point de conversion UTF-8 -> UTF-16 pour
 * tout le programme, afin d'éviter toute reconversion accidentelle vers
 * une page de code à mi-parcours.
 */
FILE* fopen_utf8(const char* utf8_path, const char* mode);

/* ========================================
 * FONCTIONS UI COMMUNES
 * (Cryptage_UI_Common.c)
 * ======================================== */

// Messages
void show_error(HWND hwnd, const char* message, const char* title);
void show_success(HWND hwnd, const char* message, const char* title);
void display_openssl_error(HWND hwnd, const char* operation);

// Dialogues de fichiers
// V37.3 : filter/ext passent en wchar_t* (littéraux L"...") pour permettre
// l'affichage et la sélection de chemins Unicode via GetOpenFileNameW /
// GetSaveFileNameW. filename/filename_size restent en UTF-8 (char*), la
// conversion UTF-16 <-> UTF-8 est entièrement interne à cette fonction.
BOOL open_file_dialog(HWND hwnd, char* filename, size_t filename_size,
    const wchar_t* filter, const wchar_t* ext, BOOL save);

// Barre de progression
void update_progress_bar(HWND hwnd, AppContext* ctx, int percent);
void reset_progress_bar(AppContext* ctx);

// Mot de passe
void toggle_password_visibility(AppContext* ctx);

// Mémoire Argon2id
void update_memory_default(AppContext* ctx);
unsigned int get_memory_param(AppContext* ctx);

// Sauvegarde de fichiers
BOOL save_binary_file_secure(const char* filename, const unsigned char* data,
    size_t data_len, HWND hwnd);
BOOL save_decrypted_text_file_secure(const char* filename, HWND hOutputEdit);
BOOL save_image_file_secure(const char* filename, const unsigned char* data,
    size_t data_len, const char* extension, HWND hwnd);

// Détection de type
FileType detect_file_type(const unsigned char* data, size_t data_len,
    AppContext* ctx);

// Réinitialisation
void reset_decrypt_state(AppContext* ctx);
void handle_clear(AppContext* ctx);

// Opérations crypto
void cleanup_crypto_operation(CryptoOperation* op);

// Polices
void create_fonts(AppContext* ctx);
void destroy_fonts(AppContext* ctx);

/* ========================================
 * INTERFACE UTILISATEUR
 * (Cryptage_UI.c)
 * ======================================== */

/**
 * Procédure de fenêtre principale
 */
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/**
 * Crée les contrôles de l'interface
 */
void create_ui_controls(HWND hwnd, HINSTANCE hInstance, AppContext* ctx);

/**
 * Met à jour l'état des boutons
 */
void update_buttons(AppContext* ctx);

/* ========================================
 * REGISTRE DE MÉMOIRE SÉCURISÉE
 * ======================================== */

typedef struct SecureMemEntry {
    void* ptr;
    size_t size;
    struct SecureMemEntry* next;
} SecureMemEntry;

typedef SecureMemEntry SecureMemNode;

typedef struct {
    SecureMemEntry* head;
    CRITICAL_SECTION lock;
    BOOL initialized;
} SecureMemRegistry;

/* ========================================
 * MACROS UTILITAIRES
 * ======================================== */

#define SECURE_ZERO(ptr, size)     do {         if ((ptr) && (size) > 0) {             SecureZeroMemory((ptr), (size));         }     } while(0)

#define IS_VALID_PTR(ptr) ((ptr) != NULL)

#define MIN_ENCRYPTED_SIZE (AAD_LEN + SALT_LEN + NONCE_LEN + TAG_LEN)

/* ========================================
 * DÉCLARATIONS POUR LA COMPATIBILITÉ
 * ======================================== */

#ifdef __cplusplus
extern "C" {
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow);

#ifdef __cplusplus
}
#endif

#endif /* CRYPTAGE_H */
