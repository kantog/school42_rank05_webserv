#!/usr/bin/env python3
from util import get_session_user

def main():
    username = get_session_user()
    
    print("Content-Type: text/txt")
    print()
    if not username:
        print("gast")
        return    
    print(f"{username}")

if __name__ == '__main__':
    main()