from fastapi import FastAPI, Body, Response, Request
from starlette.responses import JSONResponse

import DB as SQL
import cache
import token2

import logging

logging.basicConfig(level=logging.DEBUG)
log = logging.getLogger("uvicorn.error")

SQL.create()
database = SQL.connect()

app = FastAPI()

@app.post("/register")
def register(data: dict = Body(...)):
    if SQL.check_login(database, data["login"]):
        log.info("This login is already registered")
        return JSONResponse(status_code=400, content={"message": "This login is already registered"})
    else:
        SQL.register(database, data["login"], data["password"], data["info"])
        log.info(f"{data["login"]} has been registered successfully")
        return JSONResponse(status_code=200, content={"message": f"{data["login"]} has been registered successfully"})

@app.post("/login")
def login(response: Response, data = Body({})):
    try:
        user = SQL.check_password(database, data["login"], data["password"])
        if user:
            user_id, login_, password, info = user
            token_ = cache.create_session(user_id, SQL.data_builder(database, user_id))
            if token_:
                response.set_cookie(
                    key="token",
                    value=token_,
                    max_age=1800,
                    httponly=True,
                    secure=True,
                    samesite="strict"
                )
                log.info("Token has been set successfully")
            else:
                log.info("Token has not been created")
        else:
            log.info("This login is not registered")
    except Exception as e:
        log.info(f"Error: token has not been created for {user_id}, e{e}")

def autorization(token_) -> bool:
    try:
        if not cache.check_session(token_):
            print(f"Error: your session is expired")
            return False
        return True
    except:
        print(f"Error: your session is expired")
        return False

@app.get("/logout")
def logout(request: Request, response: Response):
    token_ = request.cookies.get("token")
    if not autorization(token_):
        return JSONResponse({"error": "You has been already logged out"}, status_code=401)
    else:
        response.delete_cookie("token", path="/")
        cache.delete_session(token_)
        return JSONResponse({"info": "You have been logged out"}, status_code=200)

@app.get("/me")
def me(request: Request):
    token_ = request.cookies.get("token")
    if not autorization(token_):
        return JSONResponse({"error": "Invalid token"}, 401)
    else:
        data = token2.decode_token(token_)
        user_id, login_, info = data["user_id"], data["login"], data["info"]
        return {"info": info}

@app.get("/active")
def active(request: Request):
    token_ = request.cookies.get("token")
    if not autorization(token_):
        print("Invalid token")
        return JSONResponse({"error": "Invalid token"}, 401)
    else:
        data = token2.decode_token(token_)
        user_id, login_, info = data["user_id"], data["login"], data["info"]
        return {"info": login_}