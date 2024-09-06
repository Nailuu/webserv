import datetime

current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

html_output = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Current Time</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            text-align: center;
            margin-top: 50px;
        }}
        h1 {{
            color: #333;
        }}
        .time {{
            font-size: 24px;
            color: #555;
        }}
    </style>
</head>
<body>
    <h1>Current Server Time</h1>
    <div class="time">{current_time}</div>
</body>
</html>
"""

print(html_output)
    