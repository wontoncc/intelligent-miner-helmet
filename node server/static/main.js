var SCALE = 40;
var numberNow;
var hats = [];
hats.get = function getHat(n){
  for(var i = 0; i < hats.length; i++){
    if(hats[i].number == n){
      return hats[i];
    }
  }
}

function Hat(n, e, t){
  this.x = 0;
  this.y = 0;
  this.number = n;
  this.element = e;
  this.terminal = t;
}
Hat.prototype.setX = function setX(n){
  this.element.style.left = Number(this.element.style.left.slice(0,-2)) + floorAndCeil(n * SCALE) +'px';
}
Hat.prototype.setY = function setY(n){
  this.element.style.top = Number(this.element.style.top.slice(0,-2)) + floorAndCeil(n * SCALE) +'px';
}

function locationInit (o, n) {
  hats.get(n).setX(o.x);
  hats.get(n).setY(o.y)
}

function checkHat(n){
  for(var i = 0; i < hats.length; i++){
    if(hats[i].number == n){
      return true;
    }
  }
  return false;
}

function floorAndCeil(n){
  return n > 0 ? Math.floor(n) : Math.ceil(n);
}

function GPSDataUpdate (o, n) {
  if(o.info && n == numberNow){
    var toDisplay = ["latitude", "longitude"];
    toDisplay.forEach(function (e) {
      var capitalized = e.charAt(0).toUpperCase() + e.slice(1);
      GUI['content'+capitalized].innerHTML = o[e];
    });
  }
  console.log(o);
  // GUI.LocationPoint.style.top = Number(GUI.LocationPoint.style.top.slice(0, -2)) + floorAndCeil(o.dY * 20) +'px';
  // GUI.LocationPoint.style.left = Number(GUI.LocationPoint.style.left.slice(0, -2)) + floorAndCeil(o.dX * 20) +'px';
  hats.get(n).setX(o.dX);
  hats.get(n).setY(o.dY);
}

function GasDataUpdate (o, n){
  if(n == numberNow){
    var randomValue = Math.random() / 10;
    GUI.contentCO.innerHTML = (randomValue).toFixed(2) + '% 量程';

    if(o.ch > 0.1 && o.ch4 < 0.5){
      o.ch4 = o.ch4 / Math.abs(5 - o.ch4 * 10);
    }

    GUI.contentCH4.innerHTML = (o.ch4 * 100).toFixed(2) + '% 量程';
    if(o.co > 0.7){
      // GUI.labelCO.style['background-color'] = 'red';
    } else{
      GUI.labelCO.style['background-color'] = 'black';
    }
    if(o.ch4 > 0.9){
      GUI.labelCH4.style['background-color'] = 'red';
    } else{
      GUI.labelCH4.style['background-color'] = 'black';
    }
  }
  if(o.ch4 > 0.9 || o.co > 0.7){
    if(hats.get(n).terminal.className.search('in-danger') == -1){
      hats.get(n).terminal.className += " in-danger";
    }
  } else{
    hats.get(n).terminal.className = hats.get(n).terminal.className.replace(" in-danger", "");
  }
}

var GUI = {
  comButton: '#com-button',
  comToOpen: 'select',
  presentContent: "#present-content",
  LocationMap: "#map",
  contentLongitude: "#content-longitude",
  contentLatitude: "#content-latitude",
  contentCO: '#content-co',
  contentCH4: '#content-ch4',
  labelCO: '#label-co',
  labelCH4: '#label-ch4',
  aboutButton: '#about-btn',
  terminalList: '#terminals',
  map: '#map'
}
for(var i in GUI){
  GUI[i] = document.querySelector(GUI[i]);
}
GUI.comButton.innerHTML = '→';
// GUI.LocationPoint.style.top= (GUI.LocationMap.clientHeight-64) / 2 + 'px';
// GUI.LocationPoint.style.left= (GUI.LocationMap.clientWidth-64) / 2 + 'px';

