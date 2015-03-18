var ua2text = function(ua) {
	var s = '';
	// only use values RGB from RGBA per Pixel, whitespace as delimiter
	for (var i = 0; i < ua.length; i+=4) {
		s += ua[i].toString() + ' ' + ua[i + 1].toString() + ' ' + ua[i + 2].toString() + ' ';
	}
	return s;
};

var resizeElement = function(obj, maxWidth) {
	var ratio = 0;
	var new_width = obj.width;
	var new_height = obj.height;
	if (obj.width > maxWidth)
	{
		ratio = obj.width / maxWidth;
		new_width = maxWidth;
		new_height = (obj.height / ratio);
	}
	console.log("new size: " + new_width + " x " + new_height);
	return [new_width, new_height];
};

/**
 * Calculates a new width and height if the width is higher than maxWidth.
 * @param upScale scales the values up if True
 */
var calcOptimalSize = function(width, height, maxWidth, upscale=false) {
	var ratio = 0;
	var new_width = width;
	var new_height = height;
	if (width > maxWidth)
	{
		ratio = width / maxWidth;
		new_width = maxWidth;
		new_height = (height / ratio);
	}
	else {
		if(upscale) {
			ratio = maxWidth / width;
			new_width = maxWidth;
			new_height = (height * ratio);
		}
	}
	console.log("optimal size: " + new_width + " x " + new_height);
	return [new_width, new_height];
};

var getXMLforPicture = function(width, height, image_payload) {
	var time = new Date();
	var timeStr = time.toLocaleTimeString();
	var xmlString = '<?xml version="1.0" encoding="UTF-8"?>';
	xmlString += '<package>';
	xmlString += '<type>picture</type>';
	xmlString += '<width>' + width +'</width>';
	xmlString += '<height>' + height +'</height>';
	xmlString += '<timestamp>' + timeStr + '</timestamp>';
	xmlString += '<data>' + image_payload + '</data>';
	xmlString += '</package>';
	return xmlString;
};

var sendPicture = function(width, height, image_payload, facade) {
	var result = facade.sendData(getXMLforPicture(width, height, image_payload));
	if (result == 0) {
		console.log("Photo sent! Dimensions " +  width + " x " + height);
		toastr.success('Your photo was sent!');
	}
	else {
		toastr.error(result);
	}
};

var drawPictureToCanvas = function(picture, picture_width, picture_height, dest_canvas, dest_width, upscale=true) {
	var optimal_size = calcOptimalSize(picture_width, picture_height, dest_width, upscale)
	dest_canvas.width = optimal_size[0];
	dest_canvas.height = optimal_size[1];
	dest_canvas.getContext('2d').drawImage(picture, 0, 0, optimal_size[0], optimal_size[1]);
};
		
var drawPictureFromFileToCanvas = function(image_file, dest_canvas, dest_width) {
	var img = document.getElementById("preview_img");
	var reader = new FileReader();
	reader.onload = function(e) {
		img.src = e.target.result;
	}
	reader.readAsDataURL(image_file);
	img.onload = function() {
		drawPictureToCanvas(img, img.width, img.height, dest_canvas, dest_width, false);
	}
};

var get720pPictureBase64Payload = function(image, work_canvas) {
	// dont upscale, it can be done in the rendering client if necessary. Keep bandwidth low!
	drawPictureToCanvas(image, image.width, image.height, work_canvas, 1280, false);
	var payload = work_canvas.toDataURL("image/jpeg");
	payload = payload.replace(/^data:image\/jpeg;base64,/ig, "");
	return payload;
};

var get720pFilePictureBase64Payload = function(image_file, work_canvas, callback) {
	var image = document.getElementById("data_img");
	var reader = new FileReader();
	reader.onload = function(e) {image.src = e.target.result;};
	reader.readAsDataURL(image_file);
	image.onload = function() {
		var payloadFP = get720pPictureBase64Payload(image, work_canvas);
		callback(payloadFP);
	}
};
