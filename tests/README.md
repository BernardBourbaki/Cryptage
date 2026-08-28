# Tests unitaires - Cryptage_Core.c

Harnais de tests pour le noyau cryptographique de Cryptage (`encrypt_data`,
`decrypt_data`, `bin_to_hex`, `hex_to_bin`, `is_password_strong`), sans
dépendance externe (pas de framework de test tiers).

## Compilation (MSYS2 MinGW64 ou Invite de commandes Windows)

Depuis la racine du dépôt :

```
gcc -I. -finput-charset=UTF-8 -fexec-charset=CP1252 tests/test_crypto_core.c tests/test_stubs.c Cryptage_Core.c -o tests/test_crypto.exe -lssl -lcrypto -luser32
```

## Exécution

```
tests\test_crypto.exe        (Invite de commandes)
./tests/test_crypto.exe      (MSYS2 MinGW64)
```

Une réussite complète n'affiche que des lignes `[OK]` et se termine par
« N verification(s), 0 echec(s) ».

## Pourquoi test_stubs.c ?

`Cryptage_Core.c` appelle `show_error()`/`show_success()`/
`display_openssl_error()`, normalement définies dans
`Cryptage_UI_Common.c` et qui affichent une vraie `MessageBoxA` Win32. Au
moins un appel (`encrypt_data`, plaintext > 10 Mo) n'est pas gardé par un
test `hwnd != NULL`, et `MessageBoxA(NULL, ...)` reste un appel Win32
valide qui bloquerait un test automatisé en attendant un clic humain.
`test_stubs.c` fournit donc ses propres définitions silencieuses (trace
sur `stderr`) de ces trois fonctions ; le harnais ne lie jamais
`Cryptage_UI_Common.c`.

## Couverture actuelle

- **Fondamentaux** : round-trip hex, round-trip chiffrement/déchiffrement, mot de passe incorrect.
- **Cas limites et négatifs** : texte vide, taille maximale (10 Mo) acceptée / rejetée au-delà, tag GCM falsifié, en-tête tronqué, version de format invalide, `memory_cost_kib` hors bornes.
- **Non-régression** : contrôle du retour NULL de `secure_malloc` dans `bin_to_hex` (régression V38.0.3, corrigée par le commit `b2b789a`).
- **Fuzzing léger** (sans outillage externe) : mutation systématique des 84 octets d'en-tête, entrée malformée pour `hex_to_bin`.
- **Validateur de mot de passe** : `is_password_strong` (bornes de longueur, 4 catégories de caractères, limitation UTF-8 documentée depuis la V38.0.1).

## Non couvert ici (tests d'intégration UI, phase suivante)

`EN_CHANGE` (`Cryptage_UI.c`/`MainWndProc`), l'échec de `CreateThread()`
(`handle_encrypt`/`handle_decrypt`), `detect_file_type`, et le contrôle
`MAX_CRYPT_SIZE` de `handle_decrypt` vivent dans
`Cryptage_UI.c`/`Cryptage_UI_Common.c` : les deux premiers exigent une
vraie fenêtre Win32, les deux autres entreraient en collision avec
`test_stubs.c` si on liait directement `Cryptage_UI_Common.c`.
