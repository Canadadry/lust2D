var test = require("requiered");

var root_ui = -1;
x = 10;
y = 10;
w = 100;
h = 100;
dx = 4;
dy = 4;

function conf() {
  window.width = 800;
  window.height = 600;
  window.title = "from lust2D";
}

function init(){
  root_ui = test.create();
  ui_compute(root_ui);
}

function render() {
  ClearBackground({ r: 128, g: 128, b: 128 });
  DrawRectangleRec({ x: x, y: y, w: w, h: h }, { r: 255, g: 0, b: 0 });
  x += dx;
  y += dy;
  if (x < 0 || x + w > window.width) {
    dx *= -1;
  }
  if (y < 0 || y + h > window.height) {
    dy *= -1;
  }
  ui_draw(root_ui);
}
