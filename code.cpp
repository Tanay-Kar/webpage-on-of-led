#if defined(ESP32)
  #include <WiFi.h>
  #include <WebServer.h>
  WebServer server(80);
  const int ledPin = 2; // Pin 2 on ESP32 UNO headers
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  ESP8266WebServer server(80);
  const int ledPin = D2; // Pin D2 on ESP8266 UNO headers
#else
  #error "Please select an ESP32 or ESP8266 board under Tools > Board"
#endif

// Replace with your Wi-Fi or Mobile Hotspot credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Embedded Webpage (Modern UI)
const char* html_page = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Smart LED Controller</title>
  <style>
    :root {
      --bg-color: #0f172a;
      --card-bg: #1e293b;
      --text-main: #f8fafc;
      --text-sub: #94a3b8;
      --accent-green: #10b981;
      --accent-glow: rgba(16, 185, 129, 0.35);
      --switch-off: #334155;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
      background-color: var(--bg-color);
      color: var(--text-main);
      display: flex;
      justify-content: center;
      align-items: center;
      min-height: 100vh;
      padding: 20px;
    }
    .card {
      background: var(--card-bg);
      padding: 35px 25px;
      border-radius: 24px;
      box-shadow: 0 20px 25px -5px rgba(0, 0, 0, 0.5);
      border: 1px solid rgba(255, 255, 255, 0.08);
      text-align: center;
      width: 100%;
      max-width: 320px;
      transition: all 0.3s ease;
    }
    .icon-box {
      width: 60px;
      height: 60px;
      background: rgba(255, 255, 255, 0.05);
      border-radius: 50%;
      display: flex;
      align-items: center;
      justify-content: center;
      margin: 0 auto 15px auto;
      font-size: 28px;
    }
    h2 { font-size: 1.4rem; font-weight: 600; margin-bottom: 4px; }
    p.subtitle { color: var(--text-sub); font-size: 0.85rem; margin-bottom: 25px; }
    
    .status-badge {
      display: inline-flex;
      align-items: center;
      gap: 8px;
      padding: 6px 14px;
      background: rgba(255, 255, 255, 0.05);
      border-radius: 20px;
      font-size: 0.85rem;
      font-weight: 500;
      color: var(--text-sub);
      margin-bottom: 30px;
      transition: all 0.3s ease;
    }
    .dot {
      width: 8px;
      height: 8px;
      border-radius: 50%;
      background-color: #64748b;
      transition: all 0.3s ease;
    }
    
    .switch-container {
      display: flex;
      justify-content: center;
      align-items: center;
    }
    .switch {
      position: relative;
      display: inline-block;
      width: 100px;
      height: 52px;
    }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider {
      position: absolute;
      cursor: pointer;
      top: 0; left: 0; right: 0; bottom: 0;
      background-color: var(--switch-off);
      transition: .4s cubic-bezier(0.4, 0, 0.2, 1);
      border-radius: 52px;
      border: 1px solid rgba(255, 255, 255, 0.1);
    }
    .slider:before {
      position: absolute;
      content: "";
      height: 40px;
      width: 40px;
      left: 6px;
      bottom: 5px;
      background-color: #ffffff;
      transition: .4s cubic-bezier(0.4, 0, 0.2, 1);
      border-radius: 50%;
      box-shadow: 0 4px 6px -1px rgba(0,0,0,0.3);
    }
    input:checked + .slider {
      background-color: var(--accent-green);
      box-shadow: 0 0 20px var(--accent-glow);
    }
    input:checked + .slider:before {
      transform: translateX(48px);
    }
    
    .active-mode .dot {
      background-color: var(--accent-green);
      box-shadow: 0 0 8px var(--accent-green);
    }
    .active-mode .status-badge {
      color: var(--accent-green);
      background: rgba(16, 185, 129, 0.12);
    }
    
    .footer-text {
      margin-top: 25px;
      font-size: 0.75rem;
      color: #475569;
      letter-spacing: 1px;
      text-transform: uppercase;
    }
  </style>
</head>
<body>
  <div class="card" id="card">
    <div class="icon-box">💡</div>
    <h2>LED Switch</h2>
    <p class="subtitle">ESP UNO Wireless Controller</p>
    
    <div class="status-badge">
      <span class="dot" id="dot"></span>
      <span id="statusText">State: OFF</span>
    </div>

    <div class="switch-container">
      <label class="switch">
        <input type="checkbox" id="ledToggle" onchange="toggleLED()">
        <span class="slider"></span>
      </label>
    </div>

    <div class="footer-text">Smart IoT Node</div>
  </div>

  <script>
    function toggleLED() {
      var isChecked = document.getElementById('ledToggle').checked;
      var state = isChecked ? 'on' : 'off';
      var card = document.getElementById('card');
      var statusText = document.getElementById('statusText');

      if (isChecked) {
        card.classList.add('active-mode');
        statusText.innerText = 'State: ON';
      } else {
        card.classList.remove('active-mode');
        statusText.innerText = 'State: OFF';
      }

      fetch('/' + state);
    }
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // LED turns off on startup

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Wi-Fi Connected!");
  Serial.print("Webpage IP Address: ");
  Serial.println(WiFi.localIP());

  // Web server routes
  server.on("/", []() {
    server.send(200, "text/html", html_page);
  });

  server.on("/on", []() {
    digitalWrite(ledPin, HIGH);
    server.send(200, "text/plain", "LED turned ON");
    Serial.println("LED turned ON");
  });

  server.on("/off", []() {
    digitalWrite(ledPin, LOW);
    server.send(200, "text/plain", "LED turned OFF");
    Serial.println("LED turned OFF");
  });

  server.begin();
  Serial.println("HTTP Server Started");
}

void loop() {
  server.handleClient();
}
