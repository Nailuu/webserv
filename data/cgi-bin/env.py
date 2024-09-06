import os

env_keys = [
    "CONTENT_TYPE", "CONTENT_LENGTH", "HTTP_COOKIE", "HTTP_ACCEPT",
    "HTTP_USER_AGENT", "PATH_INFO", "QUERY_STRING", "REMOTE_ADDR",
    "REQUEST_METHOD", "SCRIPT_FILENAME", "SCRIPT_NAME", "SERVER_NAME",
    "SERVER_SOFTWARE", "SERVER_PROTOCOL", "CGI_SOFTWARE", "SERVER_PORT",
    "GATEWAY_INTERFACE"
]

html_output = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Environment Variables</title>
    <style>
        table {
            width: 50%;
            border-collapse: collapse;
            margin: 20px 0;
            font-size: 1.2em;
            text-align: left;
        }
        table, th, td {
            border: 1px solid black;
        }
        th, td {
            padding: 10px;
        }
        th {
            background-color: #f2f2f2;
        }
    </style>
</head>
<body>
    <h1>Environment Variables</h1>
    <table>
        <tr>
            <th>Variable</th>
            <th>Value</th>
        </tr>
"""

for key in env_keys:
    value = os.environ.get(key, "Not Set")
    html_output += f"""
        <tr>
            <td>{key}</td>
            <td>{value}</td>
        </tr>
    """

html_output += """
    </table>
</body>
</html>
"""

print(html_output)