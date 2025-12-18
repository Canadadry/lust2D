
var a = { x: 50, y :50 };
var b = { x: 750, y: 550 };
var color_a = { r: 255, g: 0, b: 0 };
var color_b = { r: 0, g: 255, b: 0 };
var size = 25;

var points = [ { at: a, color: color_a },{ at: b, color: color_b },  ];
var point_moved = 0;

function project(x,y){
  var ab = { x: b.x - a.x, y: b.y - a.y };
  var ap = { x: x - a.x, y: y - a.y };
  var s = ab.x * ap.x + ab.y * ap.y;
  var n_ab_2 = ab.x * ab.x + ab.y * ab.y;
  var p = s / n_ab_2;
  if (p < 0) p = 0;
  if (p > 1) p = 1;
  return p;
}

function compute_color(x,y){
  var p = project(x, y);
  return {
    r: color_b.r * p + color_a.r * (1 - p),
    g: color_b.g * p + color_a.g * (1 - p),
    b: color_b.b * p + color_a.b * (1 - p)
  };
}

function close_enough(p1,p2,dist){
  return ((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)) < (dist * dist);
}

function handle_click () {
  var dist = 10;
  var mx = get_mouse_x();
  var my = get_mouse_y();
  if (is_mouse_button_pressed("left")) {
    for (var i = 0; i < points.length; i++) {
      if (close_enough({ x: mx, y: my }, points[i].at,dist)) {
          point_moved = i;
      }
    }
  } else if (is_mouse_button_released("left")) {
    point_moved = null;
  }else if(point_moved != null){
    points[point_moved].at.x = mx;
    points[point_moved].at.y = my;
  }
}

function conf() {
  window.width = 800;
  window.height = 600;
  window.title = "Gradient";
}

function init() {
}

function render() {
  handle_click();
  ClearBackground({ r: 128, g: 128, b: 128 });
  for (var x = size/2; x <= window.width-size/2; x+=size){
    for (var y = size/2; y <= window.height-size/2; y+=size){
      DrawRectangleRec({ x: x-size/2, y: y-size/2, w:size, h: size }, compute_color(x,y));
    }
  }
  for (var i = 0; i < points.length;i+=1){
    var size_p = 10;
    var x = points[i].at.x;
    var y = points[i].at.y;
    DrawRectangleRec({ x: x-size_p/2, y: y-size_p/2, w:size_p, h: size_p }, "#000");
    size_p = 8;
    DrawRectangleRec({ x: x-size_p/2, y: y-size_p/2, w:size_p, h: size_p }, points[i].color);
  }

}
