from jwt import encode, decode

KEY = "65537"

def create_token(metadata, algorithm='HS256'):
    try:
        return str(encode(metadata, KEY, algorithm=algorithm))
    except Exception as e:
        print(f"Error: token has not been created {e}")
        return None

def decode_token(token_, algorithm='HS256'):
    try:
        return decode(token_, KEY, algorithms=[algorithm])
    except Exception as e:
        print(f"Error: token has not been decoded {e}")
        return None
