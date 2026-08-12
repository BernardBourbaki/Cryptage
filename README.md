# Cryptage V37.3.1

**Chiffrement sécurisé de fichiers texte et images**

[![Version](https://img.shields.io/badge/version-37.3.1-blue.svg)](https://github.com/BernardBourbaki/Cryptage/releases) 
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![OpenSSL](https://img.shields.io/badge/OpenSSL-3.2+-red.svg)](https://www.openssl.org/)


## 🔐 Sécurité

* **Algorithme** : AES-256-GCM (chiffrement authentifié)
* **Dérivation de clé** : Argon2id (résistant aux attaques GPU)
* **Intégrité** : Tag d'authentification GCM
* **Format** : .crypt (propriétaire mais spécification ouverte)

## ⚠️ Important

### Compatibilité des versions

* **V37 / V37.1 / V37.2 / V37.2.1 / V37.3 / V37.3.1** : Déchiffre **UNIQUEMENT** les fichiers .crypt créés avec V37+
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

1. Téléchargez Cryptage_V37.3.1.exe depuis [Releases](https://github.com/BernardBourbaki/Cryptage/releases/latest)
2. Vérifiez le checksum SHA256 (voir checksums.txt)
3. Lancez l'exécutable (pas d'installation requise)

### Compilation depuis les sources

**Prérequis** :

* GCC (MinGW-w64 pour Windows)
* OpenSSL 3.2+

**Commande** :

1. Si vous voulez une version locale, légère, idéale pour des tests :
   ```bash
   gcc -finput-charset=UTF-8 -fexec-charset=CP1252 Cryptage_Main.c Cryptage_Core.c Cryptage_UI_Common.c Cryptage_UI.c -o Cryptage_V37.3.1.exe -lssl -lcrypto -lgdi32 -lcomctl32 -mwindows
   ```
   
2. Si vous voulez compiler vous-même la version portable, très robuste et optimisée au maximum, qui est proposée en téléchargement :
   ```bash
   gcc -static -static-libgcc -Os -s -flto -fno-ident -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections -fstack-protector-strong -finput-charset=UTF-8 -fexec-charset=CP1252 -D_FORTIFY_SOURCE=2 -DNDEBUG -I/c/msys64/mingw64/include -L/c/msys64/mingw64/lib Cryptage_Main.c Cryptage_Core.c Cryptage_UI_Common.c Cryptage_UI.c -o Cryptage_V37.3.1.exe -Wl,--gc-sections -Wl,--build-id=none -lssl -lcrypto -lwinpthread -lws2_32 -lcrypt32 -lgdi32 -lcomctl32 -mwindows
   ```
**Note** : Cette commande est conçue pour **MSYS2 MinGW-w64** avec les chemins d’inclusion par défaut (`/c/msys64/mingw64`). Adaptez `-I` et `-L` à votre propre installation si nécessaire.

## 📖 Utilisation

### Interface intuitive en 3 étapes

#### Pour chiffrer un fichier

1. **Créez un mot de passe fort** (16+ caractères recommandés)
   * Utilisez KeePass, Bitwarden ou un autre gestionnaire
   * ⚠️ Ne transmettez **JAMAIS** le mot de passe avec le fichier chiffré
2. **IMPORTER** → **CHIFFRER** → **SAUVEGARDER**
   * Cliquez sur "IMPORTER" et sélectionnez votre fichier
   * Cliquez sur "CHIFFRER"
   * Cliquez sur "SAUVEGARDER" pour créer le fichier .crypt

#### Pour déchiffrer un fichier

1. **Entrez le mot de passe** utilisé lors du chiffrement
2. **IMPORTER** → **DÉCHIFFRER** → **EXPORTER**
   * Cliquez sur "IMPORTER" et sélectionnez le fichier .crypt
   * Cliquez sur "DÉCHIFFRER"
   * Cliquez sur "EXPORTER" (Texte ou Image selon le contenu)

### Panneau "Prise en main rapide"

Le panneau d’aide rapide, entièrement visible depuis la V37.1, reste accessible via le bouton en bas.

## 🔒 Bonnes pratiques de sécurité

✅ **À FAIRE** :

* Utilisez des mots de passe de 16 caractères minimum
* Conservez vos mots de passe dans un gestionnaire sécurisé
* Testez le déchiffrement **avant** de supprimer l'original
* Gardez plusieurs copies du logiciel Cryptage_V37.3.1.exe

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
[AAD - 24 octets]

Version (4) : 370 (décimal)
Réservé (12) : extensibilité future (zéros)
Longueur du texte en clair (4)
Mémoire Argon2id (4) : en KiB

[SALT - 32 octets]
[NONCE - 12 octets]
[TAG - 16 octets]
[CIPHERTEXT - variable]

## Nouveautés V37.3.1 (12 août 2026)

#### Correction

- 🐛 Rétablissement de la réinitialisation de l'extension d'origine (`original_extension`) lors de l'import d'un fichier. Cela garantit qu'importer successivement une image puis un fichier `.crypt` ne laisse plus de résidus dans l'état interne du programme (régression corrigée par rapport à la V37.2.1).


## 📊 Nouveautés V37.3 (12 août 2026)

### Corrections et améliorations (V37.3)

- 🐛 Correction d'une incohérence entre la limite du texte en clair (10 Mo) et celle du fichier .crypt correspondant, qui ajoute 84 octets d'en-tête : un fichier chiffré à la taille maximale pouvait devenir trop volumineux pour être réimporté par le programme qui venait de le créer
- ✨ Prise en charge complète des chemins de fichiers Unicode (import, chiffrement, déchiffrement, export) — un nom de fichier ou de dossier contenant des caractères non représentables dans la page de code du système (au-delà des accents français) fonctionne désormais normalement

Voir les [Issues fermées](https://github.com/BernardBourbaki/Cryptage/issues?q=is%3Aissue+is%3Aclosed) pour le détail de chaque correctif.

## 📊 Nouveautés V37.2.1 (9 août 2026)

### Corrections (V37.2.1)

- 🐛 Réinitialisation propre de l'extension d'origine entre imports successifs (évite qu'une extension JPG/PNG/BMP persiste lors de l'import d'un fichier .crypt)

## 📊 Nouveautés V37.2 (9 août 2026)

### Corrections (V37.2)

- 🐛 Correction d'une lecture hors limites possible au déchiffrement d'un fichier .crypt tronqué ou corrompu
- 🐛 Correction d'un débordement d'un octet lors du nettoyage mémoire après chiffrement
- 🐛 Correction d'une fuite mémoire à l'import d'images (JPG/PNG/BMP)
- 🐛 Correction d'un risque de plantage en cas de fermeture de l'application pendant une opération en cours
- 🐛 Correction d'une fuite de handle GDI lors de l'affichage des boutons
- 🔧 Nettoyage d'une zone réservée mal définie dans l'en-tête des fichiers .crypt (aucun impact sur les fichiers existants, format inchangé)

Voir les [Issues fermées](https://github.com/BernardBourbaki/Cryptage/issues?q=is%3Aissue+is%3Aclosed) pour le détail de chaque correctif.

## 📊 Nouveautés V37.1 (17 décembre 2025)

### Améliorations interface (V37.1)

* ✨ Espacements harmonieux entre les groupes de boutons (plus aéré et lisible)
* ✨ Panneau "Prise en main rapide" entièrement visible avec lien direct vers V36.1
* ✨ Fenêtre plus haute pour un meilleur confort visuel

### Nouveautés V37 (par rapport à V36.1)

* ✨ Interface unique simplifiée
* ✨ Détection automatique des versions antérieures
* ✨ Messages d'erreur plus clairs
* ✨ Limite portée à 10 Mo (au lieu de 2 Mo)
* ✨ Panneau d'aide intégré
* 🔧 Architecture du code simplifiée

### Incompatibilité

⚠️ **V37+ ne déchiffre PAS les fichiers V31-V36**

Pour déchiffrer d'anciens fichiers, téléchargez [Cryptage V36.1](https://github.com/BernardBourbaki/Cryptage/releases/tag/v36.1)

## 🐛 Problèmes connus

Aucun problème connu pour le moment.

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
