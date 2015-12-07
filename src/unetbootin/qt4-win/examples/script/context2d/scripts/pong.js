// globals
playarea_canvas = document.getElementById('tutorial');
playarea_canvas.resize(320,200);
playarea = playarea_canvas.getContext('2d');
//p1_scr = document.getElementById('p1_scr');
//p2_scr = document.getElementById('p2_scr');
//status_msg = document.getElementById('status');
//debug = document.getElementById('debug');
ball_direction = 0;
up = -1;
down = 1;

//key codes
key_up = 38;
key_down = 40;
key_W = 87;
key_S = 83;
key_pause = 32;

speed = 2;			//controls the speed of the ball
paddle_inc = 10;	//how many pixels paddle can move in either direction
pause = false;

player_1 = 0;		//player IDs
player_2 = 1;
player_1_scr = 0;	//player scores
player_2_scr = 0;
player_1_direction = null;	//null = no movement whatsoever
player_2_direction = null;

pa = new Array();
divider = new Array();
paddle_1 = new Array();
paddle_2 = new Array();
ball = new Array();


function sleep(numberMillis)
{
    var now = new Date();
    var exitTime = now.getTime() + numberMillis;
    while (true) {
	now = new Date();
	if (now.getTime() > exitTime)
	    return;
    }
}

function init()
{
    pa['width'] = 150;
    pa['height'] = 140;
    pa['player_margin'] = 10;		//area behind player paddles
    pa['foreground'] = "#FFFFFF";
    pa['background'] = "#000000";

    divider['pos'] = pa['width']/2;
    divider['width'] = 4;

    paddle_1['width'] = 8;
    paddle_1['height'] = 64;
    paddle_1['x'] = pa['player_margin'];
    paddle_1['y'] = (pa['height'] /2 ) - (paddle_1['height'] / 2);

    paddle_2['width'] = 8;
    paddle_2['height'] = 64;
    paddle_2['x'] = (pa['width'] - pa['player_margin'] - paddle_2['width']);
    paddle_2['y'] = (pa['height'] /2 ) - (paddle_2['height'] / 2);

    ball['width'] = 10;
    ball['height'] = 10;
    ball['x'] = (pa['width']/2) - (ball['width'] / 2);
    ball['y'] = (pa['height']/2) - (ball['height'] / 2);

    ball_direction = Math.random() * 360;	//initialize ball direction, which is determined by angle, at random
    speed = 2;
}

function renderPlayarea()
{
    playarea.beginPath();

    playarea.clearRect(0,0,pa['width'],pa['height']);
    playarea.fillStyle = pa['background'];
    playarea.strokeStyle = pa['foreground'];
    playarea.fillRect(0,0, pa['width'], pa['height']);

    //move paddles
    if(player_1_direction != null)
	{
	    if(player_1_direction == up)
		paddle_1['y'] = paddle_1['y'] - paddle_inc;
	    else
		paddle_1['y'] = paddle_1['y'] + paddle_inc;
	}
    if(player_2_direction != null)
	{
	    if(player_2_direction == up)
		paddle_2['y'] = paddle_2['y'] - paddle_inc;
	    else
		paddle_2['y'] = paddle_2['y'] + paddle_inc;
	}
    playarea.rect(paddle_1['x'],paddle_1['y'],paddle_1['width'],paddle_1['height']);
    playarea.rect(paddle_2['x'],paddle_2['y'],paddle_2['width'],paddle_2['height']);

    //move ball
    playarea.rect(ball['x'], ball['y'], ball['width'], ball['height']);
    ball['x'] = ball['x'] + Math.cos((ball_direction)*Math.PI/180) * speed;
    ball['y'] = ball['y'] + Math.sin((ball_direction)*Math.PI/180) * speed;

    playarea.fillStyle = pa['foreground'];
    playarea.fill();

    playarea.beginPath();
    //redraw divider
    playarea.lineWidth = divider['width'];
    playarea.lineTo(divider['pos'], 0);
    playarea.lineTo(divider['pos'], pa['height'] = 200);
    playarea.lineWidth = 1;

    playarea.stroke();
    playarea.closePath();
}

function testCollisions()
{
    //make sure paddles don't go beyond play area
    if(((paddle_1['y'] <= 0) &&	(player_1_direction == up)) || ((paddle_1['y'] >= (pa['height'] - paddle_1['height'])) && (player_1_direction == down)))
	player_1_direction = null;
    if(((paddle_2['y'] <= 0) &&	(player_2_direction == up)) || ((paddle_2['y'] >= (pa['height'] - paddle_2['height'])) && (player_2_direction == down)))
	player_2_direction = null;

    //check to see if ball went beyond paddles, and if so, score accordingly and reset playarea
    if(ball['x'] <= 0)
	{
	    setScore(player_2);
	    init()
		sleep(1000);
	}
    if(ball['x'] >= (pa['width'] - ball['width']))
	{
	    setScore(player_1);
	    init();
	    sleep(1000);
	}

    //check to see if ball hit top or bottom wall. if so, change direction
    if((ball['y'] >= (pa['height'] - ball['height'])) || ball['y'] <= 0)
	ball_direction = -ball_direction;

    //check to see if the ball hit a paddle, and if so, change ball angle dependant on where it hit the paddle
    if((ball['x'] <= (paddle_1['x'] + paddle_1['width'])) && (ball['y'] >= paddle_1['y']) && (ball['y'] <= (paddle_1['y'] + paddle_1['height'])))
	{
	    ball_direction = -ball_direction/2;
	    speed += .5;
	}
    if(((ball['x'] + ball['width']) >= paddle_2['x']) && (ball['y'] >= paddle_2['y']) && (ball['y'] <= (paddle_2['y'] + paddle_2['height'])))
	{
	    ball_direction = (180+ball_direction)/2;
	    speed += .5;
	}
}

function setScore(p)
{
    if(p == player_1)
	{
	    player_1_scr++;
	    //p1_scr.firstChild.nodeValue = player_1_scr;
	}
    if(p == player_2)
	{
	    player_2_scr++;
	    //p2_scr.firstChild.nodeValue = player_2_scr;
	}
}


//handle input
document.onkeydown = function(ev)
{
    switch(ev.keyCode)
    {
	case key_W:
	player_1_direction = up;
	break;
	case key_S:
	player_1_direction = down;
	break;
	case key_up:
	player_2_direction = up;
	break;
	case key_down:
	player_2_direction = down;
	break;
    }
}

document.onkeyup = function(ev)
{
    switch(ev.keyCode)
    {
	case key_W:
	case key_S:
	player_1_direction = null;
	break;
	case key_up:
	case key_down:
	player_2_direction = null;
	break;
	case key_pause:
	if(pause == false)
	{
	    clearInterval(game);
	    //status_msg.style.visibility = "visible";
	    pause = true;
	}
	else
	{
	    game = setInterval(main, 25);
	    //status_msg.style.visibility = "hidden";
	    pause = false;
	}
	break;
    }
}

function main()
{
    testCollisions();
    renderPlayarea();
}

init();
game = setInterval(main, 25);
