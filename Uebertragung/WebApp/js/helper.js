var ua2text = function(ua) {
	var s = '';
	// only use values RGB from RGBA per Pixel
	// e.g. 255 1 1 255 -> 255 001 001
	for (var i = 0; i < ua.length; i+=4) {
		s += ("000" + ua[i].toString()).slice(-3);
		s += ("000" + ua[i + 1].toString()).slice(-3);
		s += ("000" + ua[i + 2].toString()).slice(-3);
		console.log(s);
	}
	return s;
};