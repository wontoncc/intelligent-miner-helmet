function NMEAExtractRawString (s){
  var re = /\$.*\r\n/g;
  var instructions = s.match(re);
  var list = [];
  if(instructions){
    instructions.forEach(function (e) {
      list.push(NMEAExtractSingleInstruction(e));
    });
  }
  return list;
}

function NMEAExtractSingleInstruction (s) {
  var sliceStart = s.search('\\$');
  s = s.slice(sliceStart);
  var sliceEnd = s.search('\\r\\n');
  s = s.slice(0, sliceEnd);
  var parts = s.split(',');
  var table = ['info','latitude', 'ns', 'longitude', 'ew', 'time', 'status', 'checksum'];
  var NMEAGLLObject = {};
  parts.forEach(function(e, i){
    NMEAGLLObject[table[i]] = e;
  });
  NMEAGLLObject.latitude /= 100;
  NMEAGLLObject.longitude /= 100;
  return NMEAGLLObject;
}

function distanceOf(latitude1, longitude1, latitude2, longitude2) {
  var toRadians = function (degree) {
    return degree * Math.PI / 180;
  };
  // R is the radius of the earth in meters.
  var R = 6371000;
  var deltaLatitude = toRadians(latitude2-latitude1);
  var deltaLongitude = toRadians(longitude2-longitude1);
  latitude1 =toRadians(latitude1);
  latitude2 =toRadians(latitude2);
  var a = Math.sin(deltaLatitude/2) *
  Math.sin(deltaLatitude/2) +
  Math.cos(latitude1) *
  Math.cos(latitude2) *
  Math.sin(deltaLongitude/2) *
  Math.sin(deltaLongitude/2);
  var c = 2 * Math.atan2(Math.sqrt(a),
  Math.sqrt(1-a));
  var d = R * c;
  return d;
}

exports.NMEAExtractRawString = NMEAExtractRawString;
exports.NMEAExtractSingleInstruction = NMEAExtractSingleInstruction;
exports.distanceOf = distanceOf;