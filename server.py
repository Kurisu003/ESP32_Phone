# server.py
from server_crypto import encrypt_data, decrypt_data
from flask import Flask, jsonify, request

app = Flask(__name__)

# api_encryption_key = "3SaZq5I0bhAg9ZEivR4epnFxUOyLOQvzgfafKX9KbmXJ9A43Rd"
api_encryption_key = "b"
# api_key = "wQHctnYmzu54VCHv84yeArE3udSdnoa80wPIczWzLfdEsoPhfnUmZo10laDG15H3"
api_key = "a"

#message = b"Hello, fixed key, no salt/nonce!"

encrypted = encrypt_data(api_key, api_encryption_key)
decrypted = decrypt_data(encrypted, api_encryption_key)
print(encrypted)
print(decrypted)

#decrypted = decrypt_data(encrypted)

@app.route('/api/messages', methods=['GET'])
def get_messages():
    client_key = request.headers.get('X-API-Key')
    try:
        decrypted_client_key = str(decrypt_data(client_key, api_encryption_key))
        if decrypted_client_key != api_key:
            return jsonify([{"failed":"failed"}])


        return jsonify([{"Success":"Success"}])


    except:
        return jsonify([{"failed":"failed"}])

    #     return jsonify({"error": "Unauthorized"}), 401

@app.route('/api/send_messege', methods=['POST'])
def send_message():
    return jsonify({"OK": "OK"}), 201

if __name__ == '__main__':
    context = ('cert.pem', 'key.pem')  # (cert, key)
    app.run(
        host='0.0.0.0',
        port=58375,
        debug=False,
        ssl_context=context
    )