var MAX_STEP_DECTECT_TOLERANCE = 15;
var INTEGRAL_FIX = 1.45;
var G = 9.8;
var HASH_CHECK_CAP = 5;
var STEP_FIX = 1;

var mpu6050 = require('./mpu6050');
var nmea = require('./NMEAExtract'); 

var AccDataStorage = require('./package').AccDataStorage;
var MPU6050DataQueue = require('./package').MPU6050DataQueue;

function degreeToRadian (d) {
  return (Math.PI * d / 180);
}

function adcExtract(u8arr){
  var channel0 = (u8arr[0] << 4) + (u8arr[1] >> 4);
  var channel1 = (u8arr[2] << 4) + (u8arr[3] >> 4);
  return {
    ch0: channel0,
    ch1: channel1
  };
}

function bufferCompare(a, b){
  if(a.length != b.length){
    return false;
  } else{
    for(var i = 0; i < a.length; i++){
      if(a[i] != b[i]){
        return false;
      }
    }
    return true;
  }
}

function Hat(n){
  this.serialBuffer = new Buffer(0);
  this.hashCheck = [];
  this.lostWait = undefined;

  function addWrapper(that){
    return function hashCollectionAdd(o){
      if(that.hashCheck.length + 1 > HASH_CHECK_CAP){
        that.hashCheck.splice(0,1);
      }
      that.hashCheck.push(require('crypto').createHash('md5').update(o).digest());
    }
  }
  this.hashCheck.add = addWrapper(this);
  this.hashCheck.check = (function checkWrapper(that){
    return function hashCollectionCheck(o){
      for(var i = 0; i < that.hashCheck.length; i++){
        var hash = require('crypto').createHash('md5').update(o).digest();
        if(bufferCompare(hash, that.hashCheck[i])){
          // console.log(that.hashCheck[i]);
          // console.log(hash);
          return true;
        }
      }
      return false;
    }
  })(this);

  this.number = n;

  this.accDataStorage = new AccDataStorage();
  this.dataQueue = new MPU6050DataQueue();
  this.locOffset = { x:0, y:0 };
  this.stepCount = 0;

  this.waitGPSData = undefined;
  this.lastGPSData = undefined;
  this.accumulateMeters = 0;
  this.GPSEnabled = false;
}

function AccDataPackageProcess(that, conn){
  return function accProcessWrapper(p, i){
    if(that.accDataStorage.stepDectect){
      if(p[0].az <= 1 * G){
        console.log('! step - ', that.accDataStorage.integralDegree);
        console.log('       - ', (MAX_STEP_DECTECT_TOLERANCE - that.accDataStorage.stepDectectTolerance) + ' packages after last step.');
        var toSend = { gps: {} };
        toSend.gps.dY = 0.5 * Math.cos(mpu6050.degreeToRadian(that.accDataStorage.integralDegree));
        toSend.gps.dX = 0.5 * Math.sin(mpu6050.degreeToRadian(that.accDataStorage.integralDegree));
        that.locOffset.x += toSend.gps.dX;
        that.locOffset.y += toSend.gps.dY;
        conn.write(toSend);
        that.accDataStorage.stepDectect = false;
        that.accDataStorage.stepDectectTolerance = MAX_STEP_DECTECT_TOLERANCE;
        that.stepCount += 1;
      } else{
        that.accDataStorage.stepDectectTolerance -= 1;
        if(that.accDataStorage.stepDectectTolerance == 0){
          that.accDataStorage.stepDectect = false;
          that.accDataStorage.stepDectectTolerance = MAX_STEP_DECTECT_TOLERANCE;
        }
      }
    }
    if(p[0].az >= 1.03 * G){
      // console.log('beep');
      if(!that.accDataStorage.stepDectect){
        that.accDataStorage.stepDectectTolerance = MAX_STEP_DECTECT_TOLERANCE;
      }
      that.accDataStorage.stepDectect = true;

    }

    if(Math.abs(p[1].wz) <= 0){
      p[1].wz = 0;
      that.accDataStorage.SpinningStopTolerance -= 1;
    }
    if(that.accDataStorage.SpinningStopTolerance == 0){
      that.accDataStorage.SpinningStopTolerance = 5;
      that.accDataStorage.isSpinniing = false;
    }

    // filter
    var lastAndCurrentSameSign = 
      (that.dataQueue.last() > 0 && p[1].wz > 0) ||
      (that.dataQueue.last() < 0 && p[1].wz < 0);
    var isGreatDrop = Math.abs(that.dataQueue.last()) - Math.abs(p[1].wz) >= 20;
    if(that.dataQueue.last() && lastAndCurrentSameSign && isGreatDrop){
      p[1].wz = (that.dataQueue.last() + p[1].wz)/2;
    }

    // integral
    if(!that.accDataStorage.isSpinniing && Math.abs(p[1].wz) > 4){
      that.accDataStorage.isSpinniing = true;
    }
    if(that.accDataStorage.isSpinniing){
    // if(true){
      p[1].wz = p[1].wz || that.dataQueue.last() || 0;
      if(p[1].wz != 0){
        that.accDataStorage.SpinningStopTolerance = 5;
      }

      that.accDataStorage.integralDegree += 0.05 * INTEGRAL_FIX * p[1].wz;
      if(that.accDataStorage.integralDegree > 360){
        that.accDataStorage.integralDegree -= 360;
      }
      if(that.accDataStorage.integralDegree < -360){
        that.accDataStorage.integralDegree += 360;
      }
      // console.log(that.accDataStorage.integralDegree);
    }
  }
}

