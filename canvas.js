document.addEventListener('DOMContentLoaded', function draw () {  
  var canvas = document.querySelector('#map-overlay');
  var canvasContext = canvas.getContext('2d');

  (function drawLabel(x, y){
    canvasContext.font = 'normal 18pt "黑体"';
    canvasContext.strokeStyle = 'white';
    canvasContext.lineWidth = 2;
    canvasContext.strokeText('位置地图', x,y);
    canvasContext.fillStyle = 'black';
    canvasContext.fillText('位置地图', x,y);
  })(380, 475);


  (function drawRuler(x, y){

    function drawStrokedLine(x1, y1, x2, y2){
      canvasContext.lineWidth = 4;
      canvasContext.strokeStyle = 'white';
      canvasContext.moveTo(x1,y1);
      canvasContext.lineTo(x2,y2);
      canvasContext.stroke()
      canvasContext.lineWidth = 2;
      canvasContext.strokeStyle = 'black';
      canvasContext.stroke();
    }
    drawStrokedLine(x, y-10, x, y+10);
    drawStrokedLine(x+100, y-10, x+100, y+10);
    drawStrokedLine(x, y, x+100, y);

    for(var i = 10; i < 100; i += 10){
      canvasContext.lineWidth = 0.3;
      canvasContext.moveTo(x+i,y-5);
      canvasContext.lineTo(x+i,y);
      canvasContext.stroke();
    }
    canvasContext.font = 'normal 10pt "Microsoft Yahei"';
    canvasContext.lineWidth = 2;
    canvasContext.strokeStyle = 'white';
    canvasContext.strokeText('10m', x+35, y+15);
    canvasContext.fillText('10m', x+35, y+15);
  })(200, 465);
});