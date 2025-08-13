import redis
import token2

cache = redis.Redis(host='localhost', port=6379, db=0)

def create_session(user_id, metadata):
    token_ = token2.create_token(metadata)
    if token_:
        cache.set(token_, str(user_id), ex=1800)
        return token_
    else:
        return None

def check_session(token_) -> bool:
    try:
        return bool(cache.get(token_))
    except:
        print("Error: there is not such session")
        return False

def delete_session(token_):
    cache.delete(token_)