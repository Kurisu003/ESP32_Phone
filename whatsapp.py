# whatsapp.py
import threading
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

# -------------------------------------------------
# Global driver + thread-safety lock
# -------------------------------------------------
_driver = None
_driver_lock = threading.Lock()


def start(headless=False, timeout=180):
    """Start the browser, restore session or show QR, block until logged in."""
    global _driver
    with _driver_lock:
        if _driver is not None:
            print("Driver already running.")
            return

        options = webdriver.FirefoxOptions()
        if headless:
            options.add_argument("--headless")
        options.add_argument("--width=1200")
        options.add_argument("--height=800")

        profile_path = os.path.abspath("./ff_profile")
        os.makedirs(profile_path, exist_ok=True)
        options.add_argument(f'--profile={profile_path}')
        options.set_preference("dom.webdriver.enabled", False)
        options.set_preference("useAutomationExtension", False)

        service = Service(GeckoDriverManager().install())
        _driver = webdriver.Firefox(service=service, options=options)

    # Open WhatsApp Web and wait for login
    _driver.get("https://web.whatsapp.com/")
    print("Opening WhatsApp Web – scan QR if needed…")
    try:
        WebDriverWait(_driver, timeout).until(
            EC.presence_of_element_located((By.CSS_SELECTOR, "div[role='grid']"))
        )
        print("Logged in!")
    except Exception:
        print("Login timed out.")
        stop()
        raise


def stop():
    """Close the browser cleanly."""
    global _driver
    with _driver_lock:
        if _driver:
            _driver.quit()
            _driver = None
            print("Browser closed.")


def _ensure_driver():
    if _driver is None:
        raise RuntimeError("Call whatsapp.start() first.")
    return _driver


# ----------------------------------------------------------------------
# Public API – no driver argument needed
# ----------------------------------------------------------------------
# whatsapp.py  ←  paste this function over the old one
def list_contacts():
    driver = _ensure_driver()
    with _driver_lock:
        try:
            # 2025 selector: every contact is inside data-list-item-id
            items = driver.find_elements(
                By.CSS_SELECTOR,
                "div[data-list-item-id] span[title]"
            )
            contacts = [el.get_attribute("title") for el in items]
            print(f"Found {len(contacts)} contacts")
            return contacts
        except Exception as e:
            print(f"[list_contacts] {e}")
            return []


def _open_contact(contact_name):
    driver = _ensure_driver()
    selector = f"span[title='{contact_name}']"
    try:
        chat = WebDriverWait(driver, 10).until(
            EC.element_to_be_clickable((By.CSS_SELECTOR, selector))
        )
        chat.click()
        time.sleep(1.2)
        return True
    except Exception:
        print(f"Contact '{contact_name}' not found.")
        return False


def get_messages_from_contact(contact_name, max_messages=500):
    if not _open_contact(contact_name):
        return []
    driver = _ensure_driver()
    with _driver_lock:
        msgs = []
        try:
            WebDriverWait(driver, 10).until(
                EC.presence_of_element_located((By.CSS_SELECTOR, "div[role='textbox']"))
            )
            time.sleep(0.8)
            bubbles = driver.find_elements(By.CSS_SELECTOR, "div.message-in, div.message-out")
            for b in bubbles[-max_messages:]:
                try:
                    out = "message-out" in b.get_attribute("class")
                    sender = "You" if out else contact_name
                    txt = " ".join(
                        t.text.strip()
                        for t in b.find_elements(By.CSS_SELECTOR, "span.selectable-text span")
                        if t.text.strip()
                    )
                    img = bool(b.find_elements(By.CSS_SELECTOR, "img, div[data-animate='true']"))
                    final = f"{txt} [img]" if img and txt else txt or "[img]"
                    if final:
                        msgs.append((sender, final))
                except:
                    continue
        except Exception as e:
            print(f"[get_messages] error: {e}")
        return msgs


def send_message_to_contact(contact_name, text):
    if not _open_contact(contact_name):
        return False
    driver = _ensure_driver()
    with _driver_lock:
        try:
            box = WebDriverWait(driver, 10).until(
                EC.element_to_be_clickable(
                    (By.CSS_SELECTOR, "div[contenteditable='true'][data-tab='10']")
                )
            )
            pyperclip.copy(text)
            box.click()
            ActionChains(driver).key_down(Keys.CONTROL).send_keys('v').key_up(Keys.CONTROL).perform()
            time.sleep(0.3)
            box.send_keys(Keys.ENTER)
            print(f"Sent to {contact_name}: {text}")
            return True
        except Exception as e:
            print(f"[send] error: {e}")
            return False