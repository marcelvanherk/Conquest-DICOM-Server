function readJSON(url) {
  var x = new XMLHttpRequest();
  var settings;
  x.onreadystatechange = function () {
	if (x.readyState===4 && x.status===200) {
	  settings = JSON.parse(x.responseText);
	}
  }
  x.open("GET", url + '?_=' + new Date().getTime(), false);
  x.send();
  return settings;
}

function gup( name, url ) { // read cgi parameter
	if (!url) url = location.href;
	name = name.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");
	var regexS = "[\\?&]"+name+"=([^&#]*)";
	var regex = new RegExp( regexS );
	var results = regex.exec( url );
	return results == null ? null : results[1];
}	

// function draw1Slice(modid, settings, idi)
// {
	// readCT(modid,settings[idi].SOPInstanceUID)
	// //settings[0].PixelSpacing
	// drawGS(modid, settings[idi].SOPInstanceUID, settings[idi].PixelSpacing)
	// var testing=123
// }

// function readCT(modid, uid) {
	// var fname = 'data/'.concat(modid, '/CT/', uid, '.jpg');
	// document.getElementById("DebugInfo").innerHTML = fname

	// var ctx = document.getElementById('tv');
	// if (ctx.getContext) {
		// ctx = ctx.getContext('2d');
		// var img1 = new Image();
		// // drawing the image
		// img1.onload = function() {
			// ctx.drawImage(img1,0,0);
		// };
		// img1.src=fname;
	// }
// }

// function drawGS(modid, uid, ctres){

	
	// if ((window.contours === undefined) || (window.contours[0].Slice != uid)){
		// var fname = 'data/'.concat(modid, '/GS/', uid, '.json');
		// window.contours = readJSON(fname);
		// window.contours[0].Slice = uid; 
	// }
	
	// if( window.contours[0].Structure === undefined ) {
		// document.getElementById("DebugInfo").innerHTML = "No contours here!"
	// }
	// else{
		// document.getElementById("DebugInfo").innerHTML = "" + window.contours.length + " contours to be drawn"
		// var ctx = document.getElementById('tv');
		// if (ctx.getContext) {
			// ctx = ctx.getContext('2d');
			// ctx.strokeStyle = "#df4b26";
			// ctx.lineJoin = "round";
			// ctx.lineWidth = 2;
			// for( var stri = 0; stri < window.contours.length; stri++ ) {
				// if( window.contours[stri].Structure === undefined ){ 
					// continue;
				// }
				// for( var cnti=0; cnti<window.contours[stri].Contours.length; cnti++){
					// ctx.beginPath();
					// for( var linei=0;linei<window.contours[stri].Contours[cnti].length; linei++){
						// var x = window.contours[stri].Contours[cnti][linei][0];
						// var y = window.contours[stri].Contours[cnti][linei][1];
						// if( linei == 0) {
							// ctx.moveTo(x,y);
						// }
						// else{
							// ctx.lineTo(x,y);
						// }
					// }
					// ctx.closePath();
					// ctx.stroke();
				// }
			// }
		// }
	// }
// }






