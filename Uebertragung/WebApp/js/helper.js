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

var calcOptimalSize = function(width, height, maxWidth) {
	var ratio = 0;
	var new_width = width;
	var new_height = height;
	if (width > maxWidth)
	{
		ratio = width / maxWidth;
		new_width = maxWidth;
		new_height = (height / ratio);
	}
	console.log("optimal size: " + new_width + " x " + new_height);
	return [new_width, new_height];
};