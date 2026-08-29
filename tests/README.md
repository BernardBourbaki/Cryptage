# Tests automatisés - Cryptage

Deux harnais de tests, sans dépendance externe (pas de framework de test
tiers, pas d'outil d'automatisation UI type pywinauto/AutoIt) : uniquement
l'API Win32 et la chaîne de compilation déjà utilisées par le programme
lui-même.

## 1. Noyau cryptographique (`test_crypto_core.c` + `test_stubs.c`)

Teste `encrypt_data`, `decrypt_data`, `bin_to_hex`, `hex_to_bin`,
`is_password_strong` sans jamais lier de code UI.

### Compilation

```
gcc -I. -finput-charset=UTF-8 -fexec-charset=CP1252 tests/test_crypto_core.c tests/test_stubs.c Cryptage_Core.c -o tests/test_crypto.exe -lssl -lcrypto -luser32
```

### Exécution

```
tests\test_crypto.exe        (Invite de commandes)
./tests/test_crypto.exe      (MSYS2 MinGW64)
```

### Pourquoi test_stubs.c ?

`Cryptage_Core.c` appelle `show_error()`/`show_success()`/
`display_openssl_error()`, normalement définies dans
`Cryptage_UI_Common.c` et qui affichent une vraie `MessageBoxA` Win32. Au
moins un appel (`encrypt_data`, plaintext > 10 Mo) n'est pas gardé par un
test `hwnd != NULL`, et `MessageBoxA(NULL, ...)` reste un appel Win32
valide qui bloquerait un test automatisé en attendant un clic humain.
`test_stubs.c` fournit donc ses propres définitions silencieuses (trace
sur `stderr`) de ces trois fonctions ; ce harnais ne lie jamais
`Cryptage_UI_Common.c`.

### Couverture

- **Fondamentaux** : round-trip hex, round-trip chiffrement/déchiffrement, mot de passe incorrect.
- **Cas limites et négatifs** : texte vide, taille maximale (10 Mo) acceptée / rejetée au-delà, tag GCM falsifié, en-tête tronqué, version de format invalide, `memory_cost_kib` hors bornes.
- **Non-régression** : retour NULL de `secure_malloc` dans `bin_to_hex` (régression V38.0.3, corrigée par le commit `b2b789a`).
- **Fuzzing léger** : mutation systématique des 84 octets d'en-tête, entrée malformée pour `hex_to_bin`.
- **Validateur de mot de passe** : `is_password_strong` (bornes, 4 catégories de caractères, limitation UTF-8 documentée depuis la V38.0.1).

## 2. Intégration UI (`test_ui_integration.c` + `test_ui_wraps.c`)

Crée une vraie fenêtre Win32 (jamais affichée) via la vraie `MainWndProc`
de `Cryptage_UI.c`, et lui envoie de vrais messages Windows - ce qui
oblige à lier `Cryptage_UI.c` et `Cryptage_UI_Common.c` pour de vrai,
contrairement au harnais du noyau.

### Compilation (plusieurs étapes : fichiers objets séparés, puis édition de liens)

```
gcc -c -I. -finput-charset=UTF-8 -fexec-charset=CP1252 -DCreateThread=Test_CreateThread Cryptage_UI.c -o tests/Cryptage_UI.o
gcc -c -I. -finput-charset=UTF-8 -fexec-charset=CP1252 Cryptage_Core.c -o tests/Cryptage_Core.o
gcc -c -I. -finput-charset=UTF-8 -fexec-charset=CP1252 Cryptage_UI_Common.c -o tests/Cryptage_UI_Common.o
gcc -c -I. -finput-charset=UTF-8 -fexec-charset=CP1252 tests/test_ui_integration.c -o tests/test_ui_integration.o
gcc -c -I. -finput-charset=UTF-8 -fexec-charset=CP1252 tests/test_ui_wraps.c -o tests/test_ui_wraps.o
gcc tests/Cryptage_UI.o tests/Cryptage_Core.o tests/Cryptage_UI_Common.o tests/test_ui_integration.o tests/test_ui_wraps.o -o tests/test_ui.exe -lssl -lcrypto -luser32 -lgdi32 -lcomctl32 -lcomdlg32 -Wl,--wrap=show_error -Wl,--wrap=show_success -Wl,--wrap=display_openssl_error
```

### Exécution

```
tests\test_ui.exe        (Invite de commandes)
./tests/test_ui.exe      (MSYS2 MinGW64)
```

Aucune fenêtre ne doit apparaître à l'écran : la fenêtre existe (ses
contrôles sont créés normalement via `WM_CREATE`) mais n'est jamais
affichée.

### Deux techniques d'interception, pour deux raisons différentes

- **`-Wl,--wrap=show_error/show_success/display_openssl_error`** : ces
  trois fonctions sont réellement définies dans `Cryptage_UI_Common.c`
  (qu'on doit lier pour de vrai cette fois) - `--wrap` redirige tous les
  appels vers nos propres versions silencieuses, quel que soit le
  fichier appelant, sans dupliquer ni modifier le code source.
- **`-DCreateThread=Test_CreateThread`** (uniquement sur `Cryptage_UI.c`)
  : `--wrap` ne fonctionne pas de façon fiable sur `CreateThread`, une
  fonction importée d'une DLL système (kernel32) plutôt qu'une fonction
  du projet - confirmé empiriquement, le vrai thread était toujours créé
  malgré le wrap. La substitution de macro au niveau du préprocesseur,
  appliquée uniquement à la compilation de ce fichier, est plus fiable.
  Point technique à retenir : `Cryptage_UI.c` compilé ainsi cherche en
  réalité `__imp_Test_CreateThread` (une variable-pointeur, pas une
  fonction directe) - `Test_CreateThread` hérite du même traitement
  `__declspec(dllimport)` que `CreateThread` dans `windows.h`.
  `test_ui_wraps.c` fournit donc cette variable-pointeur, avec l'adresse
  de sa propre implémentation dedans.

### Couverture

Les 4 anomalies historiques restantes après le harnais du noyau, toutes
vérifiées corrigées (27/27) :

- **`EN_CHANGE`** (`MainWndProc`) : modifier l'Entrée après chiffrement invalide bien `ctx->state.encrypted` (sauf si `file_type` est image ou `.crypt`) - régression V38.0.3, corrigée par le commit `ac6610b`.
- **Échec de `CreateThread()`** (`handle_encrypt`) : `operation_in_progress` repasse à `FALSE` et le bouton Chiffrer se réactive, au lieu de rester gelé - même commit `ac6610b`.
- **`detect_file_type`** : détection correcte de JPEG/PNG/BMP/`.crypt`/texte/binaire non reconnu/entrée NULL, et invariant `original_extension_len` jamais à 4 si le pointeur est NULL - commit `088a68c`.
- **`MAX_CRYPT_SIZE` dans `handle_decrypt`** : un hexadécimal collé de plus de 10 Mo + 84 octets est bien rejeté (le contrôle n'est plus du code mort imbriqué après un `return`) - commit `9aaec72`.

### Non testé, en connaissance de cause

Le cas où `dup_extension()` (appelée par `detect_file_type`) échoue
réellement (`secure_malloc` refuse son allocation de 4 octets) n'est pas
automatisé : `dup_extension` est `static` dans `Cryptage_UI_Common.c`,
donc invisible à `--wrap` (résolu à la compilation, pas à l'édition de
liens) ; une substitution `-D` de `secure_malloc` affecterait aussi tous
les autres appels de ce fichier. Vu la probabilité quasi nulle d'un échec
d'allocation de 4 octets, l'effort n'a pas semblé justifié.
