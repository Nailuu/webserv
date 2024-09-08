#!/bin/bash

echo "
<!DOCTYPE html>
<html lang='en'>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>Easte Egg</title>
    <style>
        body {
            font-family: 'Arial', sans-serif;
            background: linear-gradient(135deg, #74ebd5, #acb6e5);
            color: #333;
            text-align: center;
            margin: 0;
            padding: 0;
            overflow: hidden;
        }
        
        header {
            padding: 50px;
            font-size: 3em;
            font-weight: bold;
            text-shadow: 2px 2px 5px rgba(0, 0, 0, 0.3);
        }
        
        main {
            padding: 20px;
        }
        
        .button {
            display: inline-block;
            background-color: #ff6b6b;
            border: none;
            color: #fff;
            padding: 15px 30px;
            font-size: 20px;
            text-transform: uppercase;
            letter-spacing: 2px;
            cursor: pointer;
            transition: background-color 0.3s ease, transform 0.2s ease;
            box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
            border-radius: 5px;
        }
        
        .button:hover {
            background-color: #ff4d4d;
            transform: scale(1.1);
        }
        
        .hidden-message {
            margin-top: 20px;
            font-size: 2em;
            color: #333;
            visibility: hidden;
            transition: visibility 0s, opacity 0.5s linear;
            opacity: 0;
        }
        
        .show {
            visibility: visible;
            opacity: 1;
        }
        
        footer {
            margin-top: 50px;
            padding: 10px;
            background-color: rgba(255, 255, 255, 0.1);
            color: #333;
            position: fixed;
            bottom: 0;
            width: 100%;
        }
    </style>
</head>
<body>

<header>Easter Egg</header>

<main>
    <p>Click the button below to reveal a hidden message:</p>
    <button class='button' onclick='showMessage()'>Click Me</button>

    <p class='hidden-message' id='hiddenMessage'>🎉 Surprise! You just unlocked a hidden <a href='https://www.youtube.com/watch?v=dQw4w9WgXcQ'>secret</a>! 🎉</p>
</main>

<script>
    function showMessage() {
        const message = document.getElementById('hiddenMessage');
        message.classList.toggle('show');
    }
</script>

</body>
</html>
"
