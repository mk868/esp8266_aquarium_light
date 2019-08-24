const $ = {};

$.get = function(url, callback){
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function(){
		if(this.readyState == 4){
			var body = this.responseText;
			try{
				body = JSON.parse(body);
			}catch(ex){}
			callback(body, this.status);
		}
	};
	xhttp.open("GET", url, true);
	xhttp.send();
}

$.post = function(url, data, callback){
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function(){
		if(this.readyState == 4){
			var body = this.responseText;
			try{
				body = JSON.parse(body);
			}catch(ex){}
			callback(body, this.status);
		}
	};
	xhttp.open("POST",  url, true);
	xhttp.setRequestHeader("Content-Type", "application/json");
	xhttp.send(JSON.stringify(data));
}

function colorToHex(r, g, b) {
	r = r.toString(16);
	g = g.toString(16);
	b = b.toString(16);
	
	if(r.length === 1) {
		r = 0 + r;
	}
	if(g.length === 1) {
		g = 0 + g;
	}
	if(b.length === 1) {
		b = 0 + b;
	}
	return r + g + b;
}
	
function hexToColor(color) {
	var m = color.match(/([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})/);
	
	return {
		r: parseInt(m[1], 16),
		g: parseInt(m[2], 16),
		b: parseInt(m[3], 16)
	};
}
