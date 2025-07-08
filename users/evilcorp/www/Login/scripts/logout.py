#!/usr/bin/env python3
import cgi
import cgitb
import json
import os

# Enable CGI error reporting
cgitb.enable()

import sys

def log_debug(message):
    sys.stderr.write(f"[DEBUG] {message}\n")


def load_sessions():
    """Load sessions from file"""
    try:
        with open('sessions.json', 'r') as f:
            return json.load(f)
    except:
        return {}

def save_sessions(sessions):
    """Save sessions to file"""
    with open('sessions.json', 'w') as f:
        json.dump(sessions, f)

def main():
    # Get session ID from cookies
    cookies_string = os.environ.get('HTTP_COOKIE', '')
    session_id = None
    
    for cookie in cookies_string.split(';'):
        cookie = cookie.strip()
        if cookie.startswith('session_id='):
            session_id = cookie.split('=', 1)[1]
            break
    
    # Remove session from storage
    if session_id:
        sessions = load_sessions()
        if session_id in sessions:
            del sessions[session_id]
            save_sessions(sessions)
    
    # Clear cookie and redirect
    print("Set-Cookie: session_id=; Path=/; HttpOnly; Expires=Thu, 01 Jan 1970 00:00:00 GMT")
    print("Content-Type: text/html")
    print()
    
    try:
        with open('logout.html', 'r') as f:
            print(f.read())
    except  Exception as e:
        log_debug(f"Error: {e}")

    '''
    print("""
    <html>
    <head>
        <title>Logged Out</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                max-width: 400px;
                margin: 100px auto;
                padding: 20px;
                text-align: center;
                background-color: #f5f5f5;
            }
            .logout-container {
                background: white;
                padding: 30px;
                border-radius: 10px;
                box-shadow: 0 2px 10px rgba(0,0,0,0.1);
            }
            h1 { color: #6c757d; }
            .login-btn {
                display: inline-block;
                padding: 10px 20px;
                background-color: #007bff;
                color: white;
                text-decoration: none;
                border-radius: 5px;
                margin-top: 20px;
            }
            .login-btn:hover {
                background-color: #0056b3;
            }
        </style>
    </head>
    <body>
        <div class="logout-container">
            <h1>👋 Logged Out</h1>
            <p>You have been successfully logged out.</p>
            <p>Your session has been terminated.</p>
            <a href="../login.html" class="login-btn">Login Again</a>
        </div>
    </body>
    </html>
    """)
    '''

if __name__ == '__main__':
    main()