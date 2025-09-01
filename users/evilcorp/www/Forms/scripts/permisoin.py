#!/usr/bin/env python3

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
print('  <a href="../index.html">Go back to CGI home</a>')
print('  <a href="/index.html">Go back to home</a>')
print("  </main></body></html>")
