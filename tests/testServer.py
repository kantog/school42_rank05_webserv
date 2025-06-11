import http.client

conn = http.client.HTTPConnection("localhost", 8000)
conn.putrequest("POST", "/")
conn.putheader("Transfer-Encoding", "chunked")
conn.putheader("Content-Type", "text/plain")
conn.endheaders()

chunks = ["Hallo ", "wereld!", " Meer data."]
for chunk in chunks:
    chunk_bytes = chunk.encode()
    conn.send(hex(len(chunk_bytes))[2:].encode() + b"\r\n")
    conn.send(chunk_bytes + b"\r\n")

conn.send(b"0\r\n\r\n")  # Einde van chunked body
response = conn.getresponse()
print(response.status, response.read())
