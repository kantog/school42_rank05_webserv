<?php
header('Content-Type: text/html; charset=utf-8');
if (php_sapi_name() == "cli") {
    $_SERVER['REQUEST_METHOD'] = 'GET';
}
?>
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>From PHP</title>
  <link rel="stylesheet" href="../style.css" />
</head>
<body>

  <main>
    <h1>Cool from PHP</h1>
    <?php
    $method = $_SERVER['REQUEST_METHOD'];

    if ($method === 'GET') {
        $query = $_SERVER['QUERY_STRING'];
        echo "<p>Method: GET</p><p>Query: $query</p>";
    } elseif ($method === 'POST') {
      echo "<p>Method: POST</p>";
      echo "<p>Body:</p><ul>";
      foreach ($_POST as $key => $value) {
          echo "<li><strong>$key:</strong> " . htmlspecialchars($value) . "</li>";
      }
      echo "</ul>";
    } else {
        echo "<p>Method: $method</p><p>Not supported.</p>";
    }
    ?>
    <h1>Heck yeah!</h1>
    <a href="../index.html">Go back to CGI home</a>
    <a href="/index.html">Go back to home</a>
  </main>
</body>
</html>
