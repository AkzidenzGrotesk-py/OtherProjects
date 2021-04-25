// Slide information
var cSlide = 1;
var maxSlide = 4;
var minSlide = 1;

// Move slide forwards and backwards
function incrementSlide() {
  cSlide++;
  if (cSlide >= (maxSlide + 1)) {
    cSlide = minSlide;
  }
  location.href = "slides.html#" + cSlide;
}
function decrementSlide() {
  cSlide--;
  if (cSlide <= (minSlide - 1)) {
    cSlide = maxSlide;
  }
  location.href = "slides.html#" + cSlide;
}

// Add on click to "slide_content"
function addOnClick() {
  var x = document.getElementsByClassName("slide_content");
  for (var i = 0; i < x.length; i++) {
    x[i].addEventListener('click', incrementSlide, false);
  }
}

// jQuery
$(document).ready(function(){
  $("body").keydown(function(event){ 
	if (event.which == 39 || event.which == 32) {
		incrementSlide();
	}
	if (event.which == 37) {
		decrementSlide();
	}
  });
});