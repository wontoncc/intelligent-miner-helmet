var PACKAGE_HEAD = '>>>>>>>>>>>>';

function bufferSearch(buf, toSearch){
  for(var i = 0; i + toString.length < buf.length; i++){
    if(buf[i] === toSearch[0]){
      for(var j = 0; j < toSearch.length && i+j < buf.length; j++){
        if(buf[i+j] !== toSearch[j]){
          break;
        }
      }
      if(j == toSearch.length){
        return i;
      }
    }
  }
  return -1;
}

function hashString(s){
  return require('crypto').createHash('md5').update(s).digest('hex');
}

function findPackageHead(buf){
  var first = bufferSearch(buf, Buffer(PACKAGE_HEAD));
  if(first != -1){
    var tailedBuffer = buf.slice(first + 12);
    var second = bufferSearch(tailedBuffer, Buffer(PACKAGE_HEAD));
    if(second != -1){
      return first+12+second;
    }
    return -1;
  }
  return -1;
}

function cleanUpIdentifier(buf){
  var newBuffer = new Buffer(0);
  for(var i = 0; i < buf.length; i++){
    if(i % 32 === 0 && (buf[i] >= '0'.charCodeAt(0) && buf[i] <= '9'.charCodeAt(0))){
      continue;
    }
    newBuffer = Buffer.concat([newBuffer, buf.slice(i, i+1)]);
  }
  return newBuffer;
}

function Package(buf){
  this.number = String.fromCharCode(buf[0]);
  var startingPoint = bufferSearch(buf, Buffer(PACKAGE_HEAD));
  var toFindEnd = buf.slice(startingPoint + 12);
  var endingPoint = bufferSearch(toFindEnd, Buffer(PACKAGE_HEAD)) - 1;
  buf = buf.slice(startingPoint, endingPoint);
  this.name = buf.slice(12, 12+6).toString();
  var rawBuffer = buf.slice(12+6+13);
  this.buffer = cleanUpIdentifier(rawBuffer);
  rawBuffer = null;

  // console.log('#'+this.number, this.name);
  // console.log(this.buffer);
}


function MPU6050DataQueue(){
  this.buffer = [];
  this.length = 10;
}
MPU6050DataQueue.prototype.push = function(o){
  if(this.buffer.length == this.length){
    this.buffer = this.buffer.slice(1);
  }
  this.buffer.push(o);
}
MPU6050DataQueue.prototype.last = function(){
  return this.buffer[this.buffer.length - 1];
}

function AccDataStorage(){
  this.stepDetect = false;
  this.stepDectectTolerance = 10;
  this.SpinningStopTolerance = 5;
  this.isSpinniing = false;
  this.integralDegree = 0;
}

exports.hashString = hashString;
exports.AccDataStorage = AccDataStorage;
exports.MPU6050DataQueue = MPU6050DataQueue;
exports.findPackageHead = findPackageHead;
exports.Package = Package;