Hat.prototype.processHatPackage = function processHatPackage(package, conn){
  switch(package.name){
    case 'MPUDAT':
      if(this.GPSEnabled){
        break;
      }
      var mpu6050Extracted = mpu6050.MPU6050_ExtractFromBuffer(new Uint8Array(package.buffer));
      var packages = [];
      mpu6050Extracted.forEach(function (e, i) {
        // if(e && e.ax && mpu6050Extracted[i+1] && mpu6050Extracted[i+2]){
        if(e && e.ax && mpu6050Extracted[i+1]){
          // if(e.valid && mpu6050Extracted[i+1].valid && mpu6050Extracted[i+2].valid){
          if(e.valid && mpu6050Extracted[i+1].valid){
            packages.push([e, mpu6050Extracted[i+1], mpu6050Extracted[i+2]]);
          } else{
            // console.log((new Date()).toString(),'invalid');
            // console.log(mpu6050Extracted[i])
            // console.log(mpu6050Extracted[i+1]);
            // console.log(mpu6050Extracted[i+2]);
          }
        }
      });
      packages.forEach(AccDataPackageProcess(this, conn));
      break;
    case 'ADCDAT':
      var adcExtracted = adcExtract(new Uint8Array(package.buffer));
      conn.write({
        gas: {
          co: adcExtracted.ch0 / 4095,
          ch4: adcExtracted.ch1 / 4095
        }
      });
      break;
    case 'GPSDAT':
      var GPSExtracted = NMEAExtract.NMEAExtractRawString(package.buffer.toString());
      this.GPSEnabled = true;
      clearTimeout(this.waitGPSData);
      this.waitGPSData = setTimeout(function(){
        this.GPSEnabled = false;
      },1100);
      for(var i = 0; i < GPSExtracted.length; i++){
        if(GPSExtracted[i].info == '$GPGLL'){
          if(this.lastGPSData){
            var toSend = {gps: GPSExtracted[i]};

            var d = nmea.distanceOf(GPSExtracted[i].latitude, GPSExtracted[i].longitude, this.lastGPSData.latitude, this.lastGPSData.longitude);
            var ky = GPSExtracted[i].latitude >= this.lastGPSData.latitude ? 1 : -1;

            var kx = GPSExtracted[i].longitude >= this.lastGPSData.longitude ? 1 : -1;
            var deltaX = kx * nmea.distanceOf(GPSExtracted[i].latitude, this.lastGPSData.longitude, this.lastGPSData.latitude, this.lastGPSData.longitude);
            var deltaY = ky * nmea.distanceOf(this.lastGPSData.latitude, GPSExtracted[i].longitude, this.lastGPSData.latitude, this.lastGPSData.longitude);
            console.log(d);
            console.log('dX='+deltaX, 'dY='+deltaY);
            toSend.gps.dX = deltaX;
            toSend.gps.dY = deltaY;
            conn.write(toSend);
            if(d>0.15){
              this.accumulateMeters += d;
            }
            console.log('a_meters: '+this.accumulateMeters);

          }
          this.lastGPSData = GPSExtracted[i];
        }
      }
      break;
  }
}

exports.Hat = Hat;