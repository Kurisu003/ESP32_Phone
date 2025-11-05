# server.py
import threading
from server_crypto import decrypt_data, encrypt_data
import whatsapp
import time
from flask import Flask, jsonify, request
import re
import unicodedata

app = Flask(__name__)

# api_encryption_key = "3SaZq5I0bhAg9ZEivR4epnFxUOyLOQvzgfafKX9KbmXJ9A43Rd"
api_encryption_key = "b"
# api_key = "wQHctnYmzu54VCHv84yeArE3udSdnoa80wPIczWzLfdEsoPhfnUmZo10laDG15H3"
api_key = "a"

def clean_string(s: str) -> str:
    # Step 1: Replace common umlauts
    umlaut_map = {
        'ä': 'ae', 'Ä': 'Ae',
        'ö': 'oe', 'Ö': 'Oe',
        'ü': 'ue', 'Ü': 'Ue',
        'ß': 'ss'
    }
    for umlaut, replacement in umlaut_map.items():
        s = s.replace(umlaut, replacement)

    # Step 2: Normalize unicode (handles composed chars)
    s = unicodedata.normalize('NFKD', s)

    # Step 3: Remove emojis and non-alphanumeric / basic punctuation
    # Keep letters, numbers, spaces, and common punctuation: .,!?()-
    s = re.sub(r'[^\w\s\.\,\!\?\(\)\-]', '', s)

    # Step 4: Strip whitespace and return
    return s.strip()

def clean_list(strings: list[str]) -> list[str]:
    return [clean_string(s) for s in strings]

def api_key_is_correct(client_key):
    decrypted_client_key = str(decrypt_data(client_key, api_encryption_key))
    if decrypted_client_key != api_key:
        return False
    return True


@app.route('/api/get_contacts', methods=['GET'])
def get_contacts():
    client_key = request.headers.get('X-API-Key')
    try:
        if not api_key_is_correct(client_key):
            return jsonify([{"failed":"failed"}])

        contacts = whatsapp.list_contacts()
        return jsonify(clean_list(contacts))


    except:
        return jsonify([{"failed":"failed"}])

@app.route('/api/messages_from_contact/<contact_id>', methods=['GET'])
def messages_from_contact(contact_id):
    client_key = request.headers.get('X-API-Key')
    try:
        if not api_key_is_correct(client_key):
            return jsonify([{"failed":"failed"}])

        msgs = whatsapp.get_messages_from_contact(contact_id)
        print(f"\nFound {len(msgs)} messages:")
        for c in msgs[:10]:          # show first 10
            print("  •", c)
        return jsonify(msgs)


    except:
        return jsonify([{"failed":"failed"}])


@app.route('/api/get_unreads', methods=['GET'])
def get_unreads():
    client_key = request.headers.get('X-API-Key')
    try:
        if not api_key_is_correct(client_key):
            return jsonify([{"failed":"failed"}])

        has_unread, who = whatsapp.has_unread_notifications()
        return jsonify([has_unread, who])


    except:
        return jsonify([{"failed":"failed"}])


@app.route('/api/send_message_to_contact', methods=['POST'])
def send_message_to_contact():
    client_key = request.headers.get('X-API-Key')

    try:
        if not api_key_is_correct(client_key):
            return jsonify([{"failed":"failed"}])

        data = request.get_json()
        print(data["content"]["contact"])
        print(data["content"]["message"])

        whatsapp.send_message_to_contact(data["content"]["contact"],data["content"]["message"])
        return jsonify({"OK": "OK"}), 201

    except:
        return jsonify([{"failed":"failed"}])

def start_thread():
    thread = threading.Thread(target=whatsapp.start, kwargs={"headless": False}, daemon=True)
    thread.start()

if __name__ == '__main__':
    print(encrypt_data(api_key,api_encryption_key))
    start_thread()
    context = ('cert.pem', 'key.pem')  # (cert, key)
    app.run(
        host='0.0.0.0',
        port=36596,
        debug=False,
        ssl_context=context
    )