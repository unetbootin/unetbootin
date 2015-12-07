var ctx = document.getElementById('tutorial').getContext('2d');
ctx.save();
  for (i=0;i<10;i++){
    ctx.lineWidth = 1+i;
    ctx.beginPath();
    ctx.moveTo(5+i*14,5);
    ctx.lineTo(5+i*14,140);
    ctx.stroke();
  }
ctx.restore();