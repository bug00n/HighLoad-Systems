from fastapi import FastAPI, Body, Response, Request
from starlette.responses import JSONResponse

from dotenv import load_dotenv
from os import getenv

load_dotenv()

import src.database as SQL
import src.cache as cache
import src.token2 as token
from logging import log

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
    if cache.check_session(token_):
        return True
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
        data = token.decode_token(token_)
        user_id, login_, info = data["user_id"], data["login"], data["info"]
        return {"info": info}

@app.get("/active")
def active(request: Request):
    token_ = request.cookies.get("token")
    if not autorization(token_):
        print("Invalid token")
        return JSONResponse({"error": "Invalid token"}, 401)
    else:
        data = token.decode_token(token_)
        user_id, login_, info = data["user_id"], data["login"], data["info"]
        return {"info": login_}
