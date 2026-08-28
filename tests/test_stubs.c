/**
 * tests/test_stubs.c
 * Stubs de remplacement pour les fonctions d'affichage Win32 de Cryptage.
 *
 * Pourquoi ce fichier est nécessaire (et pas seulement pratique) :
 * Cryptage_Core.c appelle show_error()/show_success()/display_openssl_error()
 * sur plusieurs chemins d'erreur. Ces fonctions sont normalement définies
 * dans Cryptage_UI_Common.c et affichent une vraie MessageBoxA Win32.
 * Or au moins un appel (encrypt_data, cas plaintext_len > MAX_PLAINTEXT_SIZE)
 * appelle show_error(hwnd, ...) SANS jamais vérifier hwnd != NULL au
 * préalable. MessageBoxA(NULL, ...) reste un appel valide côté Win32 : il
 * affiche une boîte de dialogue modale sans propriétaire, qui bloquerait
 * indéfiniment un test automatisé/CI en attendant un clic humain.
 *
 * On ne lie donc jamais Cryptage_UI_Common.c au harnais de test : on
 * fournit ici nos propres définitions, silencieuses (trace sur stderr
 * uniquement), pour ces trois symboles.
 */

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

void show_error(HWND hwnd, const char* message, const char* title) {
    (void)hwnd;
    fprintf(stderr, "  (show_error ignoré en test) [%s] %s\n", title, message);
}

void show_success(HWND hwnd, const char* message, const char* title) {
    (void)hwnd;
    fprintf(stderr, "  (show_success ignoré en test) [%s] %s\n", title, message);
}

void display_openssl_error(HWND hwnd, const char* operation) {
    (void)hwnd;
    fprintf(stderr, "  (display_openssl_error ignoré en test) %s\n", operation);
}
