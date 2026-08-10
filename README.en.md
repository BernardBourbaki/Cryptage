# Cryptage V37.2.1

**Secure encryption for text files and images**

[![Version](https://img.shields.io/badge/version-37.2.1-blue.svg)](https://github.com/BernardBourbaki/Cryptage/releases)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![OpenSSL](https://img.shields.io/badge/OpenSSL-3.2+-red.svg)](https://www.openssl.org/)

## 🔐 Security

* **Algorithm**: AES-256-GCM (authenticated encryption)
* **Key derivation**: Argon2id (GPU-attack resistant)
* **Integrity**: GCM authentication tag
* **Format**: .crypt (proprietary but openly specified)

## ⚠️ Important

### Version compatibility

* **V37 / V37.1 / V37.2 / V37.2.1**: Decrypts **ONLY** .crypt files created with V37+
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

1. Download Cryptage_V37.2.1.exe from [Releases](https://github.com/BernardBourbaki/Cryptage/releases/latest)
2. Verify the SHA256 checksum (see checksums.txt)
3. Run the executable (no installation required)

### Building from source

**Requirements**:

* GCC (MinGW-w64 for Windows)
* OpenSSL 3.2+

**Command**:

1. For a local, lightweight build, ideal for testing:
   ```bash
   gcc -finput-charset=UTF-8 -fexec-charset=CP1252 Cryptage_Main.c Cryptage_Core.c Cryptage_UI_Common.c Cryptage_UI.c -o Cryptage_V37.2.1.exe -lssl -lcrypto -lgdi32 -lcomctl32 -mwindows
   ```

2. To build the portable, highly robust, fully optimized version yourself - the one offered for download:
   ```bash
   gcc -static -static-libgcc -Os -s -flto -fno-ident -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections -finput-charset=UTF-8 -fexec-charset=CP1252 -I/c/msys64/mingw64/include -L/c/msys64/mingw64/lib Cryptage_Main.c Cryptage_Core.c Cryptage_UI_Common.c Cryptage_UI.c -o Cryptage_V37.2.1.exe -Wl,--gc-sections -Wl,--build-id=none -D_FORTIFY_SOURCE=2 -DNDEBUG -lssl -lcrypto -lwinpthread -lws2_32 -lcrypt32 -lgdi32 -lcomctl32 -mwindows
   ```
**Note**: This command is designed for **MSYS2 MinGW-w64** with the default include paths (`/c/msys64/mingw64`). Adjust `-I` and `-L` to match your own installation if needed.

## 📖 Usage

### Simple 3-step interface

#### To encrypt a file

1. **Create a strong password** (16+ characters recommended)
   * Use KeePass, Bitwarden, or another password manager
   * ⚠️ **NEVER** send the password together with the encrypted file
2. **IMPORT** → **ENCRYPT** → **SAVE**
   * Click "IMPORTER" and select your file
   * Click "CHIFFRER"
   * Click "SAUVEGARDER" to create the .crypt file

#### To decrypt a file

1. **Enter the password** used during encryption
2. **IMPORT** → **DECRYPT** → **EXPORT**
   * Click "IMPORTER" and select the .crypt file
   * Click "DÉCHIFFRER"
   * Click "EXPORTER" (Text or Image depending on content)

### "Quick start" panel

The quick-help panel, fully visible since V37.1, remains accessible via the button at the bottom.

## 🔒 Security best practices

✅ **DO**:

* Use passwords of at least 16 characters
* Keep your passwords in a secure password manager
* Test decryption **before** deleting the original
* Keep several copies of the Cryptage_V37.2.1.exe program

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
[AAD - 24 bytes]

Version (4): 370 (decimal)
Reserved (12): future extensions (zeros)
Plaintext length (4)
Argon2id memory (4): in KiB

[SALT - 32 bytes]
[NONCE - 12 bytes]
[TAG - 16 bytes]
[CIPHERTEXT - variable]

## 📊 What's new in V37.2.1 (August 9, 2026)

### Fixes (V37.2.1)
- 🐛 Clean reset of the original file extension between successive imports (prevents a JPG/PNG/BMP extension from persisting when importing a .crypt file)

## 📊 What's new in V37.2 (August 9, 2026)

### Fixes (V37.2)

- 🐛 Fixed a possible out-of-bounds read when decrypting a truncated or corrupted .crypt file
- 🐛 Fixed a one-byte overflow during memory cleanup after encryption
- 🐛 Fixed a memory leak on image import (JPG/PNG/BMP)
- 🐛 Fixed a possible crash when closing the app while an operation is in progress
- 🐛 Fixed a GDI handle leak when redrawing buttons
- 🔧 Cleaned up a poorly-defined reserved area in the .crypt file header (no impact on existing files, format unchanged)

See [closed Issues](https://github.com/BernardBourbaki/Cryptage/issues?q=is%3Aissue+is%3Aclosed) for details on each fix.

## 📊 What's new in V37.1 (December 17, 2025)

### Interface improvements (V37.1)

* ✨ Harmonious spacing between button groups (airier and more readable)
* ✨ "Quick start" panel fully visible, with a direct link to V36.1
* ✨ Taller window for improved visual comfort

### What's new in V37 (compared to V36.1)

* ✨ Simplified single-window interface
* ✨ Automatic detection of older versions
* ✨ Clearer error messages
* ✨ Limit raised to 10 MB (from 2 MB)
* ✨ Built-in help panel
* 🔧 Simplified code architecture

### Incompatibility

⚠️ **V37+ does NOT decrypt V31-V36 files**

To decrypt older files, download [Cryptage V36.1](https://github.com/BernardBourbaki/Cryptage/releases/tag/v36.1)

## 🐛 Known issues

None at the moment.

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
