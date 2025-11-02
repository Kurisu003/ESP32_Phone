# server.py
import threading
from server_crypto import decrypt_data, encrypt_data
import whatsapp
import time
from flask import Flask, jsonify, request

app = Flask(__name__)

# api_encryption_key = "3SaZq5I0bhAg9ZEivR4epnFxUOyLOQvzgfafKX9KbmXJ9A43Rd"
api_encryption_key = "b"
# api_key = "wQHctnYmzu54VCHv84yeArE3udSdnoa80wPIczWzLfdEsoPhfnUmZo10laDG15H3"
api_key = "a"

def api_key_is_correct(client_key):
    decrypted_client_key = str(decrypt_data(client_key, api_encryption_key))
    if decrypted_client_key != api_key:
        return False
    return True


@app.route('/api/messages', methods=['GET'])
def get_messages():
    client_key = request.headers.get('X-API-Key')
    try:
        if not api_key_is_correct(client_key):
            return jsonify([{"failed":"failed"}])

        msgs = whatsapp.get_messages_from_contact("Elmar")
        print(f"\nFound {len(msgs)} contacts:")
        for c in msgs[:10]:          # show first 10
            print("  •", c)
        return jsonify([{"Success":"Success"}])


    except:
        return jsonify([{"failed":"failed"}])


@app.route('/api/send_message', methods=['POST'])
def send_message():
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
    start_thread()
    context = ('cert.pem', 'key.pem')  # (cert, key)
    app.run(
        host='0.0.0.0',
        port=58375,
        debug=False,
        ssl_context=context
    )