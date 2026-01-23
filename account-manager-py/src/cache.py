import redis
import token2
from os import getenv
from exceptions import CacheException
from src.logger import log

cache = redis.Redis(host=getenv("HOST_RDS"), port=getenv("PORT_RDS"), db=0)

def create_session(user_id, metadata):
    token_ = token2.create_token(metadata)
    if token_:
        cache.set(token_, str(user_id), ex=1800)
        return token_
    log.error("Token hasn't been created")
    raise CacheException("Token hasn't been created")

def check_session(token_) -> bool:
    try:
        return bool(cache.get(token_))
    except:
        return False

def delete_session(token_):
    log.error("Token has been deleted")
    cache.delete(token_)
