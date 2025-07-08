#!/usr/bin/env python3
import cgi
import cgitb
import hashlib
import uuid
import time

from util import hash_password, load_sessions, load_users, save_sessions

cgitb.enable()

def cleanup_expired_sessions(sessions):
    current_time = time.time()
    expired_sessions = []
    
    for session_id, session_data in sessions.items():
        if current_time - session_data.get('created', 0) > 3600:  # 1 hour
            expired_sessions.append(session_id)
    
    for session_id in expired_sessions:
        del sessions[session_id]

def main():
    form = cgi.FieldStorage()
    username = form.getvalue('username', '').strip()
    password = form.getvalue('password', '')
    
    if not username or not password:
        print("Content-Type: text/html")
        print()
        print("""
        <html>
        <head><title>Error</title></head>
        <body>
        <h1>Login Error</h1>
        <p>Username and password are required!</p>
        <a href="/login.html">Go back</a>
        </body>
        </html>
        """)
        return
    
    users = load_users()
    if username not in users or users[username]['password'] != hash_password(password):
        print("Content-Type: text/html")
        print()
        print("""
        <html>
        <head><title>Error</title></head>
        <body>
        <h1>Login Error</h1>
        <p>Invalid username or password!</p>
        <a href="../login.html">Go back</a>
        </body>
        </html>
        """)
        return
    
    sessions = load_sessions()
    cleanup_expired_sessions(sessions)
    
    session_id = str(uuid.uuid4())
    sessions[session_id] = {
        'username': username,
        'created': time.time()
    }
    
    save_sessions(sessions)
    
    print(f"Set-Cookie: session_id={session_id}; Path=/") #; HttpOnly
    print("Content-Type: text/html")
    print()
    print(f"""
    <html>
    <head>
        <title>Login Successful</title>
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
            .dashboard-btn {{
                display: inline-block;
                padding: 10px 20px;
                background-color: #4CAF50;
                color: white;
                text-decoration: none;
                border-radius: 5px;
                margin: 10px;
            }}
            .dashboard-btn:hover {{
                background-color: #45a049;
            }}
        </style>
    </head>
    <body>
        <div class="success-container">
            <h1>Login Successful!</h1>
            <p>Welcome back, {username}!</p>
            <p>You are now logged in.</p>
            <a href="/" class="dashboard-btn">Go to Home</a>
        </div>
    </body>
    </html>
    """)

if __name__ == '__main__':
    main()