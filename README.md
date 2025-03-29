# RC5
This project implements the RC5 encryption and decryption algorithm in C. It includes functions for key scheduling, encryption, decryption, and memory management.

## Features

- **Key Scheduling**: Generates subkeys for encryption and decryption using the provided key.
- **Encryption**: Encrypts data blocks using the RC5 algorithm.
- **Decryption**: Decrypts data blocks back to their original form.
- **Memory Management**: Allocates and frees memory for the RC5 context and sensitive data.

## How It Works

1. **Initialization**: The `rc5_init` function initializes the RC5 context with the number of rounds and allocates memory for subkeys.
2. **Key Scheduling**: The `rc5_key` function generates subkeys from the provided key.
3. **Encryption**: The `rc5_encrypt` function encrypts data blocks using the generated subkeys.
4. **Decryption**: The `rc5_decrypt` function decrypts the encrypted data blocks back to their original form.
5. **Cleanup**: The `rc5_destroy` function securely clears sensitive data and frees allocated memory.

## Usage

The `main` function demonstrates the usage of the RC5 implementation:

1. Initialize the RC5 context with 10 rounds.
2. Generate subkeys using the key `"ABCDE"`.
3. Encrypt 4 blocks of data.
4. Decrypt the encrypted data.
5. Print the results of encryption and decryption.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Reference

- Bruce Shnier, "The RC5 Encryption Algorithm," 1996.
