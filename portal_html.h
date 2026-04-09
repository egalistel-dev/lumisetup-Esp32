#pragma once
const char PORTAL_HTML[] PROGMEM = R"LUMI(
<!DOCTYPE html><html lang="fr">
<head>
<meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>LumiSetup — WiFi</title>
<style>
*{box-sizing:border-box;margin:0;padding:0;}
body{background:#0f0f13;color:#e8e8f0;font-family:'Segoe UI',system-ui,sans-serif;display:flex;align-items:center;justify-content:center;min-height:100vh;padding:20px;}
.card{background:#1a1a24;border:1px solid #2a2a3a;border-radius:16px;padding:32px;max-width:380px;width:100%;text-align:center;}
h1{color:#f5a623;font-size:1.5rem;margin-bottom:6px;}
p{color:#7a7a9a;font-size:.85rem;margin-bottom:24px;line-height:1.6;}
label{display:block;text-align:left;font-size:.82rem;color:#7a7a9a;margin-bottom:4px;}
input[type=text],input[type=password]{width:100%;background:#0f0f13;border:1px solid #2a2a3a;border-radius:8px;color:#e8e8f0;padding:12px;font-size:.95rem;margin-bottom:16px;outline:none;-webkit-appearance:none;}
input:focus{border-color:#f5a623;}
input[type=submit]{width:100%;padding:14px;border-radius:10px;border:none;background:#f5a623;color:#111;font-size:1rem;font-weight:700;cursor:pointer;-webkit-appearance:none;}
.lang-bar{display:flex;justify-content:flex-end;margin-bottom:16px;gap:6px;}
.lang-btn{padding:4px 12px;border-radius:20px;border:1px solid #2a2a3a;background:transparent;color:#7a7a9a;font-size:.8rem;cursor:pointer;}
.lang-btn.active{background:#f5a623;color:#111;border-color:#f5a623;font-weight:700;}
footer{margin-top:28px;font-size:.75rem;color:#4a4a6a;}
footer a{color:#f5a623;text-decoration:none;}
</style>
</head>
<body>
<div class="card">
  <div class="lang-bar">
    <button class="lang-btn" id="btnFR" onclick="setLang(true);return false;">FR</button>
    <button class="lang-btn" id="btnEN" onclick="setLang(false);return false;">EN</button>
  </div>
  <h1>💡 LumiSetup</h1>
  <p id="pDesc">Configurez la connexion WiFi<br>pour votre lampe connectée.</p>
  <form method="POST" action="/wifi/save">
    <label id="lblSsid">Nom du réseau WiFi (SSID)</label>
    <input type="text" name="ssid" id="ssid" placeholder="Mon réseau WiFi" required>
    <label id="lblPass">Mot de passe</label>
    <input type="password" name="pass" id="pass" placeholder="Mot de passe">
    <input type="submit" id="submitBtn" value="Se connecter">
  </form>
  <footer>Egalistel &nbsp;·&nbsp; <a href="https://egamaker.be" target="_blank">egamaker.be</a></footer>
</div>
<script>
const T={
  fr:{desc:'Configurez la connexion WiFi<br>pour votre lampe connectée.',ssid:'Nom du réseau WiFi (SSID)',ssidPh:'Mon réseau WiFi',pass:'Mot de passe',passPh:'Mot de passe',btn:'Se connecter'},
  en:{desc:'Configure the WiFi connection<br>for your smart lamp.',ssid:'WiFi Network Name (SSID)',ssidPh:'My WiFi Network',pass:'Password',passPh:'Password',btn:'Connect'}
};
let isFR=true;
function applyLang(){
  const l=isFR?T.fr:T.en;
  document.getElementById('pDesc').innerHTML=l.desc;
  document.getElementById('lblSsid').textContent=l.ssid;
  document.getElementById('ssid').placeholder=l.ssidPh;
  document.getElementById('lblPass').textContent=l.pass;
  document.getElementById('pass').placeholder=l.passPh;
  document.getElementById('submitBtn').value=l.btn;
  document.getElementById('btnFR').classList.toggle('active',isFR);
  document.getElementById('btnEN').classList.toggle('active',!isFR);
}
function setLang(fr){isFR=fr;applyLang();try{fetch('/toggle/lang',{method:'POST'});}catch(e){}}
applyLang();
</script>
</body></html>
)LUMI";
