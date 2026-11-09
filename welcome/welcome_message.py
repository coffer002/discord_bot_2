import os
import ctypes
import requests
import xml.etree.ElementTree as ET
from utils.logger import log

LIB_PATH = "/usr/local/lib/libwelcome.so"

c_lib = None
try:
    c_lib = ctypes.CDLL(LIB_PATH)
    c_lib.generate_welcome_card.argtypes = [
        ctypes.c_char_p,
        ctypes.c_char_p,
        ctypes.c_char_p,
        ctypes.c_char_p,
        ctypes.c_char_p
    ]
    c_lib.generate_welcome_card.restype = ctypes.c_int
except Exception as e:
    log("welcome.py", f"Erro ao carregar {LIB_PATH}: {e}")

async def welcome_message(user):
    log("welcome.py", f"Gerando boas-vindas para: {user.name}")


    main_text_string = "Boas vindas {user.mention}"
    subtext_string = "Seja bem-vindo(a), {user.name}!"


    xml_path = "/app/config.xml"
    if os.path.exists(xml_path):
        try:
            tree = ET.parse(xml_path)
            root = tree.getroot()

            node_text = root.find('welcome_text')
            if node_text is not None and node_text.text:
                main_text_string = node_text.text

            node_subtext = root.find('welcome_subtext')
            if node_subtext is not None and node_subtext.text:
                subtext_string = node_subtext.text
        except Exception as e:
            log("welcome.py", f"Erro ao ler config.xml: {e}")


    chat_mention = f"<@{user.id}>"
    username = user.name
    nickname = user.display_name


    text = main_text_string.replace("{user.mention}", chat_mention)
    text = text.replace("{user.name}", username)
    text = text.replace("{user.nickname}", nickname)

    subtext_string = subtext_string.replace("{user.name}", username)
    subtext_string = subtext_string.replace("{user.nickname}", nickname)

    banner_url = os.getenv("WELCOME_BANNER_URL", "")
    bg_path = f"/tmp/banner_{user.id}.png"
    avatar_path = f"/tmp/avatar_{user.id}.png"
    output_path = f"/tmp/welcome_{user.id}.png"


    if banner_url and not os.path.exists(bg_path):
        try:
            resp = requests.get(banner_url, timeout=10)
            if resp.status_code == 200:
                with open(bg_path, "wb") as f:
                    f.write(resp.content)
        except Exception as e:
            log("welcome.py", f"Erro ao baixar imagem de fundo: {e}")

    if not os.path.exists(bg_path):
        bg_path = ""

#avatar
    try:
        await user.display_avatar.with_format("png").save(avatar_path)
    except Exception as e:
        log("welcome.py", f"Erro ao salvar avatar: {e}")
        avatar_path = ""


    if c_lib:
        username_bytes = username.encode('utf-8')
        subtext_bytes = subtext_string.encode('utf-8')
        bg_bytes = bg_path.encode('utf-8')
        avatar_bytes = avatar_path.encode('utf-8')
        output_bytes = output_path.encode('utf-8')

        result = c_lib.generate_welcome_card(
            bg_bytes, avatar_bytes, username_bytes, subtext_bytes, output_bytes
        )

        if result == 0 and os.path.exists(output_path):
            log("welcome.py", f"Imagem gerada via C com sucesso: {output_path}")
            return text, output_path

    return text, None