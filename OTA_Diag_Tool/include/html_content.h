#ifndef __HTML_CONTENT_H__
#define __HTML_CONTENT_H__

#include <Arduino.h>

#ifdef HTML_CONTENT_IMPLEMENTATION
String html_type = "text/html";
String json_type = "application/json";
String plain_type = "text/plain";

String response_code[] = {"200", "204"};

String html_class = "HTTP/1.1";

String loading_page_html = R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>OTA Diagnostic Tester</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
            color: #333;
        }

        .container {
            max-width: 800px;
            margin: 0 auto;
            background: white;
            border-radius: 15px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.3);
            overflow: hidden;
        }

        .header {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 30px;
            text-align: center;
        }

        .header h1 {
            font-size: 2.5em;
            margin-bottom: 10px;
        }

        .header p {
            font-size: 1.1em;
            opacity: 0.9;
        }

        .status-bar {
            background: #f8f9fa;
            padding: 15px 30px;
            border-bottom: 1px solid #e9ecef;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }

        .status-indicator {
            display: flex;
            align-items: center;
            gap: 10px;
        }

        .status-dot {
            width: 12px;
            height: 12px;
            border-radius: 50%;
            background: #28a745;
            animation: pulse 2s infinite;
        }

        @keyframes pulse {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.5; }
        }

        .content {
            padding: 30px;
        }

        .section {
            margin-bottom: 30px;
        }

        .section-title {
            font-size: 1.3em;
            color: #495057;
            margin-bottom: 15px;
            padding-bottom: 10px;
            border-bottom: 2px solid #667eea;
        }

        .button-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
        }

        .api-button {
            padding: 15px 20px;
            border: none;
            border-radius: 8px;
            font-size: 1em;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s ease;
            position: relative;
            overflow: hidden;
        }

        .api-button::before {
            content: '';
            position: absolute;
            top: 0;
            left: -100%;
            width: 100%;
            height: 100%;
            background: linear-gradient(90deg, transparent, rgba(255,255,255,0.3), transparent);
            transition: left 0.5s;
        }

        .api-button:hover::before {
            left: 100%;
        }

        .btn-primary {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
        }

        .btn-primary:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4);
        }

        .btn-success {
            background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
            color: white;
        }

        .btn-success:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(17, 153, 142, 0.4);
        }

        .btn-danger {
            background: linear-gradient(135deg, #eb3349 0%, #f45c43 100%);
            color: white;
        }

        .btn-danger:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(235, 51, 73, 0.4);
        }

        .btn-info {
            background: linear-gradient(135deg, #2193b0 0%, #6dd5ed 100%);
            color: white;
        }

        .btn-info:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(33, 147, 176, 0.4);
        }

        .response-area {
            background: #f8f9fa;
            border-radius: 8px;
            padding: 20px;
            margin-top: 20px;
            min-height: 100px;
            border: 1px solid #e9ecef;
        }

        .response-title {
            font-weight: 600;
            color: #495057;
            margin-bottom: 10px;
        }

        .response-content {
            font-family: 'Courier New', monospace;
            background: white;
            padding: 15px;
            border-radius: 5px;
            border: 1px solid #dee2e6;
            white-space: pre-wrap;
            word-wrap: break-word;
        }

        .response-table {
            width: 100%;
            border-collapse: collapse;
            font-family: 'Segoe UI', Tahoma, sans-serif;
        }

        .response-table th,
        .response-table td {
            border: 1px solid #dee2e6;
            padding: 8px 10px;
            text-align: left;
            vertical-align: top;
        }

        .response-table th {
            width: 30%;
            background: #f8f9fa;
            color: #495057;
        }

        .loading {
            display: inline-block;
            width: 20px;
            height: 20px;
            border: 3px solid #f3f3f3;
            border-top: 3px solid #667eea;
            border-radius: 50%;
            animation: spin 1s linear infinite;
        }

        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }

        .footer {
            background: #f8f9fa;
            padding: 20px;
            text-align: center;
            color: #6c757d;
            font-size: 0.9em;
        }

        @media (max-width: 600px) {
            .header h1 {
                font-size: 1.8em;
            }
            
            .button-grid {
                grid-template-columns: 1fr;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Diagnostics Control Panel</h1>
            <p>Device Management & Diagnostics</p>
        </div>

        <div class="status-bar">
            <div class="status-indicator">
                <div class="status-dot"></div>
                <span>Device Online</span>
            </div>
            <div id="connection-status">Connected</div>
        </div>

        <div class="content">
            <div class="section">
                <h2 class="section-title">&#x1F697; Vehicle data</h2>
                <div class="button-grid">
                    <button class="api-button btn-info" onclick="callAPI('/diag?service-id=22&sub-id=F190')">
                        &#x1F697; Vehicle Identification Number
                    </button>
                    <button class="api-button btn-info" onclick="callAPI('/diag?service-id=22&sub-id=1001')">
                        &#x1F3CE;&#xFE0F; Engine RPM
                    </button>
                    <button class="api-button btn-info" onclick="callAPI('/diag?service-id=22&sub-id=2001')">
                        &#x1F3CE;&#xFE0F; Current Max speed
                    </button>                    
                </div>
                <label>
                    Maximum speed
                    <input id="max-speed" type="number" min="0" placeholder="Enter speed">
                </label>
                <button class="api-button btn-info" onclick="writeMaxSpeed()">
                    &#x1F3CE;&#xFE0F; Set Maximum Speed
                </button>
            </div>

            <div class="section">
                <h2 class="section-title">Headlight control</h2>
                <div class="button-grid">
                    <button id="headlight-toggle" class="api-button btn-danger" onclick="toggleHeadlight()">
                        🌙 Headlight OFF
                    </button>
                </div>
            </div>

            <div class="section">
                <h2 class="section-title">ECU Control</h2>
                <div class="button-grid">
                    <button class="api-button btn-primary" onclick="resetEcu()">
                        🔄 Restart Device
                    </button>
                    <button class="api-button btn-primary" onclick="callAPI('/diag?service-id=3E&sub-id=00')">
                        📊 Tester Present
                    </button>
                </div>
            </div>

            <div class="response-area">
                <div class="response-title">API Response:</div>
                <div class="response-content" id="response-output">
                    Click a button to see the API response here...
                </div>
            </div>
        </div>

        <div class="footer">
            <p>ESP8266 Web Server | Powered by PlatformIO</p>
        </div>
    </div>

    <script>
        function callAPI(endpoint) {
            const output = document.getElementById('response-output');
            const previousOutput = output.textContent;
            output.innerHTML = '<div class="loading"></div> Loading...';
            
            fetch(endpoint)
                .then(response => {
                    if (response.status === 202 || response.status === 503) {
                        output.textContent = previousOutput;
                        return null;
                    }
                    if (!response.ok) {
                        throw new Error('Network response was not ok');
                    }
                    return response.text();
                })
                .then(data => {
                    if (data !== null) {
                        try {
                            renderJsonTable(JSON.parse(data));
                        } catch (error) {
                            output.textContent = data;
                        }
                    }
                })
                .catch(error => {
                    output.textContent = 'Error: ' + error.message;
                });
        }

        function renderJsonTable(value) {
            const table = document.createElement('table');
            table.className = 'response-table';
            const body = document.createElement('tbody');

            Object.entries(value).forEach(([key, entry]) => {
                const row = document.createElement('tr');
                const name = document.createElement('th');
                const content = document.createElement('td');
                name.textContent = key;

                if (entry !== null && typeof entry === 'object') {
                    content.appendChild(createNestedTable(entry));
                } else {
                    content.textContent = String(entry);
                }

                row.appendChild(name);
                row.appendChild(content);
                body.appendChild(row);
            });

            table.appendChild(body);
            const output = document.getElementById('response-output');
            output.replaceChildren(table);
        }

        function createNestedTable(value) {
            const table = document.createElement('table');
            table.className = 'response-table';
            const body = document.createElement('tbody');

            Object.entries(value).forEach(([key, entry]) => {
                const row = document.createElement('tr');
                const name = document.createElement('th');
                const content = document.createElement('td');
                name.textContent = key;

                if (entry !== null && typeof entry === 'object') {
                    content.appendChild(createNestedTable(entry));
                } else {
                    content.textContent = String(entry);
                }

                row.appendChild(name);
                row.appendChild(content);
                body.appendChild(row);
            });

            table.appendChild(body);
            return table;
        }

        function writeMaxSpeed() {
            const value = document.getElementById('max-speed').value.trim();
            if (!value) {
                document.getElementById('response-output').textContent = 'Enter a maximum speed first.';
                return;
            }

            callAPI('/diag?service-id=2E&sub-id=2001&value=' + encodeURIComponent(value));
        }

        let headlightIsOn = false;

        function updateHeadlightButton() {
            const button = document.getElementById('headlight-toggle');
            button.textContent = headlightIsOn ? '💡 Headlight ON' : '🌙 Headlight OFF';
            button.className = headlightIsOn ? 'api-button btn-success' : 'api-button btn-danger';
        }

        function readHeadlightState() {
            fetch('/diag?service-id=22&sub-id=CF00')
                .then(response => response.json())
                .then(data => {
                    if (data.decoded && data.decoded.headlight_state) {
                        headlightIsOn = data.decoded.headlight_state === 'ON';
                        updateHeadlightButton();
                    }
                    renderJsonTable(data);
                })
                .catch(error => {
                    document.getElementById('response-output').textContent = 'Error: ' + error.message;
                });
        }

        function toggleHeadlight() {
            const requestedValue = headlightIsOn ? '0' : '1';
            fetch('/diag?service-id=2E&sub-id=CF00&value=' + requestedValue)
                .then(response => response.json())
                .then(data => {
                    headlightIsOn = data.decoded && data.decoded.headlight_state === 'ON';
                    updateHeadlightButton();
                    renderJsonTable(data);
                })
                .catch(error => {
                    document.getElementById('response-output').textContent = 'Error: ' + error.message;
                });
        }

        function resetEcu() {
            callAPI('/diag?service-id=11&sub-id=01');
            setTimeout(checkResetStatus, 2500);
        }

        function checkResetStatus() {
            fetch('/reset-status')
                .then(response => response.json())
                .then(status => {
                    if (status.ecu_reset) {
                        alert(status.message);
                    } else {
                        setTimeout(checkResetStatus, 1000);
                    }
                })
                .catch(() => setTimeout(checkResetStatus, 1000));
        }

            readHeadlightState();

    </script>
</body>
</html>)HTML";
#else
extern String html_type;
extern String json_type;
extern String plain_type;
extern String response_code[];
extern String html_class;
extern String loading_page_html;
#endif

#endif // __HTML_CONTENT_H__