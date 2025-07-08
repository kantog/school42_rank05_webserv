#!/usr/bin/env python3
import cgitb
from util import load_sessions, log_debug, save_sessions, get_session_id

cgitb.enable()

def main():

    session_id = get_session_id()
    if session_id:
        sessions = load_sessions()
        if session_id in sessions:
            del sessions[session_id]
            save_sessions(sessions)
    
    print("Set-Cookie: session_id=; Path=/; HttpOnly; Expires=Thu, 01 Jan 1970 00:00:00 GMT")
    print("Content-Type: text/html")
    print()
    
    try:
        with open('logout.html', 'r') as f:
            print(f.read())
    except  Exception as e:
        log_debug(f"Error: {e}")

if __name__ == '__main__':
    main()