function addHat(n){
  var tm = document.createElement('div');
  tm.className = 'terminal animated bounceIn';
  setTimeout(function(){
    tm.className = tm.className.replace(" animated bounceIn", "");
  }, 1000);
  tm.innerHTML = n;
  function onclickWrapper(i){
    return function(){
      numberNow = i;
      for(var j = 0; j < GUI.terminalList.childNodes.length; j++){
        GUI.terminalList.childNodes.item(j).className = 'terminal';
        GUI.terminalList.childNodes.item(j).style.background = 'rgba(200,200,200,0.35)';
      }
      GUI.contentCO.className = GUI.contentCO.className.replace(" animated fadeIn", "");
      GUI.contentCO.className = GUI.contentCO.className.replace(" animated flipInX", "");
      setTimeout(function(){
        GUI.contentCO.className += " animated flipInX";
        GUI.contentCH4.className += " animated flipInX";
      }, 100);
      GUI.contentCO.style.opacity = 1;
      GUI.contentCH4.className = GUI.contentCH4.className.replace(" animated fadeIn", "");
      GUI.contentCH4.className = GUI.contentCH4.className.replace(" animated flipInX", "");
      GUI.contentCH4.style.opacity = 1;
      tm.style.background = 'rgba(0,0,0,0.7)';
    };
  }
  tm.onclick = onclickWrapper(n);
  GUI.terminalList.appendChild(tm);

  var point = document.createElement('div');
  point.className = 'point animated bounceIn';
  var number = document.createElement('div');
  number.className = 'number'
  number.innerHTML = n;
  point.appendChild(number);

  point.style.top = (GUI.LocationMap.clientHeight-64) / 2 + 'px';
  point.style.left= (GUI.LocationMap.clientWidth-64) / 2 + 'px';

  GUI.map.appendChild(point);

  hats.push(new Hat(n, point, tm));

  if(hats.length === 1){
    tm.style.background = 'rgba(0,0,0,0.7)';
  }
}

function updateGLLData(data){
  var result = '';
  for(var i in data){
    result += i + ': ' + data[i] + '\r\n';
  }
  GUI.GLLContent.innerHTML = result;
}

var sockjs = new SockJS('/serial');
sockjs._send = sockjs.send;
sockjs.send = function (s) {
  sockjs._send(JSON.stringify(s));
}

sockjs.onmessage = function (e) {
  var obj = JSON.parse(e.data);
  console.log(obj);

  if(obj.number && obj.number != "" && !checkHat(obj.number)){
    addHat(obj.number);
    if(numberNow == undefined){
      numberNow = obj.number;
    }
  }

  if(obj.port){
    var comItem = document.createElement('option');
    comItem.innerHTML = obj.port.comName;
    comItem.setAttribute('value', obj.port.comName);
    console.log(GUI);
    GUI.comToOpen.appendChild(comItem);
  }
  if(obj.loc){
    locationInit(obj.loc, obj.number);
  }
  if(obj.gps){
    GPSDataUpdate(obj.gps, obj.number);
  }
  if(obj.gas){
    GasDataUpdate(obj.gas, obj.number);
  }
  if(obj.open){
    GUI.comToOpen.value = obj.open;
    GUI.comButton.innerHTML = '×';
  }
  if(obj.close == 'closed'){
    GUI.comButton.innerHTML = '→';
  }
  if(obj.lost){
    hats.get(obj.lost).element.className.replace(' animated bounceIn', '');
    hats.get(obj.lost).terminal.className.replace(' animated bounceIn', '');
    hats.get(obj.lost).element.className += ' animated bounceOut';
    hats.get(obj.lost).terminal.className += ' animated bounceOut';
    setTimeout(function(){
      GUI.map.removeChild(hats.get(obj.lost).element);
      GUI.terminalList.removeChild(hats.get(obj.lost).terminal);
      for(var i = 0; i < hats.length; i++){
        if(hats[i].number == obj.lost){
          hats.splice(i, 1);
        }
      }
    }, 2000);
  }
};
GUI.comButton.onclick = function () {
  console.log(GUI.comButton.innerHTML);
  switch(GUI.comButton.innerHTML){
    case '→': 
      sockjs.send({open: GUI.comToOpen.value});
      GUI.comButton.innerHTML = '×';
      break;
    case '×':
    console.log({close: GUI.comToOpen.value});
      sockjs.send({close: GUI.comToOpen.value});
      GUI.comButton.innerHTML = '→';
      break;
  }
}

GUI.aboutButton.onclick = function () {
  var button = document.querySelector('#about-btn');
 if(isDesktop()){ 
    var card = document.querySelector('#card');
    if(card.className === 'flipped'){
      card.className = '';
      button.innerHTML = '关于';
    } else{
      card.className = 'flipped';
      button.innerHTML = '系统';
    }
  } else{
    window.scrollTo(0,document.body.scrollHeight);
  }
}