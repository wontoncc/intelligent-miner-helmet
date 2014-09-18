var virtualHats = [1, 2];

function virtualMessageHandler (o) {
  messageHandler({ data: JSON.stringify(o) });
}

function runDemo () {
  addPort({ comName: 'COM1' });
  virtualMessageHandler({ open: 'COM1' });

  virtualMessageHandler({
    number: '1',
    loc: {
      x: -2.5,
      y: -2.5
    }
  });

  virtualMessageHandler({
    number: '2',
    loc: {
      x: 2.5,
      y: 2.5
    }
  });

  // initialize the regular gas-data generator
  for(var i = 0; i < virtualHats.length; i++){
    virtualHats[i] = {};
    (function (i, hat){
      hat.ADCDataGenerator = setInterval(function(){
        var gasData = { ch4: Math.random(), co: Math.random() };
        GasDataUpdate(gasData, String(i+1));
      }, 500);
    })(i, virtualHats[i]);
  }

  virtualHats[0].walkCount = 0;
  virtualHats[0].walkGenerator = setInterval(function() {
    var x = 0;
    var y = 0;
    switch(Math.floor(virtualHats[0].walkCount / 20)){
      case 0: x = 0.25; y = 0; break;
      case 1: x = 0; y = 0.25; break;
      case 2: x = -0.25; y = 0; break;
      case 3: x = 0; y = -0.25; break;
    }
    virtualMessageHandler({
      gps:{
        dX: x,
        dY: y
      },
      number: '1'
    });
    virtualHats[0].walkCount += 1;
    if(virtualHats[0].walkCount > 80){
      virtualHats[0].walkCount = 0;
    }
  }, 500);
}

function stopDemo () {
  for(var i = 0; i < virtualHats.length; i++){
    clearInterval(virtualHats[i].ADCDataGenerator);
    clearInterval(virtualHats[i].walkGenerator);
    virtualMessageHandler({ lost: '1' });
    virtualMessageHandler({ lost: '2' });
  }
}

var runDemoButton = document.querySelector('#run-demo');
runDemoButton.onclick = function () {
  if(runDemoButton.className.search('on') === -1){
    runDemoButton.className += ' on';
    runDemo();
  } else{
    runDemoButton.className = 'toolbar-btn';
    stopDemo();
  }
}