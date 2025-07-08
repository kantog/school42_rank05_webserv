#!/usr/bin/env python3
import cgi
import cgitb
import json
import os
import time

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

def get_session_user():
    """Get current user from session cookie"""
    # Get cookies from environment (your webserver passes this)
    cookies_string = os.environ.get('HTTP_COOKIE', '')
    
    # Parse cookies to find session_id
    session_id = None
    for cookie in cookies_string.split(';'):
        cookie = cookie.strip()
        if cookie.startswith('session_id='):
            session_id = cookie.split('=', 1)[1]
            break
    
    log_debug(f"Session ID: {session_id}")
    log_debug(f"Cookies: {cookies_string}")

    if not session_id:
        return None
    
    # Check if session exists and is valid
    sessions = load_sessions()
    if session_id not in sessions:
        return None
    
    session_data = sessions[session_id]
    
    # Check if session is expired (1 hour)
    if time.time() - session_data.get('created', 0) > 3600:
        return None
    
    return session_data['username']

def main():
    username = get_session_user()
    
    if not username:
        # Not logged in - redirect to login
        print("Content-Type: text/html")
        print()
        print("""
        <html>
        <head>
            <title>Access Denied</title>
            <style>
                body {
                    font-family: Arial, sans-serif;
                    max-width: 400px;
                    margin: 100px auto;
                    padding: 20px;
                    text-align: center;
                    background-color: #f5f5f5;
                }
                .error-container {
                    background: white;
                    padding: 30px;
                    border-radius: 10px;
                    box-shadow: 0 2px 10px rgba(0,0,0,0.1);
                }
                h1 { color: #dc3545; }
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
            <div class="error-container">
                <h1>Access Denied</h1>
                <p>You need to be logged in to access this page.</p>
                <a href="../login.html" class="login-btn">Login</a>
            </div>
        </body>
        </html>
        """)
        return
    
    # User is logged in - show dashboard
    print("Content-Type: text/html")
    print()
    print(f"""
    <html>
    <head>
        <title>Dashboard</title>
        <style>
            body {{
                font-family: Arial, sans-serif;
                max-width: 600px;
                margin: 50px auto;
                padding: 20px;
                background-color: #f5f5f5;
            }}
            .dashboard-container {{
                background: white;
                padding: 30px;
                border-radius: 10px;
                box-shadow: 0 2px 10px rgba(0,0,0,0.1);
            }}
            h1 {{ color: #4CAF50; text-align: center; }}
            .user-info {{
                background: #e9ecef;
                padding: 15px;
                border-radius: 5px;
                margin: 20px 0;
            }}
            .actions {{
                display: flex;
                gap: 10px;
                justify-content: center;
                margin-top: 30px;
            }}
            .btn {{
                padding: 10px 20px;
                text-decoration: none;
                border-radius: 5px;
                color: white;
                font-weight: bold;
            }}
            .btn-logout {{
                background-color: #dc3545;
            }}
            .btn-logout:hover {{
                background-color: #c82333;
            }}
            .secret-content {{
                background: #d4edda;
                border: 1px solid #c3e6cb;
                padding: 20px;
                border-radius: 5px;
                margin: 20px 0;
            }}
        </style>
    </head>
    <body>
        <div class="dashboard-container">
            <h1>🎉 Secret Dashboard 🎉</h1>
            
            <div class="user-info">
                <h3>Welcome, {username}!</h3>
                <p>You are successfully logged in and can access this protected page.</p>
            </div>
            
            <div class="secret-content">
                <h3>🔒 This is protected content!</h3>
                <p>Only logged-in users can see this secret information:</p>
                <ul>
                    <li>Secret code: <strong>WEBSERV-2024</strong></li>
                    <li>Today's special: <strong>Cookies and Sessions!</strong></li>
                    <li>Your session is valid and working perfectly!</li>
                </ul>
            </div>
            
            <div class="actions">
                <a href="logout.py" class="btn btn-logout">Logout</a>
            </div>
        </div>
    </body>
    </html>
    """)

if __name__ == '__main__':
    main()