# server.py
import threading
from server_crypto import decrypt_data, encrypt_data
import whatsapp
import json
from flask import Flask, jsonify, request
import re
import unicodedata

app = Flask(__name__)


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


@app.route('/api/get_contacts', methods=['GET'])
def get_contacts():
    try:

        contacts = whatsapp.list_contacts()
        return jsonify(clean_list(contacts))


    except:
        return jsonify([{"failed":"failed"}])

@app.route('/api/messages_from_contact/<contact_id>', methods=['GET'])
def messages_from_contact(contact_id):
    try:
        msgs = whatsapp.get_messages_from_contact(contact_id)

        # Remove all newlines from message texts
        sanitized_msgs = []
        for msg in msgs:
            sender, text, flags = msg
            clean_text = text.replace("\r", " ").replace("\n", " ")
            sanitized_msgs.append([sender, clean_text, flags])

        print(f"\nFound {len(sanitized_msgs)} messages:")
        print(sanitized_msgs)

        return jsonify(sanitized_msgs)

    except Exception as e:
        print("Error:", e)
        return jsonify([{"failed": "failed"}])

@app.route('/api/get_unreads', methods=['GET'])
def get_unreads():
    try:

        has_unread, who = whatsapp.has_unread_notifications()
        # return jsonify([has_unread, who])
        return jsonify(clean_list(who))


    except:
        return jsonify([{"failed":"failed"}])


@app.route('/api/send_message_to_contact', methods=['POST'])
def send_message_to_contact():
    try:
        raw = request.data.decode("utf-8")
        print(raw)
        data = json.loads(raw)
        contact = data["content"]["contact"]
        message = data["content"]["message"]
        print(data)
        print(contact)
        print(message)

        whatsapp.send_message_to_contact(contact,message)
        return jsonify({"OK": "OK"}), 201

    except:
        return jsonify([{"failed":"failed"}])

def start_thread():
    thread = threading.Thread(target=whatsapp.start, kwargs={"headless": False}, daemon=True)
    thread.start()

def get_pia_port():
    import subprocess

    # piactl_path = r"C:\Program Files\Private Internet Access\piactl.exe"
    piactl_path = "/Applications/Private Internet Access.app/Contents/MacOS/piactl"


    result = subprocess.run(
        [piactl_path, "get", "portforward"],
        capture_output=True,
        text=True
    )

    if result.returncode != 0:
        raise RuntimeError(result.stderr.strip())

    return result.stdout.strip()


if __name__ == '__main__':
    start_thread()
    context = ('cert.pem', 'key.pem')  # (cert, key)
    port = get_pia_port()
    print(port)
    app.run(
        host='0.0.0.0',
        port=port,
        debug=False,
        ssl_context=context
    )