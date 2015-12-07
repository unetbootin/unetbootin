var counter = 0;

var PIXEL_SIZE = 4;

var temp_1 = 0;

function init()
{
    setInterval('render()',50);
}

function dist(a, b, c, d)
{
    return Math.sqrt((a - c) * (a - c) + (b - d) * (b - d));
}


function render()
{
    var canvas = document.getElementById('tutorial');
    canvas.resize(128, 128);
    var ctx = canvas.getContext('2d');
    ctx.save();

    var time = counter * 5;


    for( y = 0; y < 128; y+=PIXEL_SIZE) {
	for( x = 0 ; x < 128; x+=PIXEL_SIZE) {
	    
	    
	    var temp_val = Math.floor(Math.sin(dist(x + time, y, 128.0, 128.0) / 8.0)
				      + Math.sin(dist(x, y, 64.0, 64.0) / 8.0)
				      + Math.sin(dist(x, y + time / 7, 192.0, 64) / 7.0)
				      + Math.sin(dist(x, y, 192.0, 100.0) / 8.0));
	    
	    
	    
	    
	    var temp_col = Math.floor((2 + temp_val) * 50);
	    
	    var rand_red = temp_col * 3;
	    var rand_green = temp_col  ;
	    var rand_blue = 128 - temp_col;

	    ctx.fillStyle = "rgb("+rand_red+","+rand_green+","+rand_blue+")";

	    ctx.fillRect(x,y,PIXEL_SIZE,PIXEL_SIZE);
	}
    }


    ctx.restore();
    counter++;

}

init();
