# Cryptage V38.0.2

**Secure encryption for text files and images**

[![Version](https://img.shields.io/badge/version-38.0.2-blue.svg)](https://github.com/BernardBourbaki/Cryptage/releases)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![OpenSSL](https://img.shields.io/badge/OpenSSL-3.2+-red.svg)](https://www.openssl.org/)

## 🔐 Security

* **Algorithm**: AES-256-GCM (authenticated encryption)
* **Key derivation**: Argon2id (GPU-attack resistant)
* **Integrity**: GCM authentication tag
* **Format**: .crypt (proprietary but openly specified)

## ⚠️ Important

### Version compatibility

* **V37 / V37.1 / V37.2 / V37.2.1 / V37.3 / V37.3.1 / V38.0.0 / V38.0.1 / V38.0.2**: Decrypts **ONLY** .crypt files created with V37+
* **V31-V36**: Use [Cryptage V36.1](https://github.com/BernardBourbaki/Cryptage/releases/tag/v36.1) to decrypt older files

### Limits

* **Maximum size**: 10 MB per file
* **Supported formats**:
  * Text: .txt
  * Images: .jpg, .png, .bmp
  * Encrypted: .crypt
* **Password**: No recovery possible - **use a password manager**

## 🚀 Installation

### Windows (Executable)

1. Download Cryptage_V38.0.2.exe from [Releases](https://github.com/BernardBourbaki/Cryptage/releases/latest)
2. Verify the SHA256 checksum (see checksums.txt)
3. Run the executable (no installation required)

### Building from source

**Requirements**:

* GCC (MinGW-w64 for Windows)
* OpenSSL 3.2+

**Command**:

1. For a local, lightweight build, ideal for testing:
```bash
gcc -finput-charset=UTF-8 -fexec-charset=CP1252 Cryptage_Main.c Cryptage_Core.c Cryptage_UI_Common.c Cryptage_UI.c -o Cryptage_V38.0.2.exe -lssl -lcrypto -lgdi32 -lcomctl32 -mwindows
```

2. To build the portable, highly robust, fully optimized version yourself - the one offered for download:
```bash
gcc -static -static-libgcc -Os -s -flto -fno-ident -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections -fstack-protector-strong -finput-charset=UTF-8 -fexec-charset=CP1252 -D_FORTIFY_SOURCE=2 -DNDEBUG -I/c/msys64/mingw64/include -L/c/msys64/mingw64/lib Cryptage_Main.c Cryptage_Core.c Cryptage_UI_Common.c Cryptage_UI.c -o Cryptage_V38.0.2.exe -Wl,--gc-sections -Wl,--build-id=none -lssl -lcrypto -lwinpthread -lws2_32 -lcrypt32 -lgdi32 -lcomctl32 -mwindows
```
**Note**: This command is designed for **MSYS2 MinGW-w64** with the default include paths (`/c/msys64/mingw64`). Adjust `-I` and `-L` to match your own installation if needed.

## 📖 Usage

### Intuitive interface

#### To encrypt a text file or image

1. **Create a strong password** (16+ characters recommended)
   * Use KeePass, Bitwarden or another manager
   * ⚠️ **NEVER** send the password with the encrypted file
2. **IMPORT** → **ENCRYPT** → **SAVE**
   * Click "IMPORTER" and select your file
   * Click "CHIFFRER"
   * Click "SAUVEGARDER" to create the .crypt file

#### To encrypt text entered directly

1. **Type or paste** your text directly into the "Entrée" zone
2. Enter your password
3. Click **CHIFFRER**
4. The hexadecimal result appears in "Sortie" — you can **copy-paste** it into an email

#### To decrypt a .crypt file

1. **Enter the password** used during encryption
2. **IMPORT** → **DECRYPT** → **EXPORT**
   * Click "IMPORTER" and select the .crypt file
   * Click "DÉCHIFFRER"
   * Click "EXPORTER" (Text or Image depending on content)

#### To decrypt pasted hexadecimal text

1. **Paste** the received hexadecimal text (from email, messenger...) into the "Entrée" zone
2. Enter your password
3. Click **DÉCHIFFRER**
4. The plaintext appears in "Sortie"

#### To modify imported text before encryption

1. **IMPORT** a .txt file
2. **Edit** the text directly in the "Entrée" zone
3. Click **CHIFFRER** — the **modified** text will be encrypted

### "Quick start" panel

The quick-help panel, fully visible since V37.1, remains accessible via the button at the bottom.

## 🔒 Security best practices

✅ **DO**:

* Use passwords of at least 16 characters
* Keep your passwords in a secure password manager
* Test decryption **before** deleting the original
* Keep several copies of the Cryptage_V38.0.2.exe program

❌ **DON'T**:

* Send the password AND the encrypted file over the same channel
* Reuse the same password for all your files
* Forget to verify that decryption works
* Delete the original before testing decryption

## 🛠️ Technical parameters

### Automatic configuration

The software automatically calculates the optimal memory parameter:

* **Formula**: 25% of available RAM
* **Minimum**: 4 MB (4096 KiB)
* **Maximum**: 1024 MB (1,048,576 KiB)
* **Default**: 16 MB if the calculation fails

### `.crypt` file structure
```
[AAD - 24 bytes]

Version (4): 370 (decimal)
Reserved (12): future extensions (zeros)
Plaintext length (4)
Argon2id memory (4): in KiB

[SALT - 32 bytes]
[NONCE - 12 bytes]
[TAG - 16 bytes]
[CIPHERTEXT - variable]
```

## 📊 What's new in V38.0.2 (August 14, 2026)

### State consistency and dead code fixes

* 🐛 **Dead mem_kib parameter cleanup**: removed two unused lines in `handle_decrypt` (`Cryptage_UI.c`). The `mem_kib` parameter was read from `ctx->state` then immediately overwritten by the value stored in the `.crypt` file header in `decrypt_data`. The field remains fully used by `handle_encrypt`.
* 🐛 **State reset after encryption**: after a successful encryption, the `decrypted` and `decrypted_type` indicators are properly reset to zero, preventing a visual inconsistency on the EXPORT buttons activation.
* 🐛 **State reset after decryption**: after a successful decryption, the `encrypted` indicator is properly reset to zero, preventing a visual inconsistency on the SAVE button activation.
* 📝 **Memory lock documentation**: added an explicit comment in `bin_to_hex` (`Cryptage_Core.c`) justifying the `force_lock=FALSE` choice for large hexadecimal buffers (~31 MB for a 10 MB image).

## 📊 What's new in V38.0.1 (August 14, 2026)

### Robustness and secure memory fixes

* 🐛 **UTF-8 decrypted text export**: `save_decrypted_text_file_secure` now uses `secure_get_edit_text` (Unicode API + UTF-8 conversion) instead of `GetWindowTextLengthA`/`GetWindowTextA`. This eliminates silent corruption of non-ANSI characters (Cyrillic, Japanese, mathematical symbols, etc.) when exporting decrypted text.
* 🐛 **Memory leak on successive imports**: the old `loaded_data` buffer is now freed (`secure_free`) before each new import. Sensitive data from the previous import no longer persists in locked memory.
* 🐛 **`fclose` verification in save operations**: `save_binary_file_secure`, `save_decrypted_text_file_secure` and `save_image_file_secure` now propagate `fclose` failure (delayed write failure, disk full, etc.) to the caller, instead of reporting a false success.
* 🐛 **Memory lock for UI conversion buffers**: temporary buffers in `secure_set_edit_text` (UTF-16, ANSI, fallback) are now allocated with `force_lock = TRUE`, preventing their swap to disk during their brief lifetime.

### Documentation

* 📝 **UTF-8 password validator limitation** (V38.0.1, documented limitation): `is_password_strong` analyzes individual bytes via standard C functions (`isupper`, `islower`, `isdigit`, `ispunct`), which correctly recognize only ASCII (0-127). UTF-8 multi-byte passwords may be falsely rejected. Using ASCII-generating password managers (KeePass, Bitwarden, etc.) remains recommended.

## 📊 What's new in V38.0.0 (August 13, 2026)

### New features (V38.0.0)

* ✨ **Editable "Entrée" zone**: text can be typed, modified or pasted directly into the "Entrée" zone without going through a file
* ✨ **Encryption from the "Entrée" zone**: for text, the **current** content of the zone is encrypted, not the original file
* ✨ **Decryption from hexadecimal**: hexadecimal text pasted into "Entrée" can be decrypted directly, without an intermediate .crypt file
* ✨ **Email/messenger workflow**: encrypt text, copy the hexadecimal from "Sortie", paste it into an email — the recipient can decrypt it directly

### Compatibility

* 🔧 **Unchanged .crypt format**: V38 .crypt files remain compatible with V37.3.1 and vice-versa
* 🔧 **No new source file**: the existing 6 files are sufficient

### Minor fixes (V38.0.0)

* 🔧 Improved charset handling for the font (`DEFAULT_CHARSET` instead of `ANSI_CHARSET`)
* 🔧 **Post-release fixes** (applied to `Cryptage_UI.c` and the `Cryptage_V38.0.0.exe` executable online without version number change):
  * Export after decrypting pasted hexadecimal text: fixed a regression where the EXPORT buttons remained inactive after a successful decryption from hexadecimal pasted into the "Entrée" zone (email/messenger workflow). The plaintext was correctly displayed in "Sortie", but the absence of an imported file blocked exportation. This behavior is now functional: **paste hex → DÉCHIFFRER → EXPORTER** works as expected.

## 📊 Version history

### V37.3.1 (August 12, 2026)

* 🐛 Restored the reset of the original extension (`original_extension`) during file import.

### V37.3 (August 12, 2026)

* 🐛 Fixed an inconsistency between the plaintext limit (10 MB) and the corresponding .crypt file limit
* ✨ Full support for Unicode file paths

### V37.2.1 (August 9, 2026)

* 🐛 Clean reset of the original file extension between successive imports

### V37.2 (August 9, 2026)

* 🐛 Memory security and robustness fixes

### V37.1 (December 17, 2025)

* ✨ Interface improvements

### V37

* ✨ Simplified single-window interface, limit raised to 10 MB

## 🐛 Known issues

* Display of Cyrillic or Japanese characters in the interface text zones may be limited by Windows ANSI controls. Encryption/decryption of these characters works correctly internally (via UTF-8), but their visual display may be replaced by `?`. Unicode file names are fully supported.
* **Memory security — editable "Entrée" zone**: since V38.0.0, the "Entrée" zone is editable (free typing, modification of imported text). Windows multiline `Edit` controls maintain an internal undo history (`Ctrl+Z`) managed by the operating system, outside the program's control. Transient copies of plaintext may therefore persist temporarily in process memory, not erased by Cryptage's `secure_malloc`/`secure_free` mechanism. This phenomenon is inherent to Win32 controls and does not constitute a flaw specific to Cryptage; it is reported here for transparency.
* **Password validator and UTF-8** (V38.0.1, documented limitation): the strength verification (`is_password_strong`) analyzes individual bytes via standard C functions (`isupper`, `islower`, `isdigit`, `ispunct`), which correctly recognize only ASCII characters (0-127). A UTF-8 multi-byte password containing uppercase, lowercase, digit and symbol may be falsely rejected as "weak". Using ASCII-generating password managers (KeePass, Bitwarden, etc.) is recommended.

Report bugs via [Issues](https://github.com/BernardBourbaki/Cryptage/issues).

## 📜 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

## 👤 Author

**Bernard DÉMARET**

* GitHub: [@BernardBourbaki](https://github.com/BernardBourbaki)

## 🙏 Acknowledgements

* OpenSSL for the cryptographic algorithms
* The GitHub community for feedback and suggestions

## ⚖️ Disclaimer

This software is provided "as is", without warranty of any kind. The author cannot be held liable for any data loss. **Always keep backups of your original files.**
