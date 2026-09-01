# Contribuer à Cryptage

Ce dépôt est maintenu par une seule personne. Ce document ne vise pas d'abord à
accueillir des pull requests externes, mais à documenter précisément comment
reproduire, vérifier et tester le projet — utile à quiconque veut auditer le
code ou s'assurer qu'un binaire publié correspond bien aux sources.

## Compiler depuis les sources

Les prérequis et la commande de compilation exacte (MSYS2 MinGW-w64 +
OpenSSL) sont documentés dans le [README](README.md). Merci de vous y
référer plutôt qu'à une copie ici, pour éviter toute divergence si la
commande évolue d'une version à l'autre.

## Vérifier un binaire publié

1. Téléchargez `Cryptage_VX.X.X.exe` depuis la page [Releases](../../releases).
2. Calculez son empreinte SHA-256 :
   - PowerShell : `Get-FileHash .\Cryptage_VX.X.X.exe -Algorithm SHA256`
   - Linux / macOS : `sha256sum Cryptage_VX.X.X.exe`
3. Comparez le résultat à la ligne correspondante dans `checksums.txt`, à la
   racine du dépôt.
4. Pour vérifier que le binaire correspond réellement aux sources publiées,
   recompilez-le vous-même (voir ci-dessus) et comparez les deux empreintes.

## Lancer les tests

Le dossier `tests/` contient deux harnais indépendants :

- les tests du noyau cryptographique, isolé de l'interface
  (`Cryptage_Core.c` + stubs) ;
- les tests d'intégration de l'interface Win32 (fenêtre invisible, vrais
  messages Windows).

Voir `tests/README.md` pour le détail des commandes de compilation et
d'exécution.

La CI (GitHub Actions, `.github/workflows/`) exécute automatiquement
`tests.yml` (bloquant) et `cppcheck.yml` (non bloquant) à chaque push ;
CodeQL tourne en configuration par défaut, sans fichier `.yml` dédié.

## Signaler un problème

- **Faille de sécurité** : ne pas ouvrir d'issue publique. Suivre la
  procédure décrite dans [SECURITY.md](SECURITY.md).
- **Bug non sensible ou suggestion** : ouvrez une
  [issue](../../issues) en décrivant le comportement observé, le
  comportement attendu et, si possible, les étapes pour reproduire.

## Style et portée du code

- Le noyau cryptographique (`Cryptage_Core.c`) est volontairement découplé
  de l'interface Win32 (`Cryptage_UI.c`, `Cryptage_UI_Common.c`) — merci de
  conserver cette séparation dans toute modification proposée.
- Toute modification touchant au format `.crypt` ou aux paramètres
  cryptographiques (AES-256-GCM, Argon2id) doit être documentée dans le
  README et, si pertinent, dans la page wiki
  [Format du fichier crypt](../../wiki/Format-du-fichier-crypt).
