'use strict';
var $ = function (id) { return document.getElementById(id); };
var cfg = null;

function toast(msg) {
  var t = $('toast');
  t.textContent = msg;
  t.classList.add('show');
  clearTimeout(toast._t);
  toast._t = setTimeout(function () { t.classList.remove('show'); }, 2200);
}

// ----- Tabs -----
var TAB_KEY = 'deskbuddy-tab-v3';
function showTab(id) {
  document.querySelectorAll('.tab').forEach(function (t) { t.classList.toggle('active', t.dataset.tab === id); });
  document.querySelectorAll('.pane').forEach(function (p) { p.classList.toggle('active', p.dataset.pane === id); });
  try { localStorage.setItem(TAB_KEY, id); } catch (e) {}
}
document.querySelectorAll('.tab').forEach(function (t) {
  t.addEventListener('click', function () { showTab(t.dataset.tab); });
});

// ----- Swatches -----
function buildSwatches(containerId, list, selected, valueId, onPick) {
  var box = $(containerId);
  box.innerHTML = '';
  list.forEach(function (item) {
    var el = document.createElement('label');
    el.className = 'swatch' + (item.k === selected ? ' active' : '');
    el.style.background = item.css;
    el.dataset.key = item.k;
    el.addEventListener('click', function () {
      box.querySelectorAll('.swatch').forEach(function (s) { s.classList.remove('active'); });
      el.classList.add('active');
      if (valueId) $(valueId).textContent = item.k;
      onPick(item.k);
    });
    box.appendChild(el);
  });
  if (valueId) $(valueId).textContent = selected;
}

var sel = { accent: '', text: '', bg: '', homeSlots: [] };

function render() {
  var o = cfg.opt;
  $('ip').textContent = cfg.ip;

  // simple fields
  $('notes').value = cfg.notes || '';
  $('nickname').value = cfg.nickname || '';
  $('sleepMin').value = cfg.sleepMin;
  $('units').value = cfg.units;
  $('region').value = cfg.region;
  $('locname').value = cfg.locname || '';
  $('lat').value = cfg.lat;
  $('lng').value = cfg.lng;
  $('flashMode').checked = !!cfg.flashMode;
  $('prchEn').checked = !!cfg.prchEn;
  $('prchArea').value = cfg.prchArea || '';
  $('prchLabel').value = cfg.prchLabel || '';
  $('hueEn').checked = !!cfg.hueEn;
  $('hueBridge').value = cfg.hueBridge || '';
  $('bambuEn').checked = !!cfg.bambuEn;
  $('bambuIP').value = cfg.bambuIP || '';
  $('bambuCode').value = cfg.bambuCode || '';
  $('bambuSerial').value = cfg.bambuSerial || '';
  $('hpEn').checked = !!cfg.hpEn;
  $('hpIP').value = cfg.hpIP || '';
  renderDevStatus(cfg.devstat);

  // timezone select
  var tz = $('tz'); tz.innerHTML = '';
  var grp = {};
  o.tz.forEach(function (z) {
    if (!grp[z.g]) { grp[z.g] = document.createElement('optgroup'); grp[z.g].label = z.g; tz.appendChild(grp[z.g]); }
    var op = document.createElement('option'); op.value = z.k; op.textContent = z.l;
    if (z.k === cfg.tz) op.selected = true;
    grp[z.g].appendChild(op);
  });

  // swatches
  sel.accent = cfg.accent; sel.text = cfg.text; sel.bg = cfg.bg;
  buildSwatches('accent-swatches', o.accent, cfg.accent, 'accent-value', function (k) { sel.accent = k; });
  buildSwatches('text-swatches', o.accent, cfg.text, 'text-value', function (k) { sel.text = k; });
  buildSwatches('bg-swatches', o.bg, cfg.bg, 'bg-value', function (k) { sel.bg = k; });

  // widgets
  var wbox = $('widgets'); wbox.innerHTML = '';
  sel.homeSlots = cfg.homeSlots.slice();
  cfg.homeSlots.forEach(function (cur, idx) {
    var wrap = document.createElement('div'); wrap.className = 'field';
    var lab = document.createElement('label'); lab.className = 'lbl'; lab.textContent = cfg.slotLabels[idx];
    var s = document.createElement('select');
    o.widgets.forEach(function (w) {
      var op = document.createElement('option'); op.value = w.k; op.textContent = w.l;
      if (w.k === cur) op.selected = true; s.appendChild(op);
    });
    s.addEventListener('change', function () { sel.homeSlots[idx] = s.value; });
    wrap.appendChild(lab); wrap.appendChild(s); wbox.appendChild(wrap);
  });

  // timers
  var tbox = $('timers'); tbox.innerHTML = '';
  cfg.timers.forEach(function (v, i) {
    var d = document.createElement('div'); d.className = 'tslot';
    d.innerHTML = '<div class="h">Slot ' + (i + 1) + '</div>';
    var inp = document.createElement('input');
    inp.type = 'number'; inp.min = '1'; inp.max = '180'; inp.value = v; inp.dataset.timer = i;
    d.appendChild(inp); tbox.appendChild(d);
  });

  applyHueStatus(cfg.hueStatus, cfg.huePaired);
}

