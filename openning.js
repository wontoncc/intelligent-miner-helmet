function isDesktop(){
  return navigator.userAgent.search('Android') === -1;
}

document.addEventListener('DOMContentLoaded', function openningAnimation () {
  var GUI = {
    comButton: '#com-button',
    comToOpen: '#com-toopen',
    presentContent: "#present-content",
    LocationMap: "#map",
    LocationPoint: "#point",
    contentLongitude: "#content-longitude",
    contentLatitude: "#content-latitude"
  }
  for(var i in GUI){
    GUI[i] = document.querySelector(GUI[i]);
  }

  document.querySelector('#content').className += ' animated zoomIn';
  setTimeout(function(){
    if(isDesktop()){
      document.querySelector('#head').className = 'animated fadeInUp';
    } else{
      document.querySelector('#outter-head').className = 'animated fadeInDown';
      document.querySelector('#outter-head').style['background'] = 'rgba(0,0,0,0.7)';
    }

    var titles = document.querySelectorAll('.inner-title');
    for(var i = 0; i < titles.length; i++){
      titles[i].className += ' animated fadeInLeft';
    }
  }, 800);
  setTimeout(function(){
    var labels = document.querySelectorAll('.label');
    for(var i = 0; i < labels.length; i++){
      labels[i].className += ' animated fadeInLeft';
    }
    var contents = document.querySelectorAll('.item-content');
    console.log(contents);
    for(var i = 0; i < contents.length; i++){
      contents.item(i).className += ' animated fadeIn';
    }
  }, 1200);
});