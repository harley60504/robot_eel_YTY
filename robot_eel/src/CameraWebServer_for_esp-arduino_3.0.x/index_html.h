  #pragma once
  #include <Arduino.h>

  const char INDEX_HTML[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
  <html lang="zh-TW">
  <head>
  <meta charset="UTF-8">
  <title>ESP32 LX-224 控制面板</title>

  <style>
  body {
    font-family: "Noto Sans TC", Arial, sans-serif;
    background: #eef1f5;
    margin: 0;
  }

  h2 {
    margin: 0;
    padding: 18px;
    color: #fff;
    font-size: 26px;
    text-align: center;
    background: linear-gradient(90deg,#007bff,#0056b3);
    box-shadow: 0 2px 6px rgba(0,0,0,0.2);
  }

  .container {
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(320px, 1fr));
    gap: 16px;
    padding: 16px 24px 16px;
    max-width: 1100px;
    margin: 0 auto;
    justify-content: center;
  }

  .flex-wrap {
    display: flex;
    flex-wrap: wrap;
    justify-content: center;
    gap: 16px;
  }

  .card {
    width: 320px;
    background: white;
    border-radius: 14px;
    padding: 14px 18px;
    box-shadow: 0 3px 10px rgba(0,0,0,0.12);
    border-top: 5px solid #007bff;
    transition: 0.25s;
  }
  .card:hover { transform: translateY(-4px); }

  .card h3 {
    margin-top: 0;
    margin-bottom: 10px;
    font-size: 22px;
    border-left: 6px solid #007bff;
    padding-left: 10px;
  }

  button, input, select {
    font-size: 16px;
    padding: 6px 10px;
    border-radius: 8px;
    border: 1px solid #ccc;
    margin: 3px 0;
  }

  button {
    border: none;
    background: #007bff;
    color: white;
    cursor: pointer;
    font-weight: 600;
    box-shadow: 0 2px 6px rgba(0,0,0,0.18);
  }
  button:hover { background: #0059c4; }

  .sensor-table td {
    padding: 3px 6px;
  }

  .sensor-table th {
    padding: 4px;
    font-weight: bold;
    background:#e9eef6;
    border-bottom:1px solid #ccc;
  }

  @media (max-width: 480px){
    .card { width: 90%; }
  }

  /* 相機卡片 */
  .cam-big {
    grid-column: 1 / -1;
    width: 100%;
    max-width: calc(320px * 3 + 32px);
    margin-left: auto;
    margin-right: auto;
    padding-left: 12px;
    padding-right: 12px;
    box-sizing: border-box;
  }

  .cam-inner {
    display: flex;
    gap: 16px;
    max-width: calc(320px * 3 + 32px);
    margin: 0 auto;
  }
  .cam-left { flex:3; }
  .cam-right { flex:1; }
  .cam-left img { width:100%; border-radius:10px; }

  /* WiFi */
  .wifi-item {
    padding: 8px 0;
    border-bottom: 1px solid #ddd;
    display: flex;
    align-items: center;
    justify-content: space-between;
  }
  .wifi-left { display:flex; align-items:center; gap:8px; }
  .wifi-btn { padding:4px 10px; margin-left:6px; border-radius:6px; font-size:14px; }
  .wifi-btn-edit { background:#28a745; }
  .wifi-btn-del { background:#dc3545; }
  .wifi-btn-go { background:#007bff; }

  /* popup */
  #popupBg {
    display:none;
    position:fixed; top:0; left:0; width:100%; height:100%;
    background:rgba(0,0,0,0.5);
    backdrop-filter: blur(3px);
  }
  #wifiPopup {
    position:absolute; top:40%; left:50%; transform:translate(-50%,-50%);
    background:white; padding:20px 25px; border-radius:15px;
    width:260px; box-shadow:0 0 18px rgba(0,0,0,0.25);
  }

  </style>
  </head>

  <body>

  <h2>🐍 ESP32 LX-224 控制面板</h2>

  <div class="container">

    <!-- 相機 ------------------------------>
    <div class="card cam-big">
      <div class="cam-inner">
        <div class="cam-left">
          <h3>📷 相機畫面</h3>
          <img src="/cam">
        </div>
        <div class="cam-right">
          <h3>🎛 相機控制</h3>

          <label>解析度：
            <select onchange="sendCam('framesize',this.value)">
              <option value="10">UXGA</option>
              <option value="9">SXGA</option>
              <option value="8" selected>SVGA</option>
              <option value="6">VGA</option>
              <option value="5">CIF</option>
              <option value="3">QVGA</option>
            </select>
          </label>

          <label>畫質：
            <input type="range" min="4" max="63" value="10"
              oninput="sendCam('quality', this.value)">
          </label>

          <label>亮度：
            <input type="range" min="-2" max="2" value="0"
              oninput="sendCam('brightness', this.value)">
          </label>

          <label>對比：
            <input type="range" min="-2" max="2" value="0"
              oninput="sendCam('contrast', this.value)">
          </label>
        </div>
      </div>
    </div>

  </div> <!-- container end -->

  <div class="flex-wrap">

    <!-- Servo 狀態 ------------------------------>
    <div class="card">
      <h3>🤖 Servo 狀態</h3>
      <table class="sensor-table" id="servoTable">
        <tr>
          <th>ID</th>
          <th>Target</th>
          <th>Read</th>
          <th>Error</th>
        </tr>
      </table>
    </div>

    <!-- 模式切換 ------------------------------>
    <div class="card">
      <h3>🧭 模式切換</h3>
      <button onclick="setMode(0)">Sin 模式</button>
      <button onclick="setMode(1)">CPG 模式</button>
      <button onclick="setMode(2)">Offset 模式</button>
      <p>目前模式：<span id="mode">-</span></p>

      <button onclick="toggleFeedback()">切換回授</button>
      <p>回授狀態：<span id="feedback">-</span></p>
    </div>

    <!-- 參數設定 ------------------------------>
    <div class="card">
      <h3>⚙️ 參數設定</h3>

      <label>頻率：
        <input id="freqInput" type="number" step="0.1">
      </label>
      <button onclick="setFrequency()">設定</button>

      <label>振幅：
        <input id="ampInput" type="number" step="1">
      </label>
      <button onclick="setAmplitude()">設定</button>

      <label>λ：
        <input id="lambdaInput" type="number" step="0.05">
      </label>
      <button onclick="setLambda()">設定</button>

      <label>L：
        <input id="Linput" type="number" step="0.05">
      </label>
      <button onclick="setL()">設定</button>

      <label>回授權重：
        <input id="fbGain" type="range" min="0" max="1" step="0.1"
          oninput="fbVal.innerText=this.value">
      </label>
      <span id="fbVal">1.0</span>
      <button onclick="setFeedbackGain()">設定</button>
    </div>

    <!-- 系統狀態 ------------------------------>
    <div class="card">
      <h3>📡 系統狀態</h3>
      <p>頻率：<span id="freq">-</span></p>
      <p>振幅：<span id="amp">-</span></p>
      <p>λ：<span id="lambda">-</span></p>
      <p>L：<span id="L">-</span></p>
      <p>回授權重：<span id="fbGainStatus">-</span></p>
    </div>

    <!-- 系統控制 ------------------------------>
    <div class="card">
      <h3>🕒 控制</h3>
      <p>運作時間：<span id="uptime">-</span></p>
      <button onclick="togglePause()">⏸ 暫停 / ▶️ 繼續</button>
      <button onclick="downloadCSV()">📥 下載 CSV</button>
    </div>

  </div>

  <script>
  function sendCam(v,val){ fetch(`/cam_control?var=${v}&val=${val}`); }
  function setMode(m){ fetch('/setMode?m='+m).then(r=>r.text()).then(t=>mode.innerText=t); }
  function toggleFeedback(){ fetch('/toggleFeedback').then(r=>r.text()).then(t=>feedback.innerText=t); }
  function setFrequency(){ fetch('/setFrequency?f='+freqInput.value); }
  function setAmplitude(){ fetch('/setAmplitude?a='+ampInput.value); }
  function setLambda(){ fetch('/setLambda?lambda='+lambdaInput.value); }
  function setL(){ fetch('/setL?L='+Linput.value); }
  function setFeedbackGain(){ fetch('/setFeedbackGain?g='+fbGain.value); }
  function togglePause(){ fetch('/toggle_pause'); }
  function downloadCSV(){ location.href='/download'; }

  function refreshStatus(){
    fetch('/status').then(r=>r.json()).then(j=>{

      freq.innerText=j.frequency.toFixed(2);
      amp.innerText=j.amplitude.toFixed(1);
      lambda.innerText=j.lambda.toFixed(2);
      L.innerText=j.L.toFixed(2);
      fbGainStatus.innerText=j.fbGain.toFixed(2);

      // ===== Servo Table =====
      const tbl = document.getElementById("servoTable");
      tbl.innerHTML = `
        <tr>
          <th>ID</th>
          <th>Target</th>
          <th>Read</th>
          <th>Error</th>
        </tr>
      `;

      j.servo.forEach(s => {
        const row = document.createElement("tr");
        row.innerHTML = `
          <td>${s.id}</td>
          <td>${s.target}</td>
          <td>${s.read}</td>
          <td>${s.error}</td>
        `;
        tbl.appendChild(row);
      });

      uptime.innerText = `${j.uptime_min}:${String(j.uptime_sec).padStart(2,'0')}`;
    });
  }

  setInterval(refreshStatus,1000);
  </script>

  </body>
  </html>
  )rawliteral";
