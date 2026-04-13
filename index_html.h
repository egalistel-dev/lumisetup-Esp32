#pragma once
const char INDEX_HTML[] PROGMEM = R"LUMI(
<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>LumiSetup</title>
<link rel="manifest" href="/manifest.json">
<link rel="apple-touch-icon" href="/icon.png">
<style>
:root{--bg:#0f0f13;--card:#1a1a24;--border:#2a2a3a;--accent:#f5a623;--accent2:#e05c5c;--green:#4caf7d;--red:#e05c5c;--text:#e8e8f0;--muted:#7a7a9a;--radius:14px;}
*{box-sizing:border-box;margin:0;padding:0;}
body{background:var(--bg);color:var(--text);font-family:'Segoe UI',system-ui,sans-serif;min-height:100vh;padding:16px;}
.page-grid{display:grid;grid-template-columns:1fr;gap:16px;max-width:960px;margin:0 auto;}
@media(min-width:700px){.page-grid{grid-template-columns:1fr 1fr;}.full-width{grid-column:1/-1;}}
header{text-align:center;padding:20px 0 8px;}
header h1{font-size:1.6rem;color:var(--accent);letter-spacing:.06em;}
header p{color:var(--muted);font-size:.85rem;margin-top:4px;}
.lang-bar{display:flex;justify-content:center;gap:8px;margin-top:10px;}
.lang-btn{padding:4px 16px;border-radius:20px;border:1px solid var(--border);background:transparent;color:var(--muted);font-size:.82rem;cursor:pointer;transition:all .2s;}
.lang-btn.active{background:var(--accent);color:#111;border-color:var(--accent);font-weight:700;}
.card{background:var(--card);border:1px solid var(--border);border-radius:var(--radius);padding:20px;}
.card-title{font-size:.72rem;text-transform:uppercase;letter-spacing:.1em;color:var(--muted);margin-bottom:16px;}
.status-row{display:flex;align-items:center;gap:10px;margin-bottom:10px;font-size:.95rem;}
.dot{width:10px;height:10px;border-radius:50%;background:var(--muted);flex-shrink:0;}
.dot.on{background:var(--green);box-shadow:0 0 8px var(--green);}
.dot.warn{background:var(--accent);box-shadow:0 0 8px var(--accent);}
.dot.err{background:var(--red);box-shadow:0 0 8px var(--red);}
.toggle-row{display:flex;align-items:center;justify-content:space-between;margin-bottom:14px;gap:12px;}
.toggle-label{font-size:.95rem;}.toggle-sub{font-size:.76rem;color:var(--muted);margin-top:2px;}
.switch{position:relative;width:52px;height:28px;flex-shrink:0;}
.switch input{opacity:0;width:0;height:0;}
.slider{position:absolute;inset:0;background:var(--border);border-radius:28px;cursor:pointer;transition:background .3s;}
.slider::before{content:'';position:absolute;width:20px;height:20px;left:4px;top:4px;background:white;border-radius:50%;transition:transform .3s;}
input:checked+.slider{background:var(--green);}
input:checked+.slider::before{transform:translateX(24px);}
.mode-row{display:flex;gap:10px;margin-bottom:8px;}
.mode-btn{flex:1;padding:14px;border:2px solid var(--border);border-radius:10px;background:transparent;color:var(--muted);font-size:.95rem;font-weight:700;cursor:pointer;transition:all .2s;}
.mode-btn.active{border-color:var(--accent);color:var(--accent);background:rgba(245,166,35,.1);}
.override-btn{width:100%;padding:16px;border-radius:10px;border:2px solid var(--accent2);background:transparent;color:var(--accent2);font-size:.95rem;font-weight:700;cursor:pointer;transition:all .25s;}
.override-btn.active{background:var(--accent2);color:white;box-shadow:0 0 18px rgba(224,92,92,.4);}
.form-row{display:flex;align-items:center;justify-content:space-between;margin-bottom:14px;gap:12px;}
.form-label{font-size:.88rem;flex:1;}.form-label small{display:block;color:var(--muted);font-size:.74rem;}
input[type=range]{-webkit-appearance:none;flex:1.2;height:6px;background:var(--border);border-radius:3px;outline:none;}
input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;width:20px;height:20px;border-radius:50%;background:var(--accent);cursor:pointer;}
input[type=number],input[type=time],select,.txt-input{background:var(--bg);border:1px solid var(--border);border-radius:8px;color:var(--text);padding:8px 10px;font-size:.88rem;width:90px;text-align:center;}
input[type=time]{width:112px;}
select{width:auto;padding:8px 6px;text-align:left;}
.txt-input{width:160px;text-align:left;}
.color-row{display:flex;align-items:center;justify-content:space-between;margin-bottom:14px;}
input[type=color]{width:52px;height:40px;border:none;border-radius:8px;cursor:pointer;padding:2px;background:var(--bg);}
.save-btn{width:100%;padding:14px;border-radius:10px;border:none;background:var(--accent);color:#111;font-size:1rem;font-weight:700;cursor:pointer;transition:opacity .2s;margin-top:4px;}
.save-btn:hover{opacity:.88;}
.danger-btn{width:100%;padding:12px;border-radius:10px;border:1px solid var(--accent2);background:transparent;color:var(--accent2);font-size:.88rem;font-weight:600;cursor:pointer;transition:all .2s;margin-top:10px;}
.success-btn{width:100%;padding:12px;border-radius:10px;border:1px solid var(--green);background:transparent;color:var(--green);font-size:.88rem;font-weight:600;cursor:pointer;transition:all .2s;margin-top:6px;}
hr{border:none;border-top:1px solid var(--border);margin:14px 0;}
.hint{font-size:.76rem;color:var(--muted);margin-top:6px;line-height:1.5;}
.info-box{background:rgba(76,175,125,.1);border:1px solid var(--green);border-radius:8px;padding:10px;margin-bottom:14px;font-size:.82rem;color:var(--green);line-height:1.5;}
.toast{position:fixed;bottom:24px;left:50%;transform:translateX(-50%);background:var(--green);color:white;padding:10px 28px;border-radius:24px;font-size:.9rem;opacity:0;transition:opacity .3s;pointer-events:none;z-index:99;white-space:nowrap;}
.toast.show{opacity:1;}
.modal-bg{display:none;position:fixed;inset:0;background:rgba(0,0,0,.7);z-index:50;align-items:center;justify-content:center;}
.modal-bg.open{display:flex;}
.modal{background:var(--card);border:1px solid var(--border);border-radius:var(--radius);padding:28px;max-width:340px;width:90%;text-align:center;}
.modal h2{font-size:1.1rem;margin-bottom:10px;}.modal p{font-size:.85rem;color:var(--muted);margin-bottom:20px;line-height:1.6;}
.modal-btns{display:flex;gap:10px;justify-content:center;}
.modal-btns button{padding:10px 22px;border-radius:8px;font-size:.9rem;cursor:pointer;border:none;font-weight:600;}
.btn-cancel{background:var(--border);color:var(--text);}.btn-confirm{background:var(--accent2);color:white;}
footer{text-align:center;padding:24px 0 12px;font-size:.78rem;color:var(--muted);grid-column:1/-1;}
footer a{color:var(--accent);text-decoration:none;}
</style>
</head>
<body>

<header class="full-width">
  <h1>💡 LumiSetup</h1>
  <p id="clockDisplay">--:--</p>
  <div class="lang-bar">
    <button class="lang-btn" id="btnFR" onclick="setLang(true)">FR</button>
    <button class="lang-btn" id="btnEN" onclick="setLang(false)">EN</button>
  </div>
</header>

<div class="page-grid">

  <div class="card">
    <div class="card-title" id="tStatus"></div>
    <div class="status-row"><div class="dot" id="dotLeds"></div><span id="statusLeds"></span></div>
    <div class="status-row"><div class="dot" id="dotPir"></div><span id="statusPir"></span></div>
    <div class="status-row"><div class="dot" id="dotRange"></div><span id="statusRange"></span></div>
    <div class="status-row"><div class="dot" id="dotOverride"></div><span id="statusOverride"></span></div>
    <div class="status-row"><div class="dot" id="dotMqtt"></div><span id="statusMqtt"></span></div>
  </div>

  <div class="card">
    <div class="card-title" id="tControls"></div>
    <div class="toggle-row">
      <div><div class="toggle-label" id="tSysLabel"></div><div class="toggle-sub" id="tSysSub"></div></div>
      <label class="switch"><input type="checkbox" id="toggleSystem" onchange="postToggle('/toggle/system')"><span class="slider"></span></label>
    </div>
    <hr>
    <div class="card-title" id="tMode" style="margin-top:4px"></div>
    <div class="mode-row">
      <button class="mode-btn" id="btnAuto" onclick="setMode(false)">AUTO</button>
      <button class="mode-btn" id="btnOn" onclick="setMode(true)">ON</button>
    </div>
    <p class="hint" id="tModeHint"></p>
    <hr>
    <div class="card-title" id="tOverrideTitle" style="margin-top:4px"></div>
    <button class="override-btn" id="btnOverride" onclick="postToggle('/toggle/override')"></button>
    <p class="hint" id="tOverrideHint"></p>
  </div>

  <div class="card">
    <div class="card-title" id="tLeds"></div>
    <div class="color-row">
      <span class="form-label" id="tColor"></span>
      <input type="color" id="colorPicker" value="#ffc864">
    </div>
    <div class="form-row">
      <span class="form-label" id="tBrightness"></span>
      <input type="range" id="brightness" min="0" max="100" value="80"
             oninput="document.getElementById('lblBrightness').textContent=this.value+'%'">
    </div>
    <div class="form-row">
      <span class="form-label" id="tNumLeds"></span>
      <input type="number" id="numLeds" min="1" max="300" value="30">
    </div>
    <div class="toggle-row">
      <div><div class="toggle-label" id="tFadeLabel"></div><div class="toggle-sub">Fade-in / fade-out</div></div>
      <label class="switch"><input type="checkbox" id="fadeEffect" checked><span class="slider"></span></label>
    </div>
    <div class="form-row">
      <span class="form-label" id="tDuration"></span>
      <input type="number" id="duration" min="5" max="3600" value="120">
    </div>
    <div class="form-row">
      <span class="form-label" id="tDebounce"></span>
      <input type="number" id="pirDebounce" min="1" max="60" value="3">
    </div>
  </div>

  <div class="card">
    <div class="card-title" id="tScheduleTitle"></div>
    <div class="form-row">
      <span class="form-label" id="tScheduleMode"></span>
      <select id="scheduleMode" onchange="onScheduleModeChange()">
        <option value="0" id="optManual"></option>
        <option value="1" id="optHA"></option>
        <option value="2" id="optSun"></option>
      </select>
    </div>
    <div id="manualFields">
      <div class="form-row"><span class="form-label" id="tStart"></span><input type="time" id="startTime" value="18:00"></div>
      <div class="form-row"><span class="form-label" id="tEnd"></span><input type="time" id="endTime" value="23:00"></div>
    </div>
    <div id="haFields" style="display:none">
      <div class="info-box" id="haInfo"></div>
      <div class="form-row"><span class="form-label" id="tStart2"></span><input type="time" id="startTimeHA" disabled style="opacity:.5"></div>
      <div class="form-row"><span class="form-label" id="tEnd2"></span><input type="time" id="endTimeHA" disabled style="opacity:.5"></div>
    </div>
    <div id="sunFields" style="display:none">
      <div class="form-row">
        <span class="form-label" id="tCity"></span>
        <input type="text" id="city" placeholder="Brussels" class="txt-input">
      </div>
      <button class="success-btn" id="tSunTest" onclick="testSunrise()"></button>
      <p class="hint" id="sunStatus" style="margin-top:8px;"></p>
    </div>
    <hr>
    <div class="form-row">
      <span class="form-label" id="tTz"></span>
      <select id="timezone" style="width:auto;max-width:190px;"></select>
    </div>
  </div>

  <div class="card full-width">
    <div class="card-title" id="tMqttTitle"></div>
    <div class="toggle-row">
      <div><div class="toggle-label" id="tMqttLabel"></div><div class="toggle-sub" id="tMqttSub"></div></div>
      <label class="switch"><input type="checkbox" id="mqttEnabled" onchange="onMqttToggle()"><span class="slider"></span></label>
    </div>
    <div id="mqttFields" style="display:none">
      <div class="form-row"><span class="form-label" id="tMqttHost"></span><input type="text" id="mqttHost" placeholder="192.168.1.x" class="txt-input"></div>
      <div class="form-row"><span class="form-label" id="tMqttPort"></span><input type="number" id="mqttPort" value="1883" min="1" max="65535"></div>
      <div class="form-row"><span class="form-label" id="tMqttId"></span><input type="text" id="mqttId" placeholder="lumisetup" class="txt-input"></div>
      <div class="form-row"><span class="form-label" id="tMqttUser"></span><input type="text" id="mqttUser" placeholder="(optionnel)" class="txt-input"></div>
      <div class="form-row"><span class="form-label" id="tMqttPass"></span><input type="password" id="mqttPass" placeholder="(optionnel)" class="txt-input"></div>
      <button class="success-btn" id="tMqttTest" onclick="mqttTest()"></button>
      <p class="hint" id="mqttStatus" style="margin-top:8px;"></p>
    </div>
  </div>

  <div class="card full-width">
    <button class="save-btn" id="tSaveBtn" onclick="saveConfig()"></button>
    <button class="danger-btn" id="tWifiBtn" onclick="openWifiModal()"></button>
  </div>

  <footer class="full-width">
    <span id="tFooter"></span> &nbsp;·&nbsp;
    <a href="https://egamaker.be" target="_blank">egamaker.be</a>
  </footer>
</div>

<div class="modal-bg" id="wifiModal">
  <div class="modal">
    <h2 id="tModalTitle"></h2><p id="tModalBody"></p>
    <div class="modal-btns">
      <button class="btn-cancel" id="tModalCancel" onclick="closeWifiModal()"></button>
      <button class="btn-confirm" id="tModalConfirm" onclick="confirmWifiReset()"></button>
    </div>
  </div>
</div>
<div class="toast" id="toast"></div>

<script>
const T={
  fr:{
    status:'État du système',controls:'Contrôles',
    sysLabel:'Système actif',sysSub:'Désactiver éteint tout',
    mode:'Mode',modeHint:'AUTO : PIR actif dans la plage horaire | ON : toujours allumé',
    overrideTitle:'Override',
    overrideBtn:'🔆 Maintenir allumé',overrideBtnActive:'🔆 Override actif — appuyer pour désactiver',
    overrideHint:"Force l'allumage hors plage. Appuyez à nouveau pour revenir au comportement normal.",
    leds:'LEDs',color:'Couleur',brightness:'Intensité',numLeds:'Nombre de LEDs',fadeLabel:'Effet fondu',
    duration:'Durée allumage',durationSub:'secondes après détection',
    debounce:'Anti-rebond PIR',debounceSub:'délai min entre détections (s)',
    scheduleTitle:'Plage horaire',scheduleMode:'Mode',
    optManual:'⚙️ Manuel',optHA:'🏠 Soleil via Home Assistant',optSun:'🌅 Soleil automatique',
    start:'Début',end:'Fin',start2:'Début (HA)',end2:'Fin (HA)',
    haInfo:'⏱ Plage horaire gérée par Home Assistant via MQTT.<br>Coucher → Lever mis à jour automatiquement.',
    city:'Ville',sunTest:'🌅 Tester lever/coucher',tz:'Fuseau horaire',
    saveBtn:'💾 Enregistrer la configuration',wifiBtn:'📶 Réinitialiser la connexion WiFi',
    footer:'Conçu par Egalistel',
    modalTitle:'Réinitialiser le WiFi ?',
    modalBody:"Les identifiants WiFi seront effacés.<br>L'ESP redémarrera et ouvrira le point d'accès <strong>LumiSetup</strong>.",
    cancel:'Annuler',confirm:'Confirmer',
    ledsOn:'LEDs allumées',ledsOff:'LEDs éteintes',
    pirOn:'Mouvement détecté !',pirOff:'Aucun mouvement',
    rangeOn:'Dans la plage horaire',rangeOff:'Hors plage horaire',
    ovOn:'Override actif',ovOff:'Override inactif',
    mqttOn:'MQTT connecté',mqttOff:'MQTT déconnecté',mqttDis:'MQTT désactivé',
    toastSaved:'✓ Configuration enregistrée',toastReset:'📶 Redémarrage...',
    mqttTitle:'Home Assistant / MQTT',mqttLabel:'MQTT activé',mqttSub:'Connexion au broker Mosquitto',
    mqttHost:'Adresse du broker',mqttPort:'Port',mqttId:'Client ID',
    mqttUser:'Utilisateur',mqttPass:'Mot de passe',mqttTest:'🔌 Tester la connexion',
  },
  en:{
    status:'System Status',controls:'Controls',
    sysLabel:'System active',sysSub:'Disabling turns everything off',
    mode:'Mode',modeHint:'AUTO: PIR active within time range | ON: always on',
    overrideTitle:'Override',
    overrideBtn:'🔆 Force light on',overrideBtnActive:'🔆 Override active — press to disable',
    overrideHint:'Forces the light on regardless of schedule. Press again to return to normal.',
    leds:'LEDs',color:'Color',brightness:'Brightness',numLeds:'Number of LEDs',fadeLabel:'Fade effect',
    duration:'On duration',durationSub:'seconds after detection',
    debounce:'PIR debounce',debounceSub:'min delay between triggers (s)',
    scheduleTitle:'Time Range',scheduleMode:'Mode',
    optManual:'⚙️ Manual',optHA:'🏠 Sunrise via Home Assistant',optSun:'🌅 Automatic sunrise',
    start:'Start',end:'End',start2:'Start (HA)',end2:'End (HA)',
    haInfo:'⏱ Time range managed by Home Assistant via MQTT.<br>Sunset → Sunrise updated automatically.',
    city:'City',sunTest:'🌅 Test sunrise/sunset',tz:'Timezone',
    saveBtn:'💾 Save configuration',wifiBtn:'📶 Reset WiFi connection',
    footer:'Made by Egalistel',
    modalTitle:'Reset WiFi?',
    modalBody:'WiFi credentials will be erased.<br>The ESP will restart and open the <strong>LumiSetup</strong> access point.',
    cancel:'Cancel',confirm:'Confirm',
    ledsOn:'LEDs on',ledsOff:'LEDs off',
    pirOn:'Motion detected!',pirOff:'No motion',
    rangeOn:'Within time range',rangeOff:'Outside time range',
    ovOn:'Override active',ovOff:'Override inactive',
    mqttOn:'MQTT connected',mqttOff:'MQTT disconnected',mqttDis:'MQTT disabled',
    toastSaved:'✓ Configuration saved',toastReset:'📶 Restarting...',
    mqttTitle:'Home Assistant / MQTT',mqttLabel:'MQTT enabled',mqttSub:'Connect to Mosquitto broker',
    mqttHost:'Broker address',mqttPort:'Port',mqttId:'Client ID',
    mqttUser:'Username',mqttPass:'Password',mqttTest:'🔌 Test connection',
  }
};

const TZ_LIST=["Europe/Brussels","Europe/Paris","Europe/London","Europe/Amsterdam","Europe/Berlin","Europe/Madrid","Europe/Rome","Europe/Zurich","Europe/Lisbon","Europe/Helsinki","Europe/Athens","Europe/Bucharest","Europe/Moscow","America/New_York","America/Chicago","America/Denver","America/Los_Angeles","America/Toronto","America/Vancouver","America/Sao_Paulo","America/Argentina/Buenos_Aires","Asia/Dubai","Asia/Kolkata","Asia/Bangkok","Asia/Singapore","Asia/Shanghai","Asia/Tokyo","Asia/Seoul","Australia/Sydney","Pacific/Auckland","Africa/Johannesburg","Africa/Cairo","UTC"];

let isFR=true, lastStatus={};

function populateTZ(){
  const s=document.getElementById('timezone');
  s.innerHTML='';
  TZ_LIST.forEach(tz=>{const o=document.createElement('option');o.value=tz;o.textContent=tz;s.appendChild(o);});
}

function applyLang(){
  const l=isFR?T.fr:T.en;
  ['btnFR','btnEN'].forEach(id=>document.getElementById(id).classList.toggle('active',id==='btnFR'?isFR:!isFR));
  const m={tStatus:'status',tControls:'controls',tSysLabel:'sysLabel',tSysSub:'sysSub',
    tMode:'mode',tModeHint:'modeHint',tOverrideTitle:'overrideTitle',tOverrideHint:'overrideHint',
    tLeds:'leds',tColor:'color',tNumLeds:'numLeds',tFadeLabel:'fadeLabel',
    tScheduleTitle:'scheduleTitle',tScheduleMode:'scheduleMode',
    optManual:'optManual',optHA:'optHA',optSun:'optSun',
    tStart:'start',tEnd:'end',tStart2:'start2',tEnd2:'end2',
    tCity:'city',tSunTest:'sunTest',tTz:'tz',
    tSaveBtn:'saveBtn',tWifiBtn:'wifiBtn',tFooter:'footer',
    tModalTitle:'modalTitle',tModalCancel:'cancel',tModalConfirm:'confirm',
    tMqttTitle:'mqttTitle',tMqttLabel:'mqttLabel',tMqttSub:'mqttSub',
    tMqttHost:'mqttHost',tMqttPort:'mqttPort',tMqttId:'mqttId',
    tMqttUser:'mqttUser',tMqttPass:'mqttPass',tMqttTest:'mqttTest'};
  Object.entries(m).forEach(([id,key])=>{const e=document.getElementById(id);if(e)e.textContent=l[key];});
  setHTML('haInfo',l.haInfo);setHTML('tModalBody',l.modalBody);
  setHTML('tBrightness',l.brightness+'<small id="lblBrightness">'+document.getElementById('brightness').value+'%</small>');
  setHTML('tDuration',l.duration+'<small>'+l.durationSub+'</small>');
  setHTML('tDebounce',l.debounce+'<small>'+l.debounceSub+'</small>');
  if(lastStatus.ledsOn!==undefined) applyStatus(lastStatus);
  const ov=document.getElementById('btnOverride');
  if(ov) ov.textContent=lastStatus.overrideOn?l.overrideBtnActive:l.overrideBtn;
}

function setText(id,t){const e=document.getElementById(id);if(e)e.textContent=t;}
function setHTML(id,h){const e=document.getElementById(id);if(e)e.innerHTML=h;}
function hexToRgb(h){return{r:parseInt(h.slice(1,3),16),g:parseInt(h.slice(3,5),16),b:parseInt(h.slice(5,7),16)};}
function rgbToHex(r,g,b){return'#'+[r,g,b].map(v=>v.toString(16).padStart(2,'0')).join('');}
function sd(id,cls){document.getElementById(id).className='dot '+cls;}
function showToast(msg,err=false){const t=document.getElementById('toast');t.textContent=msg;t.style.background=err?'var(--accent2)':'var(--green)';t.classList.add('show');setTimeout(()=>t.classList.remove('show'),2800);}

async function setLang(fr){isFR=fr;applyLang();try{await fetch('/toggle/lang',{method:'POST'});}catch(e){}}

function applyStatus(d){
  const l=isFR?T.fr:T.en;
  document.getElementById('toggleSystem').checked=d.systemActive;
  document.getElementById('btnAuto').classList.toggle('active',!d.modeOn);
  document.getElementById('btnOn').classList.toggle('active',d.modeOn);
  const ov=document.getElementById('btnOverride');
  ov.classList.toggle('active',d.overrideOn);
  ov.textContent=d.overrideOn?l.overrideBtnActive:l.overrideBtn;
  sd('dotLeds',    d.ledsOn?'on':'off');    setText('statusLeds',    d.ledsOn?l.ledsOn:l.ledsOff);
  sd('dotPir',     d.pirState?'warn':'off');setText('statusPir',     d.pirState?l.pirOn:l.pirOff);
  sd('dotRange',   d.inRange?'on':'err');   setText('statusRange',   d.inRange?l.rangeOn:l.rangeOff);
  sd('dotOverride',d.overrideOn?'warn':'off');setText('statusOverride',d.overrideOn?l.ovOn:l.ovOff);
  if(!d.mqttEnabled){sd('dotMqtt','off');setText('statusMqtt',l.mqttDis);}
  else if(d.mqttConnected){sd('dotMqtt','on');setText('statusMqtt',l.mqttOn);}
  else{sd('dotMqtt','err');setText('statusMqtt',l.mqttOff);}
}

async function refreshStatus(){
  try{
    const d=await(await fetch('/status')).json();
    document.getElementById('clockDisplay').textContent=d.time||'--:--';
    if(d.langFR!==undefined&&isFR!==d.langFR){isFR=d.langFR;applyLang();}
    lastStatus=d; applyStatus(d);
  }catch(e){}
}

async function postToggle(url){try{await fetch(url,{method:'POST'});await refreshStatus();}catch(e){}}
async function setMode(on){const isOn=document.getElementById('btnOn').classList.contains('active');if(isOn===on)return;await postToggle('/toggle/mode');}

function onScheduleModeChange(){
  const mode=parseInt(document.getElementById('scheduleMode').value);
  document.getElementById('manualFields').style.display=mode===0?'block':'none';
  document.getElementById('haFields').style.display=mode===1?'block':'none';
  document.getElementById('sunFields').style.display=mode===2?'block':'none';
}

function onMqttToggle(){
  document.getElementById('mqttFields').style.display=document.getElementById('mqttEnabled').checked?'block':'none';
}

async function testSunrise(){
  const city=document.getElementById('city').value.trim();
  if(!city)return;
  const st=document.getElementById('sunStatus');
  st.textContent='⏳ Récupération...';st.style.color='var(--muted)';
  try{
    const d=await(await fetch('/sun/test?city='+encodeURIComponent(city))).json();
    if(d.ok){st.textContent='🌅 Coucher: '+d.sunset+' → Lever: '+d.sunrise;st.style.color='var(--green)';}
    else{st.textContent='❌ '+d.msg;st.style.color='var(--accent2)';}
  }catch(e){st.textContent='❌ Erreur réseau';st.style.color='var(--accent2)';}
}

async function mqttTest(){
  const st=document.getElementById('mqttStatus');
  st.textContent='⏳ Vérification...';st.style.color='var(--muted)';
  try{
    const s=await(await fetch('/status')).json();
    if(s.mqttConnected){st.textContent='✅ MQTT connecté au broker';st.style.color='var(--green)';return;}
    await fetch('/mqtt/test',{method:'POST'});
    await new Promise(r=>setTimeout(r,2000));
    const s2=await(await fetch('/status')).json();
    if(s2.mqttConnected){st.textContent='✅ MQTT connecté au broker';st.style.color='var(--green)';}
    else{st.textContent='❌ Connexion échouée — vérifiez IP, port et credentials';st.style.color='var(--accent2)';}
  }catch(e){st.textContent='❌ Erreur réseau';st.style.color='var(--accent2)';}
}

function applyConfigData(d){
  document.getElementById('colorPicker').value=rgbToHex(d.red||255,d.green||200,d.blue||100);
  document.getElementById('brightness').value=d.brightness??80;
  const lbl=document.getElementById('lblBrightness');
  if(lbl) lbl.textContent=(d.brightness??80)+'%';
  document.getElementById('numLeds').value=d.numLeds??30;
  document.getElementById('duration').value=d.duration??120;
  document.getElementById('pirDebounce').value=d.pirDebounce??3;
  document.getElementById('fadeEffect').checked=d.fadeEffect!==false;
  const sh=String(d.startHour??18).padStart(2,'0'),sm=String(d.startMin??0).padStart(2,'0');
  const eh=String(d.endHour??23).padStart(2,'0'),em=String(d.endMin??0).padStart(2,'0');
  const tStart=sh+':'+sm, tEnd=eh+':'+em;
  document.getElementById('startTime').value=tStart;
  document.getElementById('endTime').value=tEnd;
  document.getElementById('startTimeHA').value=tStart;
  document.getElementById('endTimeHA').value=tEnd;
  // Appliquer le mode AVANT onScheduleModeChange pour éviter le flash
  const schedSel=document.getElementById('scheduleMode');
  schedSel.value=d.scheduleMode??0;
  onScheduleModeChange();
  document.getElementById('city').value=d.city||'Brussels';
  const tzSel=document.getElementById('timezone');
  if(tzSel) tzSel.value=d.timezone||'Europe/Brussels';
  document.getElementById('mqttEnabled').checked=d.mqttEnabled===true;
  document.getElementById('mqttHost').value=d.mqttHost||'';
  document.getElementById('mqttPort').value=d.mqttPort||1883;
  document.getElementById('mqttId').value=d.mqttId||'lumisetup';
  document.getElementById('mqttUser').value=d.mqttUser||'';
  document.getElementById('mqttFields').style.display=d.mqttEnabled?'block':'none';
  if(d.langFR!==undefined&&isFR!==d.langFR){isFR=d.langFR;applyLang();}
}

async function loadConfig(){
  try{
    const d=await(await fetch('/config')).json();
    applyConfigData(d);
  }catch(e){console.error(e);}
}

async function saveConfig(){
  const l=isFR?T.fr:T.en;
  const rgb=hexToRgb(document.getElementById('colorPicker').value);
  const mode=parseInt(document.getElementById('scheduleMode').value);
  // En mode manuel on envoie les heures manuelles, sinon on envoie 0 (sera géré par HA ou standalone)
  let sh=0,sm=0,eh=0,em=0;
  if(mode===0){
    [sh,sm]=document.getElementById('startTime').value.split(':').map(Number);
    [eh,em]=document.getElementById('endTime').value.split(':').map(Number);
  }
  const body=new URLSearchParams({
    red:rgb.r,green:rgb.g,blue:rgb.b,
    brightness:document.getElementById('brightness').value,
    numLeds:document.getElementById('numLeds').value,
    duration:document.getElementById('duration').value,
    pirDebounce:document.getElementById('pirDebounce').value,
    fadeEffect:document.getElementById('fadeEffect').checked?'1':'0',
    startHour:sh,startMin:sm,endHour:eh,endMin:em,
    scheduleMode:mode,
    city:document.getElementById('city').value,
    timezone:document.getElementById('timezone').value,
    mqttEnabled:document.getElementById('mqttEnabled').checked?'1':'0',
    mqttHost:document.getElementById('mqttHost').value,
    mqttPort:document.getElementById('mqttPort').value,
    mqttId:document.getElementById('mqttId').value,
    mqttUser:document.getElementById('mqttUser').value,
    mqttPass:document.getElementById('mqttPass').value,
  });
  try{
    const resp=await fetch('/config',{method:'POST',body});
    const d=await resp.json();
    // Recharger la config retournée par l'ESP pour mettre à jour l'interface
    // (important pour le mode soleil standalone qui met à jour startHour/endHour)
    applyConfigData(d);
    showToast(l.toastSaved);
  }catch(e){showToast('❌ Erreur sauvegarde',true);}
}

function openWifiModal(){document.getElementById('wifiModal').classList.add('open');}
function closeWifiModal(){document.getElementById('wifiModal').classList.remove('open');}
async function confirmWifiReset(){
  const l=isFR?T.fr:T.en;closeWifiModal();
  try{await fetch('/wifi/reset',{method:'POST'});showToast(l.toastReset);}catch(e){}
}

populateTZ();applyLang();loadConfig();refreshStatus();setInterval(refreshStatus,3000);
</script>
</body></html>
)LUMI";
