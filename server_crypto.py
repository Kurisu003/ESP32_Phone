# server_crypto.py
import os
import base64
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
from cryptography.hazmat.primitives.ciphers.aead import AESGCM
from cryptography.exceptions import InvalidTag  # ← ADD THIS LINE

SALT       = b"fixed_salt_for_demo"
ITERATIONS = 600_000
KEY_LENGTH = 32
NONCE_SIZE = 12

class DecryptionError(ValueError):
    pass  # Custom error type

def make_kdf():
    return PBKDF2HMAC(hashes.SHA256(), KEY_LENGTH, SALT, ITERATIONS)

def _to_bytes(obj: bytes | str) -> bytes:
    if isinstance(obj, bytes):
        return obj
    if isinstance(obj, str):
        # Plain text
        return obj.encode("utf-8")
    raise TypeError("Expected bytes or str")

# -------------------------------------------------
def encrypt_data(data: str, password: str) -> str:
    """Returns: base64(nonce || ciphertext || tag) as string"""
    pw_bytes   = _to_bytes(password)
    data_bytes = _to_bytes(data)

    kdf = make_kdf()
    key = kdf.derive(pw_bytes)

    nonce = os.urandom(NONCE_SIZE)
    ct_tag = AESGCM(key).encrypt(nonce, data_bytes, None)

    raw = nonce + ct_tag
    return base64.b64encode(raw).decode('ascii')  # ← string!

# -------------------------------------------------
def decrypt_data(encrypted: str, password: str) -> str:
    try:
        """Accepts base64 string, returns plaintext"""
        # 1. Decode Base64 → raw bytes
        try:
            blob = base64.b64decode(encrypted)
        except Exception as e:
            raise ValueError("Invalid base64") from e

        # 2. Validate length
        if len(blob) < NONCE_SIZE + 16:
            raise ValueError("Encrypted data too short")

        # 3. Split
        nonce = blob[:NONCE_SIZE]
        ct_tag = blob[NONCE_SIZE:]

        # 4. Derive key
        pw_bytes = _to_bytes(password)
        kdf = make_kdf()
        key = kdf.derive(pw_bytes)

        # 5. Decrypt
        plain_bytes = AESGCM(key).decrypt(nonce, ct_tag, None)
        return plain_bytes.decode("utf-8")
    except InvalidTag:
        raise DecryptionError("Invalid password or corrupted data")
    except base64.binascii.Error:
        raise DecryptionError("Invalid base64 encoding")
    except Exception as e:
        raise DecryptionError("Decryption failed")