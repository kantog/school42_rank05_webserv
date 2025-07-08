#!/usr/bin/env python3
import cgi
import cgitb
import json
from util import hash_password, load_users, log_debug

cgitb.enable()


def save_users(users):
    try:
        with open('users.json', 'w') as f:
            json.dump(users, f, indent=4)
    except Exception as e:
        log_debug(f"FOUT bij opslaan: {e}")

def main():

    form = cgi.FieldStorage()
    username = form.getvalue('username', '').strip()
    password = form.getvalue('password', '')
    confirm_password = form.getvalue('confirm_password', '')
    
    if not username or not password:
        print("Content-Type: text/html")
        print()
        print("""
        <html>
        <head><title>Error</title></head>
        <body>
        <h1>Error</h1>
        <p>Username and password are required!</p>
        <a href="/create_user.html">Go back</a>
        </body>
        </html>
        """)
        return
    
    if password != confirm_password:
        print("Content-Type: text/html")
        print()
        print("""
        <html>
        <head><title>Error</title></head>
        <body>
        <h1>Error</h1>
        <p>Passwords do not match!</p>
        <a href="/create_user.html">Go back</a>
        </body>
        </html>
        """)
        return
    
    users = load_users()
    if username in users:
        print("Content-Type: text/html")
        print()
        print("""
        <html>
        <head><title>Error</title></head>
        <body>
        <h1>Error</h1>
        <p>Username already exists!</p>
        <a href="/create_user.html">Go back</a>
        </body>
        </html>
        """)
        return
    
    users[username] = {
        'password': hash_password(password)
    }
    
    save_users(users)
    
    print("Content-Type: text/html")
    print()
    print(f"""
    <html>
    <head>
        <title>Account Created</title>
        <style>
            body {{
                font-family: Arial, sans-serif;
                max-width: 400px;
                margin: 100px auto;
                padding: 20px;
                text-align: center;
                background-color: #f5f5f5;
            }}
            .success-container {{
                background: white;
                padding: 30px;
                border-radius: 10px;
                box-shadow: 0 2px 10px rgba(0,0,0,0.1);
            }}
            h1 {{ color: #4CAF50; }}
            .login-btn {{
                display: inline-block;
                padding: 10px 20px;
                background-color: #007bff;
                color: white;
                text-decoration: none;
                border-radius: 5px;
                margin-top: 20px;
            }}
            .login-btn:hover {{
                background-color: #0056b3;
            }}
        </style>
    </head>
    <body>
        <div class="success-container">
            <h1>Account Created Successfully!</h1>
            <p>Welcome, {username}!</p>
            <p>Your account has been created.</p>
            <a href="../login.html" class="login-btn">Login Now</a>
        </div>
    </body>
    </html>
    """)

if __name__ == '__main__':
    main()