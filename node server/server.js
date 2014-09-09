var SerialPort = require('serialport').SerialPort;
var sockjs = require('sockjs');
var node_static = require('node-static');

var Hat = require('./hat').Hat;
var hats = [];

var Package = require('./package').Package;
var findPackageHead = require('./package').findPackageHead;

var sockjsServer = sockjs.createServer();
var avaliableCOM = [];
var connectionPool = [];

sockjsServer.on('connection', function (conn) {
  // var integralDegree = 0;
  var GPSEnabled = false;

  connectionPool.push(conn);

  conn._write = conn.write;
  conn.write = function (o){
    for(var i = 0; i < connectionPool.length; i++){
      connectionPool[i]._write(JSON.stringify(o));
    }
  }

  for(var i in hats){
    conn._write(JSON.stringify({ loc: hats[i].locOffset, number: hats[i].number }));
  }

  // list avaliable serialport
  require('serialport').list(function (err, ports) {
    ports.forEach(function (p) {
      conn.write({port: p});
    });
  });

  setTimeout(function(){
    if(avaliableCOM.length > 0){
      conn.write({ open: avaliableCOM[0].path });
    }
  }, 300);


  console.log('* sockjs client connected.');
  var serialport;

  // console.log(avaliableCOM);
  conn.on('data', function (msg) {
    var obj = JSON.parse(msg);
    if(obj.send){
      serialport.write(obj.send);
    }
    if(obj.open){
      for(var i = 0; i < avaliableCOM.length; i++){
        if(avaliableCOM[i].path == obj.open){
          serialport = avaliableCOM[i];
          console.log('* open existing serialport.');
          break;
        }
      }
      if(i === avaliableCOM.length){
        serialport = new SerialPort(obj.open, {
          baudrate: 115200
        }, false);
      }

      function sscomOpen(){
        avaliableCOM.push(serialport);
        conn.write({ open: serialport.path });

        console.log('* serial open');
      }

      serialport.on('open', sscomOpen);
      // serialport.write(Buffer([0xFF, 0xAA, 0x52]));
      serialport.on('data', function (data) {
        do{
          var comingHatNumber = String.fromCharCode(data[0]);
          if(isNaN(comingHatNumber)){
            break;
          }
          if(hats[comingHatNumber] == undefined){
            hats[comingHatNumber] = new Hat(comingHatNumber);
          }
          for(var i in hats){
            var matchPackage = findPackageHead(hats[i].serialBuffer);
            if(matchPackage !== -1){
              var package = new Package(hats[i].serialBuffer);

              conn.writeWithoutNumber = conn.write;
              conn.write = function anotherConnWriteWrapper(o){
                o.number = hats[i].number;
                conn.writeWithoutNumber(o);
              }

              hats[i].processHatPackage(package, conn);

              conn.write = conn.writeWithoutNumber;
              if(comingHatNumber == i){
                hats[i].serialBuffer = new Buffer(hats[i].serialBuffer.slice(matchPackage-1));
              }
            }
          }
          clearTimeout(hats[comingHatNumber].waitLost);
          if(!hats[comingHatNumber].hashCheck.check(data)){
            hats[comingHatNumber].hashCheck.add(data);
            hats[comingHatNumber].serialBuffer = Buffer.concat([hats[comingHatNumber].serialBuffer, data]);
          } else{
            console.log('* package data collided.');
          }
          // hats[comingHatNumber].serialBuffer = Buffer.concat([hats[comingHatNumber].serialBuffer, data]);
          hats[comingHatNumber].waitLost = setTimeout(function(){
            try{
              (function clearLostWrapper(n) {
                clearTimeout(hats[n].waitLost);
              })(comingHatNumber);
            } catch(e){
              console.log('* clear lost hat unexpectedly.');
            }
            console.log('* step count:',hats[comingHatNumber].stepCount);
            hats.splice(comingHatNumber,1);
            console.log('* lost:', '#'+comingHatNumber);
            for(var i = 0; i < connectionPool.length; i++){
              connectionPool[i].write({ lost: comingHatNumber });
            }
          }, 5000);
        } while(0);
      });
      if(serialport.readable === undefined){
        serialport.open();
      }
    }
    if(obj.close){
      if(serialport){
        try{
          serialport.close();
        } catch(e){
          console.log('* serialport close unexpectedly.');
        }
        console.log('* serialport closed.');

        conn.write({ close: 'closed' });
        avaliableCOM = [];
        serialport = undefined;
      }
    }
  });
  conn.on('close', function () {
    for(var i = 0; i < connectionPool.length; i++){
      if(connectionPool[i] == conn){
        connectionPool.splice(i,1);
      }
    }
    console.log('* sockjs client disconnects.');
  })
});


var static_directory = new node_static.Server('static');

var server = require('http').createServer();
server.addListener('request', function (req, res) {
  static_directory.serve(req, res);
});
server.addListener('upgrade', function (req, res) {
  res.end();
});

sockjsServer.installHandlers(server, { prefix: '/serial' });

server.listen(8099, '0.0.0.0');