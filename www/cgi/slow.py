#!/usr/bin/env python3
import os
import sys
import time

time.sleep(10)  # Simuleer trage CGI

print("Content-Type: text/html\n")

print("<html><body>")
print("<h1>Start...</h1>")
method = os.environ.get("REQUEST_METHOD", "")

if method == "GET":
    query = os.environ.get("QUERY_STRING", "")
    print(f"<p>Method: GET</p><p>Query: {query}</p>")

elif method == "POST":
    length = int(os.environ.get("CONTENT_LENGTH", 0))
    print(f"Content-Length: {length}")
    body = sys.stdin.read(length)
    print(f"<p>Method: POST</p><p>Body: {body}</p>")

else:
    print(f"<p>Method: {method}</p><p>Niet ondersteund.</p>")

print("<h1>Fack yeah!</h1>")
print('  <a href="/index.html">Terug naar home</a>')
print("</body></html>")
