
// TODO:
//   get colors per structure automatically. DONE
//   get ids of observers to be drawn.  DONE, changed from m selected to all or single.

// json files with general info.
var basefolder;
var fCTinfo;
var fTemplate;
var fGSlist;
var fObslist;

// initalize these whenever patient is changed.
var colorsPerStr;
var structNames;

// bridge to UI
var currentSliceNr;
var selectedGS;
var selectedObs;
var selectedStructures;

// loaded info for current sliceNr
var currentCT;
var BGloaded;
var currentGS;
var currentObs;


function redraw(){
	if( BGloaded ){
		drawCT();
		drawObs();
		drawGS();
	}
}


function drawCT() {
	if( BGloaded ){
		var ctx = document.getElementById('canvasCT').getContext('2d');
		ctx.clearRect(0, 0, 512,512);
		ctx.drawImage(currentCT,0,0);
	}
}

function drawObs() {
	if( currentObs != undefined){
		var ctx = document.getElementById('canvasObs').getContext('2d');
		var ncnts = drawStructures(ctx,currentObs,false);
		document.getElementById("ObserversNames").innerHTML = "Observers: " + ncnts + " contours drawn."
	}
}

function drawGS() {
	if( currentGS != undefined) {
		var ctx = document.getElementById('canvasGS').getContext('2d');
		var ncnts = drawStructures(ctx,currentGS,true); 
		document.getElementById("GoldenStandardNames").innerHTML = "Golden Standards: " + ncnts + " contours drawn."
	}
}



// initialize all important data now! :)
function changePatientData(patient){
	basefolder="data/".concat(patient, "/")

	fCTinfo = readJSON(basefolder.concat("/CT/sliceinfo.json" ));
	fTemplate = readJSON(basefolder.concat('/GS/Template.json') );
	fGSlist = readJSON(basefolder.concat('/GS/list.json') );
	fObslist = readJSON(basefolder.concat( '/Observers/list.json') );
	
	// fill anmes and colors of structures
	structNames = getStructureNames();
	colorsPerStr = [];
	function getAndConvertColor(strci){
		colorsPerStr[strci.Structure] = 'rgb(' + strci.Color[0] +','+strci.Color[1]+','+strci.Color[2]+')';
	}
	fTemplate.forEach( getAndConvertColor );
	
	//initialize oher random variables
	if( currentSliceNr === undefined )
		currentSliceNr = Math.round(fCTinfo.length/2);
	else
		currentSliceNr = Math.min(currentSliceNr, fCTinfo.length);  // update it to the new dataset.
	selectedGS = -1;
	if ( fGSlist.length>0 && fGSlist[0].Observer != undefined )
		selectedGS = fGSlist[0].Observer;
	selectedObs = "*all";
	selectedStructures = "*all";
	BGloaded = false;

	if ( currentCT === undefined )
		currentCT = new Image();
}

// get info read files
function getNrOfSlices(){
	return fCTinfo.length;
}

function getStructureNames(){
	var names = [];
	for( i=0; i<fTemplate.length; i++)
		if ( fTemplate[i].Structure != undefined )
			names[i]=fTemplate[i].Structure;
	return names;
}

function getGSNames(){
	var names = [];
	for( i=0; i<fGSlist.length; i++)
		if ( fGSlist[i].Observer != undefined )
			names[i]=fGSlist[i].Observer;
	return names;
}

function getObsNames(){
	var names = [];
	for( i=0; i<fObslist.length; i++)
		if ( fObslist[i].Observer != undefined )
			names[i]=fObslist[i].Observer;
	return names;
}

function shouldThisStructureBeDrawn( strci, isGS ){
	var drawIt = true;
	
	// is there data?
	if( strci.Structure === undefined )
		drawIt = false;
	
	// is the observer being drawn?
	if( drawIt && (isGS)  && (strci.Observer != selectedGS) )
		drawIt = false;
	
	if( drawIt && (!isGS) && (selectedObs != "*all") && (strci.Observer != selectedObs) )
		drawIt = false;
	
	// is this structure (as in its names is selected) being drawn?
	if( drawIt && (selectedStructures != "*all") && (strci.Structure != selectedStructures) )
		drawIt = false;
	
	// is this structure defined in the template?
	if( drawIt && (structNames.indexOf(strci.Structure) == -1 ))
		drawIt = false;

	// add section stuff here?
	return drawIt;
}

function setPencilForThisStructure(ctx, structure, isGS){
	if( isGS ){
		ctx.lineWidth = 3;
		ctx.strokeStyle = colorsPerStr[structure.Structure];
		ctx.setLineDash([3, 2]);
	}
	else{
		ctx.lineWidth = 1;
		ctx.strokeStyle = colorsPerStr[structure.Structure]; //'rgb(128,128,128)'; // alternative: the invert of the structure color!
		ctx.setLineDash([]);
	}
	
}

function drawStructures( ctx, structures, isGS ){
	var numcnts = 0;
	var currentCTres = fCTinfo[currentSliceNr].PixelSpacing;
	var currentCTImagePositionPatient = fCTinfo[currentSliceNr].ImagePositionPatient;
	ctx.clearRect(0, 0, 512,512);
	ctx.lineJoin = "round";
	for( var stri = 0; stri < structures.length; stri++ ) {
		if( shouldThisStructureBeDrawn(structures[stri], isGS) ){
			setPencilForThisStructure(ctx, structures[stri], isGS);
			for( var cnti=0; cnti<structures[stri].Contours.length; cnti++){
				ctx.beginPath();
				for( var linei=0;linei<structures[stri].Contours[cnti].length; linei++){
					var x = (structures[stri].Contours[cnti][linei][0]  - currentCTImagePositionPatient[0])/currentCTres[0];
					var y = (structures[stri].Contours[cnti][linei][1]  - currentCTImagePositionPatient[1])/currentCTres[1];
					if( linei == 0) {
						ctx.moveTo(x,y);
					}
					else{
						ctx.lineTo(x,y);
					}
				}
				ctx.closePath();
				ctx.stroke();
				numcnts += 1;
			}
		}
	}
	return numcnts;
}

// links to UI
function changeSlide(newi){
	if( (currentSliceNr != newi) && (newi >= 0) && (newi < fCTinfo.length) ){
		currentSliceNr = newi;
		// load image and contour files
		var uid = fCTinfo[newi].SOPInstanceUID;
		// load CT image
		BGloaded = false;
		currentCT.onload = function(){ BGloaded=true; redraw(); };
		currentCT.src=basefolder.concat('/CT/', uid, '.jpg');
		// load GS
		currentGS = readJSON(basefolder.concat('/GS/', uid, '.json'));
		// load observers
		currentObs = readJSON(basefolder.concat('/Observers/', uid, '.json'));
		redraw();
	}
}

function changeSelectedStructures( strcts ){
	selectedStructures = strcts;
	redraw();
}

function changeSelectedGS( GSname ){
	selectedGS = GSname;
	redraw();
}

function changeSelectedObs( Obsname ){
	selectedObs = Obsname;
	redraw();
}

