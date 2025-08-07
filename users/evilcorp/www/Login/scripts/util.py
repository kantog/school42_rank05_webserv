import json
import os
import sys
import time
import hashlib

def log_debug(message):
    sys.stderr.write(f"[DEBUG] {message}\n")

def hash_password(password):
    return hashlib.sha256(password.encode()).hexdigest()

def load_sessions():
    try:
        with open('sessions.json', 'r') as f:
            return json.load(f)
    except:
        return {}
    
def save_sessions(sessions):
    with open('sessions.json', 'w') as f:
        json.dump(sessions, f, indent=4)


def load_users():
    try:
        with open('users.json', 'r') as f:
            return json.load(f)
    except:
        return {}

def get_session_id():
    cookies_string = os.environ.get('HTTP_COOKIE', '')
    
    session_id = None
    for cookie in cookies_string.split(';'):
        cookie = cookie.strip()
        if cookie.startswith('session_id='):
            session_id = cookie.split('=', 1)[1]
            break
    return session_id

def get_session_data():

    session_id = get_session_id()
    if not session_id:
        return None
    
    sessions = load_sessions()
    if session_id not in sessions:
        return None
    
    session_data = sessions[session_id]
    
    if time.time() - session_data.get('created', 0) > 3600:
        return None
    
    # log_debug(f"Session data: {session_data}")
    return session_data

def get_session_user():
    data = get_session_data()
    if data:
        return data.get('username', None)
    return None
