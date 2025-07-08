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
        print("Content-Type: text/txt")
        print()
        print("gast")
        return
    
    # User is logged in - show dashboard
    print("Content-Type: text/txt")
    print()
    print(f"""{username}""")

if __name__ == '__main__':
    main()