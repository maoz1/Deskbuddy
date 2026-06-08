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
    hueEn: $('hueEn').checked, hueBridge: $('hueBridge').value
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

// restore last tab
try { var st = localStorage.getItem(TAB_KEY); if (st) showTab(st); } catch (e) {}

load();
