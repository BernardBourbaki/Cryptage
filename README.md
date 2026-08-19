# Cryptage V38.0.4

**Chiffrement sécurisé de fichiers texte et images**

[![Version](https://img.shields.io/badge/version-38.0.4-blue.svg)](https://github.com/BernardBourbaki/Cryptage/releases)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![OpenSSL](https://img.shields.io/badge/OpenSSL-3.2+-red.svg)](https://www.openssl.org/)

## 🔐 Sécurité

* **Algorithme** : AES-256-GCM (chiffrement authentifié)
* **Dérivation de clé** : Argon2id (résistant aux attaques GPU)
* **Intégrité** : Tag d'authentification GCM
* **Format** : .crypt (propriétaire mais spécification ouverte)

## ⚠️ Important

### Compatibilité des versions

* **V37 / V37.1 / V37.2 / V37.2.1 / V37.3 / V37.3.1 / V38.0.0 / V38.0.1 / V38.0.2 / V38.0.3 / V38.0.4** : Déchiffre **UNIQUEMENT** les fichiers .crypt créés avec V37+
* **V31-V36** : Utilisez [Cryptage V36.1](https://github.com/BernardBourbaki/Cryptage/releases/tag/v36.1) pour déchiffrer les anciens fichiers

### Limites

* **Taille maximale** : 10 Mo par fichier
* **Formats supportés** :
  * Texte : .txt
  * Images : .jpg, .png, .bmp
  * Crypté : .crypt
* **Mot de passe** : Aucune récupération possible - **utilisez un gestionnaire de mots de passe**

## 🚀 Installation

### Windows (Exécutable)

1. Téléchargez Cryptage_V38.0.4.exe depuis [Releases](https://github.com/BernardBourbaki/Cryptage/releases/latest)
2. Vérifiez le checksum SHA256 (voir checksums.txt)
3. Lancez l'exécutable (pas d'installation requise)

### Compilation depuis les sources

**Prérequis** :

* GCC (MinGW-w64 pour Windows)
* OpenSSL 3.2+

**Commande** :

1. Si vous voulez une version locale, légère, idéale pour des tests :
```bash
gcc -finput-charset=UTF-8 -fexec-charset=CP1252 Cryptage_Main.c Cryptage_Core.c Cryptage_UI_Common.c Cryptage_UI.c -o Cryptage_V38.0.4.exe -lssl -lcrypto -lgdi32 -lcomctl32 -mwindows
```

2. Si vous voulez compiler vous-même la version portable, très robuste et optimisée au maximum, qui est proposée en téléchargement :
```bash
gcc -static -static-libgcc -Os -s -flto -fno-ident -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections -fstack-protector-strong -finput-charset=UTF-8 -fexec-charset=CP1252 -D_FORTIFY_SOURCE=2 -DNDEBUG -I/c/msys64/mingw64/include -L/c/msys64/mingw64/lib Cryptage_Main.c Cryptage_Core.c Cryptage_UI_Common.c Cryptage_UI.c -o Cryptage_V38.0.4.exe -Wl,--gc-sections -Wl,--build-id=none -lssl -lcrypto -lwinpthread -lws2_32 -lcrypt32 -lgdi32 -lcomctl32 -mwindows
```
**Note** : Cette commande est conçue pour **MSYS2 MinGW-w64** avec les chemins d'inclusion par défaut (`/c/msys64/mingw64`). Adaptez `-I` et `-L` à votre propre installation si nécessaire.

## 📖 Utilisation

### Interface intuitive

#### Pour chiffrer un fichier texte ou image

1. **Créez un mot de passe fort** (16+ caractères recommandés)
   * Utilisez KeePass, Bitwarden ou un autre gestionnaire
   * ⚠️ Ne transmettez **JAMAIS** le mot de passe avec le fichier chiffré
2. **IMPORTER** → **CHIFFRER** → **SAUVEGARDER**
   * Cliquez sur "IMPORTER" et sélectionnez votre fichier
   * Cliquez sur "CHIFFRER"
   * Cliquez sur "SAUVEGARDER" pour créer le fichier .crypt

#### Pour chiffrer du texte saisi directement

1. **Tapez ou collez** votre texte directement dans la zone "Entrée"
2. Saisissez votre mot de passe
3. Cliquez sur **CHIFFRER**
4. Le résultat hexadécimal apparaît dans "Sortie" — vous pouvez le **copier-coller** dans un email

#### Pour déchiffrer un fichier .crypt

1. **Entrez le mot de passe** utilisé lors du chiffrement
2. **IMPORTER** → **DÉCHIFFRER** → **EXPORTER**
   * Cliquez sur "IMPORTER" et sélectionnez le fichier .crypt
   * Cliquez sur "DÉCHIFFRER"
   * Cliquez sur "EXPORTER" (Texte ou Image selon le contenu)

#### Pour déchiffrer un texte hexadécimal collé

1. **Collez** le texte hexadécimal reçu (par email, messagerie...) dans la zone "Entrée"
2. Saisissez votre mot de passe
3. Cliquez sur **DÉCHIFFRER**
4. Le texte clair apparaît dans "Sortie"

#### Pour modifier un texte importé avant chiffrement

1. **IMPORTER** un fichier .txt
2. **Modifiez** le texte directement dans la zone "Entrée"
3. Cliquez sur **CHIFFRER** — c'est le texte **modifié** qui sera chiffré

### Panneau "Prise en main rapide"

Le panneau d'aide rapide, entièrement visible depuis la V37.1, reste accessible via le bouton en bas.

## 🔒 Bonnes pratiques de sécurité

✅ **À FAIRE** :

* Utilisez des mots de passe de 16 caractères minimum
* Conservez vos mots de passe dans un gestionnaire sécurisé
* Testez le déchiffrement **avant** de supprimer l'original
* Gardez plusieurs copies du logiciel Cryptage_V38.0.4.exe

❌ **À NE PAS FAIRE** :

* Envoyer le mot de passe ET le fichier chiffré par le même canal
* Utiliser le même mot de passe pour tous vos fichiers
* Oublier de vérifier que le déchiffrement fonctionne
* Supprimer l'original avant d'avoir testé

## 🛠️ Paramètres techniques

### Configuration automatique

Le logiciel calcule automatiquement le paramètre mémoire optimal :

* **Formule** : 25% de la RAM disponible
* **Minimum** : 4 Mo (4096 KiB)
* **Maximum** : 1024 Mo (1 048 576 KiB)
* **Par défaut** : 16 Mo si le calcul échoue

### Structure du fichier `.crypt`
```
[AAD - 24 octets]

Version (4) : 370 (décimal)
Réservé (12) : extensibilité future (zéros)
Longueur du texte en clair (4)
Mémoire Argon2id (4) : en KiB

[SALT - 32 octets]
[NONCE - 12 octets]
[TAG - 16 octets]
[CIPHERTEXT - variable]
```

## 📊 Nouveautés V38.0.4 (19 août 2026)

### Corrections de robustesse post-release

* 🐛 Vérification NULL dans bin_to_hex : ajout d’un test de retour après secure_malloc pour éviter un crash en cas d’échec d’allocation.
* 🐛 Plafond MAX_CRYPT_SIZE effectif : correction de l’accolade mal placée dans handle_decrypt ; la limite sur l’hexadécimal collé est désormais opérationnelle.
* 🐛 Réactivation de l’UI après échec de CreateThread : update_buttons() est désormais appelé pour débloquer l’interface en cas d’échec de création de thread.
* 🐛 Invalidation de l’état encrypted sur EN_CHANGE : la modification de la zone « Entrée » après chiffrement remet correctement encrypted à FALSE, évitant une sauvegarde incohérente.

### Compatibilité

* 🔧 Format .crypt inchangé : compatibilité totale V38.0.3 ↔ V38.0.4
* 🔧 Compilation : MSVC / MinGW-w64 avec OpenSSL 3.2+

## 📊 Nouveautés V38.0.3 (15 août 2026)

### Corrections de robustesse et cohérence d'état

* 🐛 **Verrouillage mémoire des buffers hexadécimaux** : `bin_to_hex` et `hex_to_bin` utilisent désormais un paramètre `force_lock` explicite choisi par l'appelant selon la sensibilité des données. Les buffers contenant du clair (image importée, image déchiffrée) sont verrouillés (`TRUE`) ; les buffers de chiffré (`FALSE`) conservent le comportement existant avec effacement via `secure_free()`.
* 🐛 **Augmentation du working set** : `SetProcessWorkingSetSize(48 Mo, 96 Mo)` au démarrage améliore le taux de succès de `VirtualLock` sur les buffers de conversion. Échec silencieux accepté si le privilège utilisateur est insuffisant.
* 🐛 **Nettoyage du paramètre zero_on_free** : suppression du paramètre `zero_on_free` dans `secure_malloc` et `SecureMemEntry`. Le comportement réel (effacement inconditionnel `OPENSSL_cleanse` + `VirtualLock` optionnel) est désormais explicite.
* 🐛 **Désactivation visuelle des boutons pendant opération** : `update_buttons()` tient compte de `operation_in_progress` pour griser l'ensemble des 7 boutons pendant le chiffrement/déchiffrement (Argon2id compris).
* 🐛 **Protection contre le double-free à la fermeture** : remise à `NULL` de `loaded_data`, `loaded_len`, `original_extension` et `original_extension_len` dans `WM_DESTROY` après `secure_free()`, rendant le nettoyage ultérieur dans `WinMain` idempotent.
* 🐛 **Limite de taille sur hexadécimal collé** : le déchiffrement depuis la zone « Entrée » applique désormais `MAX_CRYPT_SIZE` (10 Mo + 84 octets d'en-tête), identique au chemin fichier `.crypt`.

### Compatibilité

* 🔧 **Format .crypt inchangé** : compatibilité totale V37.3.1 ↔ V38.0.3
* 🔧 Compilation : MSVC / MinGW-w64 avec OpenSSL 3.2+

## 📊 Nouveautés V38.0.2 (14 août 2026)
## 📊 Nouveautés V38.0.1 (14 août 2026)

### Corrections de robustesse et de sécurité mémoire

* 🐛 **Exportation de texte déchiffré en UTF-8** : `save_decrypted_text_file_secure` utilise désormais `secure_get_edit_text` (API Unicode + conversion UTF-8) au lieu de `GetWindowTextLengthA`/`GetWindowTextA`. Cela élimine la corruption silencieuse des caractères non-ANSI (cyrillique, japonais, symboles mathématiques, etc.) lors de l'exportation d'un texte déchiffré.
* 🐛 **Fuite mémoire sur import successif** : l'ancien buffer `loaded_data` est désormais libéré (`secure_free`) avant chaque nouvel import. Les données sensibles de l'import précédent ne persistent plus en mémoire verrouillée.
* 🐛 **Vérification de `fclose` dans les sauvegardes** : les fonctions `save_binary_file_secure`, `save_decrypted_text_file_secure` et `save_image_file_secure` propagent désormais l'échec de `fclose` (écriture différée échouée, disque plein, etc.) à l'appelant, au lieu de signaler un succès erroné.
* 🐛 **Verrouillage mémoire des buffers de conversion UI** : les buffers temporaires de `secure_set_edit_text` (UTF-16, ANSI, fallback) sont désormais alloués avec `force_lock = TRUE`, empêchant leur swapage sur disque pendant leur brève durée de vie.

### Documentation

* 📝 **Limitation UTF-8 du validateur de mot de passe** : `is_password_strong` analyse les octets individuellement via les fonctions C standard (`isupper`, `islower`, `isdigit`, `ispunct`), qui ne reconnaissent correctement que l'ASCII. Les mots de passe UTF-8 multi-octets peuvent être faussement rejetés. L'usage de gestionnaires de mots de passe générant de l'ASCII (KeePass, Bitwarden, etc.) reste recommandé.

## 📊 Nouveautés V38.0.0 (13 août 2026)

### Nouvelles fonctionnalités (V38.0.0)

* ✨ **Zone "Entrée" éditable** : le texte peut être saisi, modifié ou collé directement dans la zone "Entrée" sans passer par un fichier
* ✨ **Chiffrement depuis la zone "Entrée"** : pour le texte, c'est le contenu **actuel** de la zone qui est chiffré, pas le fichier d'origine
* ✨ **Déchiffrement depuis l'hexadécimal** : un texte hexadécimal collé dans "Entrée" peut être déchiffré directement, sans fichier .crypt intermédiaire
* ✨ **Flux mail/messagerie** : chiffrez un texte, copiez l'hexadécimal de "Sortie", collez-le dans un email — le destinataire peut le déchiffrer directement

### Compatibilité

* 🔧 **Format .crypt inchangé** : les fichiers .crypt V38 restent compatibles avec V37.3.1 et vice-versa
* 🔧 **Aucun nouveau fichier source** : les 6 fichiers existants suffisent

### Corrections mineures (V38.0.0)

* 🔧 Amélioration de la gestion du jeu de caractères de la police (`DEFAULT_CHARSET` au lieu de `ANSI_CHARSET`)
* 🔧 **Corrections post-release** (appliquées au fichier `Cryptage_UI.c` et à l'exécutable `Cryptage_V38.0.0.exe` en ligne sans changement de numéro de version) :
  * Exportation après déchiffrement d'un texte hexadécimal collé : correction d'une régression où les boutons EXPORTER restaient inactifs après un déchiffrement réussi à partir d'un hexadécimal collé dans la zone "Entrée" (flux mail/messagerie). Le texte clair était correctement affiché dans "Sortie", mais l'absence de fichier importé bloquait l'exportation. Ce comportement est désormais fonctionnel : **coller l'hex → DÉCHIFFRER → EXPORTER** fonctionne comme attendu.

## 📊 Historique des versions

### V37.3.1 (12 août 2026)

* 🐛 Rétablissement de la réinitialisation de l'extension d'origine (`original_extension`) lors de l'import d'un fichier

### V37.3 (12 août 2026)

* 🐛 Correction d'une incohérence entre la limite du texte en clair (10 Mo) et celle du fichier .crypt correspondant
* ✨ Prise en charge complète des chemins de fichiers Unicode

### V37.2.1 (9 août 2026)

* 🐛 Réinitialisation propre de l'extension d'origine entre imports successifs

### V37.2 (9 août 2026)

* 🐛 Corrections de sécurité mémoire et de robustesse

### V37.1 (17 décembre 2025)

* ✨ Améliorations de l'interface

### V37

* ✨ Interface unique simplifiée, limite portée à 10 Mo

## 🐛 Problèmes connus

* L'affichage de caractères cyrilliques ou japonais dans les zones de texte de l'interface peut être limité par les contrôles ANSI de Windows. Le chiffrement/déchiffrement de ces caractères fonctionne correctement en interne (via UTF-8), mais leur affichage visuel peut être remplacé par des `?`. Les noms de fichiers Unicode sont pleinement supportés.
* **Sécurité mémoire — zone « Entrée » éditable** : depuis la V38.0.0, la zone « Entrée » est éditable (saisie libre, modification d'un texte importé). Les contrôles `Edit` multilignes de Windows conservent un historique d'annulation interne (`Ctrl+Z`) géré par le système d'exploitation, hors du contrôle du programme. Des copies transitoires du texte en clair peuvent donc persister temporairement en mémoire process, non effacées par le mécanisme `secure_malloc`/`secure_free` de Cryptage. Ce phénomène est inhérent aux contrôles Win32 et ne constitue pas une faille propre à Cryptage ; il est signalé ici par transparence.
* **Validateur de mot de passe et UTF-8** (V38.0.1, limitation documentée) : la vérification de robustesse (`is_password_strong`) analyse les octets individuellement via les fonctions C standard (`isupper`, `islower`, `isdigit`, `ispunct`), qui ne reconnaissent correctement que les caractères ASCII (0-127). Un mot de passe UTF-8 multi-octets contenant majuscule, minuscule, chiffre et symbole peut être faussement rejeté comme "faible". L'usage de gestionnaires de mots de passe générant de l'ASCII (KeePass, Bitwarden, etc.) est recommandé.

Signalez les bugs via [Issues](https://github.com/BernardBourbaki/Cryptage/issues).

## 📜 Licence

Ce projet est sous licence MIT. Voir [LICENSE](LICENSE) pour plus de détails.

## 👤 Auteur

**Bernard DÉMARET**

* GitHub : [@BernardBourbaki](https://github.com/BernardBourbaki)

## 🙏 Remerciements

* OpenSSL pour les algorithmes cryptographiques
* La communauté GitHub pour les retours et suggestions

## ⚖️ Avertissement

Ce logiciel est fourni "tel quel", sans garantie d'aucune sorte. L'auteur ne peut être tenu responsable de toute perte de données. **Conservez toujours des sauvegardes de vos fichiers originaux.**
