# Account Manager API

This project is a backend service for user management, providing registration, authentication, and session management. It is implemented using **FastAPI**, with **PostgreSQL** for user data storage, **Redis** for session storage, and **JWT** for token-based authentication.

---

## Features

* User registration
* Authentication using login and password
* Session management with Redis
* JWT-based authentication
* Cookie-based authentication (`HttpOnly`)
* Retrieval of current user information
* User logout with session invalidation

---

## Technology Stack

* **Python 3.10+**
* **FastAPI**
* **PostgreSQL** (`psycopg2`)
* **Redis**
* **JWT** (`PyJWT`)
* **python-dotenv**

---

## Environment Variables

The project uses a `.env` file (which should not be committed to the repository).

### Example `.env`

```env
# PostgreSQL
DBNAME_PGRS=postgres
USER_PGRS=postgres
PASSWORD_PGRS=postgres
HOST_PGRS=localhost
PORT_PGRS=5432

# Redis
HOST_RDS=localhost
PORT_RDS=6379

# JWT
KEY_TKN=super_secret_key
```

---

## Running the Project

### 1. Install dependencies

```bash
pip install -r requirements.txt
```

### 2. Start PostgreSQL and Redis

These services can be run locally or using Docker.

### 3. Run the server

```bash
uvicorn src.main:app --reload
```

The API will be available at:

```
http://127.0.0.1:8000
```

Swagger documentation:

```
http://127.0.0.1:8000/docs
```

---

## API Endpoints

### POST `/register`

Registers a new user.

**Request Body:**

```json
{
  "login": "user",
  "password": "password",
  "info": "additional information"
}
```

### POST `/login`

Authenticates a user.

**Request Body:**

```json
{
  "login": "user",
  "password": "password"
}
```

A successful login will set a `token` cookie.

### GET `/me`

Returns information about the currently authenticated user.
Requires a valid `token` cookie.

### GET `/active`

Returns the login of the currently authenticated user.

### GET `/logout`

Deletes the authentication cookie and removes the session from Redis.

---

## Security Considerations

* Passwords should be stored as hashes using secure algorithms such as bcrypt.
* JWT tokens store minimal necessary information.
* Cookies are set with the `HttpOnly` flag to prevent access from client-side scripts.
* Sessions have a limited lifetime (30 minutes).

