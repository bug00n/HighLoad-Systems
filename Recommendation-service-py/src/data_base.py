import psycopg2 as sql

class DataBase:
    def __init__(self, port, name):
        self.name = name
        self.port = port
        self.address = "0.0.0.0"

        conn = sql.connect(
            dbname="postgres",
            user="postgres",
            password="postgres",
            host="0.0.0.0",
            port="5432"
        )

        cursor = conn.cursor()
        cursor.autocommit = True

        try:
            cursor.execute(f"CREATE DATABASE {name}")
        except Exception as e:
            pass

        cursor.close()
        conn.close()