# Cryptage V36.1

Portable Windows encryption/decryption application using AES-256-GCM and Argon2id.

![Version](https://img.shields.io/badge/version-36.1-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

![Cryptage V36.1 Interface](https://github.com/BernardBourbaki/cryptage-v36.1/blob/screenshots/Interface.PNG)

## 🔒 Security

- **Encryption**: AES-256-GCM (military-grade standard)
- **Key Derivation**: Argon2id (GPU/ASIC resistant)
- **Authentication**: GCM Tag (guaranteed integrity)
- **Supported Formats**: Text (UTF-8), Images (JPG, PNG, BMP)
- **Maximum Size**: 2 MB per file

## ✨ Features

- ✅ Intuitive colorful interface
- ✅ Automatic format detection
- ✅ Automatic memory parameter extraction
- ✅ Automatic cleanup after export
- ✅ Compatible with versions V31-V36
- ✅ 100% portable (no installation required)

## 📦 Download

**Latest Version**: [Releases](../../releases)

Download `cryptage_v36.1.exe` and run it directly.
No installation needed!

## 🚀 Quick Start

### Encrypting a File

1. Create a strong password (16+ characters)
2. Click "Importer le fichier source" (Import source file)
3. Select your file
4. Click "Chiffrer" (Encrypt)
5. Save as `.crypt`

### Decrypting a File

1. Click "Importer le fichier source" (Import source file)
2. Select the `.crypt` file
3. Memory parameter is automatically extracted
4. Enter the password
5. Click "Déchiffrer" (Decrypt)
6. Export (Text or Image)

## 📖 Complete Documentation

For detailed documentation, see [README.txt](https://github.com/BernardBourbaki/cryptage-v36.1/blob/docs/README.txt) (French)

## 🔐 Security & Best Practices

⚠️ **IMPORTANT**: Security depends on your password!

**Recommendations**:
- Use a password manager (KeePass recommended)
- Passwords of 16+ characters (ideal: 60 characters)
- NEVER transmit password and file through the same channel
- Consult the Security Guide in the documentation

## 📊 Technical Specifications

```
Algorithms:
  Encryption     : AES-256-GCM
  KDF            : Argon2id
  CSPRNG         : OpenSSL RAND_bytes

Argon2id Parameters:
  Iterations     : 2
  Memory         : Configurable (default: 25% RAM)
  Parallelism    : 1

.crypt File Structure:
  Version        : 361 (V36.1)
  AAD Header     : 28 bytes
  Salt           : 16 bytes
  Nonce          : 12 bytes
  GCM Tag        : 16 bytes
  Data           : Variable (max 2 MB)
```

## 🏗️ Building from Source

### Prerequisites
- MinGW-w64
- OpenSSL 1.1.1+

### Build Command
```bash
gcc -o cryptage_v36.1.exe Cryptage_UI.c Cryptage_Core.c \
    -I./openssl/include \
    -L./openssl/lib \
    -lssl -lcrypto \
    -lgdi32 -lcomctl32 \
    -mwindows \
    -static \
    -O2
```

## 📜 Changelog

### Version 36.1 (December 2024)

**New Features**:
- Automatic cleanup after save
- Automatic memory parameter extraction
- Improved support for hex .txt files
- Enhanced format validation

**Bug Fixes**:
- Import of V36.1 .crypt files
- Hex file decryption
- Residual memory management

## 🌐 Language

The application interface is currently in French. English localization is planned for a future release.

## 📄 License

MIT License - © 2024 Bernard DÉMARET

See [LICENSE](LICENSE) for full details.

## ⚠️ Disclaimer

This software is provided "as is" without warranty of any kind.
The author disclaims all liability for data loss.

**Use at your own risk.**

## 🔍 Security Audit Status

This project has not yet undergone an independent security audit. While it uses industry-standard cryptographic algorithms (AES-256-GCM, Argon2id), we recommend:

- Reviewing the source code before use in production environments
- Testing thoroughly with non-sensitive data first
- Following all password best practices
- Not relying solely on this tool for critical data protection

Community security reviews and contributions are welcome!

## 🙏 Acknowledgments

- OpenSSL Project for the cryptographic library
- Argon2 Community for the KDF
- Claude (Anthropic) for development assistance

## 📞 Support

- **Issues**: [Report a bug](../../issues)
- **Discussions**: [Forum](../../discussions)
- **Documentation**: Available in French in the [docs branch](../../tree/docs)

## 🤝 Contributing

Contributions are welcome! Please feel free to:
- Report bugs or security issues
- Suggest new features
- Submit pull requests
- Improve documentation
- Translate the interface

For security vulnerabilities, please see [SECURITY.md](SECURITY.md).

## 🗺️ Roadmap

Future enhancements under consideration:
- Cross-platform support (Linux, macOS)
- Command-line interface
- Support for larger files (>2 MB)
- English UI localization
- Additional file format support

---

⭐ If you find this project useful, please consider giving it a star!

🇫🇷 **Version française** : [README.md](README.md)