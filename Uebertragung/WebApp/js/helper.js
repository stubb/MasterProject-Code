var ua2text = function(ua) {
	var s = '';
	// only use values RGB from RGBA per Pixel, whitespace as delimiter
	for (var i = 0; i < ua.length; i+=4) {
		s += ua[i].toString() + ' ' + ua[i + 1].toString() + ' ' + ua[i + 2].toString() + ' ';
	}
	return s;
};