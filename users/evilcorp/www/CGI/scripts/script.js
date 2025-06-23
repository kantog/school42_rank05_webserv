#!/usr/bin/env node

const fs = require('fs');

function getStdin(callback) {
  const stdin = process.stdin;
  let data = '';
  stdin.setEncoding('utf8');
  stdin.on('data', chunk => data += chunk);
  stdin.on('end', () => callback(data));
}

console.log("Content-Type: text/html\n");

console.log(`
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>From JavaScript</title>
  <link rel="stylesheet" href="../style.css" />
</head>
<body>
  <main>
    <h1>Cool from JavaScript</h1>
`);

const method = process.env.REQUEST_METHOD;

if (method === "GET") {
  const query = process.env.QUERY_STRING || '';
  console.log(`<p>Method: GET</p><p>Query: ${query}</p>`);
} else if (method === "POST") {
  const length = parseInt(process.env.CONTENT_LENGTH || '0');
  getStdin((body) => {
    console.log(`<p>Method: POST</p><p>Body: ${body}</p>`);
    finish();
  });
} else {
  console.log(`<p>Method: ${method}</p><p>Not supported.</p>`);
  finish();
}

function finish() {
  console.log(`
    <h1>Heck yeah!</h1>
    <a href="../index.html">Go back to CGI home</a>
    <a href="/index.html">Go back to home</a>
  </main>
</body>
</html>
`);
}
