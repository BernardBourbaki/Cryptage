/**
 * tests/test_ui_wraps.c
 * Utilise -Wl,--wrap (voir la commande de compilation fournie) pour
 * intercepter show_error()/show_success()/display_openssl_error() sans
 * entrer en collision avec les vraies définitions de
 * Cryptage_UI_Common.c - contrairement au harnais du noyau seul,
 * cette fois on a besoin de lier Cryptage_UI.c ET Cryptage_UI_Common.c
 * pour de vrai (MainWndProc, detect_file_type, create_ui_controls...),
 * donc plus question de fournir nos propres définitions directes de ces
 * trois fonctions comme dans tests/test_stubs.c : ça entrerait en
 * collision avec les vraies. --wrap redirige tous les appels vers nos
 * versions __wrap_*, quel que soit le fichier appelant.
 */

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

void __wrap_show_error(HWND hwnd, const char* message, const char* title) {
    (void)hwnd;
    fprintf(stderr, "  (show_error intercepte) [%s] %s\n", title, message);
}

void __wrap_show_success(HWND hwnd, const char* message, const char* title) {
    (void)hwnd;
    fprintf(stderr, "  (show_success intercepte) [%s] %s\n", title, message);
}

void __wrap_display_openssl_error(HWND hwnd, const char* operation) {
    (void)hwnd;
    fprintf(stderr, "  (display_openssl_error intercepte) %s\n", operation);
}

/* ---- Echec controle de la creation de thread (test de regression) ----
 * --wrap=CreateThread ne fonctionne pas de facon fiable sur une fonction
 * importee d'une DLL systeme (kernel32) - confirme empiriquement (le
 * thread reel etait toujours cree). Technique de repli, plus fiable :
 * Cryptage_UI.c est compile a part avec -DCreateThread=Test_CreateThread
 * (voir la commande de compilation), qui substitue purement et
 * simplement le nom au niveau du preprocesseur, dans CE fichier
 * uniquement. Cryptage_UI.c appelle donc en realite Test_CreateThread,
 * definie ci-dessous - qui, elle, n'a pas ce -D et peut donc appeler la
 * vraie CreateThread normalement quand on ne simule pas d'echec. */

static BOOL g_force_create_thread_failure = FALSE;

void set_force_create_thread_failure(BOOL v) {
    g_force_create_thread_failure = v;
}

static HANDLE WINAPI fake_create_thread(LPSECURITY_ATTRIBUTES lpThreadAttributes,
    SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId) {
    if (g_force_create_thread_failure) {
        fprintf(stderr, "  (CreateThread intercepte : echec simule)\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    return CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress,
        lpParameter, dwCreationFlags, lpThreadId);
}

/* Cryptage_UI.c (compile avec -DCreateThread=Test_CreateThread) genere
 * un appel INDIRECT via __imp_Test_CreateThread, exactement comme pour
 * toute fonction declaree __declspec(dllimport) dans windows.h - le nom
 * substitue herite du meme traitement (confirme par l'erreur de
 * l'edition de liens : "undefined reference to __imp_Test_CreateThread").
 * On fournit donc nous-memes cette variable-pointeur, avec l'adresse de
 * notre fonction dedans - exactement ce que ferait un vrai import de DLL. */
typedef HANDLE (WINAPI *CreateThreadFn)(LPSECURITY_ATTRIBUTES, SIZE_T,
    LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
CreateThreadFn __imp_Test_CreateThread = fake_create_thread;
