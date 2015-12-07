function roundedRect(ctx,x,y,width,height,radius){
  ctx.beginPath();
  ctx.moveTo(x,y+radius);
  ctx.lineTo(x,y+height-radius);
  ctx.quadraticCurveTo(x,y+height,x+radius,y+height);
  ctx.lineTo(x+width-radius,y+height);
  ctx.quadraticCurveTo(x+width,y+height,x+width,y+height-radius);
  ctx.lineTo(x+width,y+radius);
  ctx.quadraticCurveTo(x+width,y,x+width-radius,y);
  ctx.lineTo(x+radius,y);
  ctx.quadraticCurveTo(x,y,x,y+radius);
  ctx.stroke();
}

var canvas = document.getElementById('tutorial');

  // Make sure we don't execute when canvas isn't supported
  if (canvas.getContext){

    // use getContext to use the canvas for drawing
    var ctx = canvas.getContext('2d');

    // Draw shapes
    roundedRect(ctx,12,12,150,150,15);
    roundedRect(ctx,19,19,150,150,9);
    roundedRect(ctx,53,53,49,33,10);
    roundedRect(ctx,53,119,49,16,6);
    roundedRect(ctx,135,53,49,33,10);
    roundedRect(ctx,135,119,25,49,10);

    // Character 1
    ctx.beginPath();
    ctx.arc(37,37,13,Math.PI/7,-Math.PI/7,false);
    ctx.lineTo(34,37);
    ctx.fill();

    // blocks
    for(i=0;i<8;i++){
      ctx.fillRect(51+i*16,35,4,4);
    }
    for(i=0;i<6;i++){
      ctx.fillRect(115,51+i*16,4,4);
    }
    for(i=0;i<8;i++){
      ctx.fillRect(51+i*16,99,4,4);
    }

    // character 2
    ctx.beginPath();
    ctx.moveTo(83,116);
    ctx.lineTo(83,102);

    ctx.bezierCurveTo(83,94,89,88,97,88);
    ctx.bezierCurveTo(105,88,111,94,111,102);
    ctx.lineTo(111,116);
    ctx.lineTo(106.333,111.333);
    ctx.lineTo(101.666,116);
    ctx.lineTo(97,111.333);
    ctx.lineTo(92.333,116);
    ctx.lineTo(87.666,111.333);
    ctx.lineTo(83,116);
    ctx.fill();
    ctx.fillStyle = "white";
    ctx.beginPath();
    ctx.moveTo(91,96);
    ctx.bezierCurveTo(88,96,87,99,87,101);
    ctx.bezierCurveTo(87,103,88,106,91,106);
    ctx.bezierCurveTo(94,106,95,103,95,101);
    ctx.bezierCurveTo(95,99,94,96,91,96);
    ctx.moveTo(103,96);
    ctx.bezierCurveTo(100,96,99,99,99,101);
    ctx.bezierCurveTo(99,103,100,106,103,106);
    ctx.bezierCurveTo(106,106,107,103,107,101);
    ctx.bezierCurveTo(107,99,106,96,103,96);
    ctx.fill();
    ctx.fillStyle = "black";
    ctx.beginPath();
    ctx.arc(101,102,2,0,Math.PI*2,true);
    ctx.fill();
    ctx.beginPath();
    ctx.arc(89,102,2,0,Math.PI*2,true);
    ctx.fill();
  }
