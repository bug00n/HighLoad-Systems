from jwt import encode, decode
from os import getenv
from logger import log

KEY = getenv("KEY_TKN")

def create_token(metadata, algorithm='HS256'):
    try:
        return str(encode(metadata, KEY, algorithm=algorithm))
    except Exception as e:
        log.error(f"Token has not been created: {e}")
        return None

def decode_token(token_, algorithm='HS256'):
    try:
        return decode(token_, KEY, algorithms=[algorithm])
    except Exception as e:
        log.error(f"Token has not been created: {e}")
        return None
