import psycopg2 as sql
from os import getenv
from src.logger import log


def create(name="users_db"):
    conn = sql.connect(
        dbname=getenv("DBNAME_PGRS"),
        user=getenv("USER_PGRS"),
        password=getenv("PASSWORD_PGRS"),
        host=getenv("HOST_PGRS"),
        port=getenv("PORT_PGRS")
    )

    cursor = conn.cursor()
    conn.autocommit = True

    try:
        cursor.execute(f"CREATE DATABASE {name}")
    except (Exception, sql.DatabaseError) as error:
        log.warning("database creation failed, database already exists")

    cursor.close()
    conn.close()

def connect(name="users_db") -> tuple:
    conn = sql.connect(
        dbname=getenv("DBNAME_PGRS"),
        user=getenv("USER_PGRS"),
        password=getenv("PASSWORD_PGRS"),
        host=getenv("HOST_PGRS"),
        port=getenv("PORT_PGRS")
    )

    conn.autocommit = True
    cur = conn.cursor()

    cur.execute('''
                    CREATE TABLE IF NOT EXISTS users (
                        id SERIAL PRIMARY KEY,
                        login VARCHAR(32) NOT NULL,
                        password VARCHAR(32) NOT NULL,
                        info VARCHAR(100)
                    );
                ''')

    return conn, cur

def register(database, login, password, info):
    conn, cursor = database
    cursor.execute("INSERT INTO users (login, password, info) VALUES (%s, %s, %s);", (login, password, info))
    conn.commit()

def check_login(database, login) -> bool:
    conn, cursor = database
    cursor.execute("SELECT * FROM users WHERE login = %s;", (login,))
    return bool(cursor.fetchone())

def check_password(database, login, password):
    conn, cursor = database
    try:
        cursor.execute("SELECT * FROM users WHERE login = %s;", (login,))
        data = cursor.fetchone()
        if bool(data[2] == password):
            return data
        return None
    except:
        log.error("Login failed")
        return None

def data_builder(database, user_id):
    conn, cursor = database
    cursor.execute("SELECT * FROM users WHERE id = %s;", (user_id,))
    user_id, login, password, info = cursor.fetchone()
    return {"user_id": user_id, "login": login, "info": info}
