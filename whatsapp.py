from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.firefox.service import Service
from webdriver_manager.firefox import GeckoDriverManager
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
import pyperclip

import time
import os


def get_driver():
    options = webdriver.FirefoxOptions()
    options.add_argument("--width=1200")
    options.add_argument("--height=800")

    # Persistent profile path
    profile_path = os.path.abspath("./ff_profile")
    if not os.path.exists(profile_path):
        os.makedirs(profile_path)
        print(f"Created profile folder: {profile_path}")

    # Tell Firefox to use this profile
    options.add_argument(f'--profile={profile_path}')

    # Optional: Reduce detection
    options.set_preference("dom.webdriver.enabled", False)
    options.set_preference("useAutomationExtension", False)

    service = Service(GeckoDriverManager().install())
    driver = webdriver.Firefox(service=service, options=options)
    return driver


def wait_for_login(driver, timeout=180):
    wait = WebDriverWait(driver, timeout)
    print("Opening WhatsApp Web...")

    # Check if already logged in
    try:
        print("Checking if already logged in...")
        wait.until(EC.presence_of_element_located((By.CSS_SELECTOR, "div[role='grid']")))
        print("Already logged in! Skipping QR code.")
        return True
    except:
        print("Not logged in. Please scan the QR code with your phone...")
        try:
            wait.until(EC.presence_of_element_located((By.CSS_SELECTOR, "div[role='grid']")))
            print("Login successful!")
            return True
        except:
            print("Login timed out.")
            return False


def open_contact(driver, contact_name):
    chat_selector = f"span[title='{contact_name}']"
    try:
        chat = WebDriverWait(driver, 10).until(
            EC.element_to_be_clickable((By.CSS_SELECTOR, chat_selector))
        )
        chat.click()
        time.sleep(1.5)
        return True
    except Exception as e:
        print(f"Contact '{contact_name}' not found. Make sure the name matches exactly.")
        return False


def get_chat_messages(driver, contact_name, max_messages=500):
    messages = []
    try:
        # Wait until the chat input box appears → chat is loaded
        WebDriverWait(driver, 10).until(
            EC.presence_of_element_located((By.CSS_SELECTOR, "div[role='textbox']"))
        )
        time.sleep(1)

        # All message bubbles
        msg_bubbles = driver.find_elements(By.CSS_SELECTOR, "div.message-in, div.message-out")

        for msg in msg_bubbles[-max_messages:]:
            try:
                # ----- sender -----
                is_outgoing = "message-out" in msg.get_attribute("class")
                sender = "You" if is_outgoing else contact_name

                # ----- text (caption) -----
                text_parts = msg.find_elements(By.CSS_SELECTOR, "span.selectable-text span")
                text = " ".join([t.text.strip() for t in text_parts if t.text.strip()])

                # ----- image detection -----
                has_image = bool(msg.find_elements(By.CSS_SELECTOR, "img, div[data-animate='true']"))
                img_tag = " [img]" if has_image else ""

                # ----- combine -----
                final_msg = f"{text}{img_tag}".strip()
                if final_msg or has_image:          # include pure-image messages
                    messages.append((sender, final_msg or "[img]"))
            except:
                continue
    except Exception as e:
        print(f"Warning: Could not read messages – {e}")

    return messages

def send_message(driver, text):
    try:
        # Wait for input box
        input_box = WebDriverWait(driver, 10).until(
            EC.element_to_be_clickable((By.CSS_SELECTOR, "div[contenteditable='true'][data-tab='10']"))
        )

        # Copy text to clipboard
        pyperclip.copy(text)


        # Click input, paste, send
        input_box.click()
        ActionChains(driver).key_down(Keys.CONTROL).send_keys('v').key_up(Keys.CONTROL).perform()
        time.sleep(0.3)
        input_box.send_keys(Keys.ENTER)

        print(f"Sent: {text}")
        return True

    except Exception as e:
        print(f"Send failed: {e}")
        return False

def main():
    driver = get_driver()
    driver.get("https://web.whatsapp.com/")

    if not wait_for_login(driver):
        driver.quit()
        return

    current_contact = None  # Track last opened contact

    print("\nWhatsApp Web Ready!")
    print("• Enter a contact name to view messages")
    print("• Type 'send' to send messages to the last opened contact")
    print("• Type 'exit' to quit\n")

    while True:
        action = input("Enter contact, 'send', or 'exit': ").strip()

        if action.lower() == "exit":
            break

        elif action.lower() == "send":
            if not current_contact:
                print("No contact selected yet. Open a chat first.")
                continue

            print(f"\n--- Sending to: {current_contact} ---")
            print("Type your message and press Enter (type 'back' to return)\n")

            while True:
                msg = input(">> ").strip()
                if msg.lower() == "back":
                    break
                if not msg:
                    continue

                if send_message(driver, msg):
                    print(f"Sent: {msg}")
                else:
                    print("Failed to send. Is the chat open?")
                time.sleep(1)  # Avoid flooding

        else:
            # Open contact and show messages
            if open_contact(driver, action):
                current_contact = action
                print(f"Opened chat with '{current_contact}'. Loading messages...")
                time.sleep(2)
                msgs = get_chat_messages(driver, current_contact)
                print(f"\nLast {len(msgs)} messages:\n")
                for i, (sender, text) in enumerate(msgs, 1):
                    print(f"{i:3d} | {sender:8} | {text}")
            else:
                print("Contact not found. Check spelling and try again.")

    input("\nPress Enter to close browser...")
    driver.quit()
    print("Session saved. See you next time!")

if __name__ == "__main__":
    main()