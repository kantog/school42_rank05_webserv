#!/usr/bin/env python3
import os
import sys

# dit werkt niet
import time

print("Content-Type: text/html\n")

print('''
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>From Python</title>
  <link rel="stylesheet" href="../style.css" />
</head>
''')
print("<body>")
print('  <main>')
print("<h1>Cool from Python</h1>")
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
    print(f"<p>Method: {method}</p><p>Not supported.</p>")

print("<h1>Heck yeah!</h1>")
print('  <a href="../index.html">Go back to CGI home</a>')
print('  <a href="/index.html">Go back to home</a>')
print("  </main></body></html>")