function applyHueStatus(msg, paired) {
  var box = $('hueStatus');
  if (msg) {
    var ok = /OK|success|Found/.test(msg);
    box.className = 'status ' + (ok ? 'ok' : 'bad');
    box.textContent = msg; box.style.display = '';
  } else {
    box.style.display = 'none';
  }
  var pill = $('huePill');
  pill.className = 'pill ' + (paired ? 'ok' : 'bad');
  pill.textContent = paired ? 'Paired' : 'Not paired';
  $('hueLive').style.display = paired ? '' : 'none';
}

// ----- Live Hue control -----
var HUE_PRESETS = [
  { css: '#ffd9a0', cmd: { ct: 454 } },          // warm white
  { css: '#cfe6ff', cmd: { ct: 250 } },          // cool white
  { css: '#ff3030', cmd: { hue: 0, sat: 254 } },
  { css: '#ff9000', cmd: { hue: 5000, sat: 254 } },
  { css: '#ffe000', cmd: { hue: 11000, sat: 254 } },
  { css: '#30d030', cmd: { hue: 25500, sat: 254 } },
  { css: '#30e0e0', cmd: { hue: 39000, sat: 254 } },
  { css: '#3060ff', cmd: { hue: 46920, sat: 254 } },
  { css: '#a040ff', cmd: { hue: 50000, sat: 254 } },
  { css: '#ff40c0', cmd: { hue: 56100, sat: 254 } }
];

function hueCtl(cmd) {
  fetch('/api/huecontrol', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(cmd) });
}

function hexToHueSat(hex) {
  var r = parseInt(hex.substr(1, 2), 16) / 255,
      g = parseInt(hex.substr(3, 2), 16) / 255,
      b = parseInt(hex.substr(5, 2), 16) / 255;
  var max = Math.max(r, g, b), min = Math.min(r, g, b), d = max - min, h = 0;
  if (d) {
    if (max === r) h = ((g - b) / d) % 6;
    else if (max === g) h = (b - r) / d + 2;
    else h = (r - g) / d + 4;
    h *= 60; if (h < 0) h += 360;
  }
  var s = max === 0 ? 0 : d / max;
  return { hue: Math.round(h / 360 * 65535), sat: Math.round(s * 254) };
}

function buildHueControls() {
  var box = $('huePresets');
  box.innerHTML = '';
  HUE_PRESETS.forEach(function (p) {
    var el = document.createElement('div');
    el.className = 'swatch'; el.style.background = p.css;
    el.addEventListener('click', function () { hueCtl(p.cmd); toast('Sent'); });
    box.appendChild(el);
  });
  $('hueOnBtn').addEventListener('click', function () { hueCtl({ on: true }); });
  $('hueOffBtn').addEventListener('click', function () { hueCtl({ on: false }); });
  $('hueBri').addEventListener('change', function () { hueCtl({ on: true, bri: +this.value }); });
  $('hueColor').addEventListener('change', function () {
    var c = hexToHueSat(this.value); hueCtl({ on: true, hue: c.hue, sat: c.sat });
  });
}

