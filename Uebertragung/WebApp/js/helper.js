var ua2text = function(ua) {
	var s = '';
	for (var i = 0; i < ua.length; i++) {
		s += ua[i].toString();
	}
	return s;
};