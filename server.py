# server.py
import threading
from server_crypto import decrypt_data, encrypt_data
import whatsapp
import json
from flask import Flask, jsonify, request
import re
import unicodedata
import time
app = Flask(__name__)

file_path = "./contact_map.txt"

def get_key(key):
    global file_path
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            for line in file:
                line = line.strip()
                if '=' in line:
                    existing_key, value = line.split('=', 1)
                    if existing_key == key:
                        return value

        return None  # Key not found

    except FileNotFoundError:
        return None
    except Exception as e:
        return f"Error: {e}"

def append_key(key, value):
    global file_path
    try:
        existing_keys = set()

        try:
            with open(file_path, 'r', encoding='utf-8') as file:
                for line in file:
                    line = line.strip()
                    if '=' in line:
                        existing_key = line.split('=', 1)[0]
                        existing_keys.add(existing_key)
        except FileNotFoundError:
            pass  # File does not exist yet

        if key in existing_keys:
            return f"Key '{key}' already exists. No changes made."

        with open(file_path, 'a', encoding='utf-8') as file:
            file.write(f"{key}={value}\n")

        return f"Key '{key}' successfully added."

    except Exception as e:
        return f"Error: {e}"



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
        clean_list = clean_list(contacts)

        for key, value in zip(clean_list, contacts):
            append_key(key, value)

        return jsonify(clean_list)


    except:
        return jsonify([{"failed":"failed"}])

@app.route('/api/messages_from_contact/<contact_id>', methods=['GET'])
def messages_from_contact(contact_id):

    actual_contact = get_key(contact_id) or contact_id
    print(actual_contact)

    try:
        msgs = whatsapp.get_messages_from_contact(actual_contact)

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
        return jsonify(clean_list(who))


    except:
        return jsonify([{"failed":"failed"}])


@app.route('/api/send_message_to_contact', methods=['POST'])
def send_message_to_contact():
    try:
        raw = request.data.decode("utf-8")
        data = json.loads(raw)
        contact = data["content"]["contact"]
        message = data["content"]["message"]

        actual_contact = get_key(contact)

        whatsapp.send_message_to_contact(actual_contact,message)
        return jsonify({"OK": "OK"}), 201

    except:
        return jsonify([{"failed":"failed"}])

def start_thread():
    thread = threading.Thread(target=whatsapp.start, kwargs={"headless": False}, daemon=True)
    thread.start()

def get_pia_port():
    import subprocess

    # piactl_path = r"C:\Program Files\Private Internet Access\piactl.exe"
    # piactl_path = "/Applications/Private Internet Access.app/Contents/MacOS/piactl"
    piactl_path = "/usr/local/bin/piactl"  # Adjust this based on where it's installed



    result = subprocess.run(
        [piactl_path, "get", "portforward"],
        capture_output=True,
        text=True
    )

    if result.returncode != 0:
        raise RuntimeError(result.stderr.strip())

    return result.stdout.strip()


if __name__ == '__main__':
    # time.sleep(10)
    # whatsapp.send_message_to_contact("Mars","hallo mars dies ist ein test")

    # append_key("Hallo","Welt")
    # print(get_key("Hallo"))

    start_thread()
    context = ('cert.pem', 'key.pem')  # (cert, key)
    port = get_pia_port()
    print(port)
    if(port == "Inactive"):
        print("Turn on VPN")
    else:
        app.run(
            host='0.0.0.0',
            port=port,
            debug=False,
            ssl_context=context
        )