function renderDevStatus(s) {
  if (!s) return;
  var b = $('bambuStat');
  if ($('bambuEn').checked) {
    b.style.display = '';
    b.className = 'status ' + (s.bambuOnline ? 'ok' : 'bad');
    b.textContent = s.bambuOnline
      ? (s.bambuState + ' · ' + s.bambuPct + '% · ' + s.bambuRemain + ' min left · nozzle ' + s.bambuNozzle + '° bed ' + s.bambuBed + '°')
      : 'Offline / connecting…';
  } else { b.style.display = 'none'; }
  var h = $('hpStat');
  if ($('hpEn').checked) {
    h.style.display = '';
    h.className = 'status ' + (s.hpOnline ? 'ok' : 'bad');
    h.textContent = s.hpOnline ? (s.hpState + (s.hpInk >= 0 ? (' · ink ~' + s.hpInk + '%') : '')) : 'Offline';
  } else { h.style.display = 'none'; }
}

function collect() {
  var timers = [];
  document.querySelectorAll('[data-timer]').forEach(function (inp) { timers[+inp.dataset.timer] = +inp.value; });
  return {
    notes: $('notes').value, nickname: $('nickname').value,
    accent: sel.accent, text: sel.text, bg: sel.bg,
    sleepMin: +$('sleepMin').value, units: $('units').value, region: $('region').value,
    tz: $('tz').value, flashMode: $('flashMode').checked,
    locname: $('locname').value, lat: parseFloat($('lat').value), lng: parseFloat($('lng').value),
    timers: timers, homeSlots: sel.homeSlots,
    prchEn: $('prchEn').checked, prchArea: $('prchArea').value, prchLabel: $('prchLabel').value,
    hueEn: $('hueEn').checked, hueBridge: $('hueBridge').value,
    bambuEn: $('bambuEn').checked, bambuIP: $('bambuIP').value, bambuCode: $('bambuCode').value,
    bambuSerial: $('bambuSerial').value, hpEn: $('hpEn').checked, hpIP: $('hpIP').value
  };
}

function load() {
  return fetch('/api/config').then(function (r) { return r.json(); }).then(function (d) { cfg = d; render(); });
}

function save() {
  var btn = $('saveBtn'); btn.disabled = true;
  fetch('/api/save', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(collect()) })
    .then(function (r) { return r.json(); })
    .then(function () { toast('Saved'); return load(); })
    .catch(function () { toast('Save failed'); })
    .finally(function () { btn.disabled = false; });
}

function hueAction(path, btn) {
  var b = $(btn); b.disabled = true;
  fetch(path, { method: 'POST' })
    .then(function (r) { return r.json(); })
    .then(function (d) { applyHueStatus(d.status, d.paired); if (d.bridge) $('hueBridge').value = d.bridge; })
    .catch(function () { applyHueStatus('Request failed', false); })
    .finally(function () { b.disabled = false; });
}

$('saveBtn').addEventListener('click', save);
$('testAlertBtn').addEventListener('click', function () {
  fetch('/api/testalert', { method: 'POST' }).then(function () { toast('Alert triggered'); });
});
$('hueFindBtn').addEventListener('click', function () { hueAction('/api/huefind', 'hueFindBtn'); });
$('huePairBtn').addEventListener('click', function () { hueAction('/api/huepair', 'huePairBtn'); });
$('hueTestBtn').addEventListener('click', function () { hueAction('/api/huetest', 'hueTestBtn'); });

buildHueControls();

// restore last tab
try { var st = localStorage.getItem(TAB_KEY); if (st) showTab(st); } catch (e) {}

load();
