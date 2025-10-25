var Point=function(x,y){
  return { x: x || 0, y: y || 0 };
}

var points = [Point(), Point(), Point(), Point(), Point()];
var point_moved = null;
var dirty = false;

function conf() {
  window.width = 800;
  window.height = 600;
  window.title = "path editor";
}

function init() {
  NewCanvas("canvas", window.width, window.height);
  var offset = 20;
  points[0].x=offset
  points[0].y=offset
  points[1].x=window.width-offset
  points[1].y=offset*2
  points[2].x=window.width-offset*2
  points[2].y=offset
  points[3].x=window.width-offset
  points[3].y=window.height-offset
  points[4].x=offset
  points[4].y=window.height-offset
  updateCanvas();
}

function updateCanvas(){
  SetCanvas("canvas");
  ClearCanvas("#fff");
  SetFillColor("#aaffee");
  MoveTo(points[0]);
  BezierTo(points[1],points[2],points[3],20);
  LineTo(points[4]);
  LineTo(points[0]);
  Close();
  CanvasToImage("shape");
}

function close_enough(p1,p2,dist){
  return ((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)) < (dist * dist);
}

function move_point() {
  if (is_mouse_button_pressed("left")) {
    for (var i = 0; i < points.length; i++) {
      var dist = 10;
      var m = Point(get_mouse_x(), get_mouse_y());
      if (close_enough(m, points[i],dist)) {
        point_moved = i;
      }
    }
  } else if (is_mouse_button_released("left")) {
    point_moved = null;
  }else if(point_moved != null){
    points[point_moved] = Point(get_mouse_x(), get_mouse_y());
    dirty = true;
  }
}

function render() {
  move_point();
  if(dirty){
    dirty = false;
    updateCanvas();
  }
  ClearBackground("#fff");
  DrawImagePro(
    "shape",
    undefined,
    { x: 0, y: 0, w: window.width, h: window.height }
  );
  for (var i = 0; i < points.length;i++){
    var size = 10;
    var c = "#0f0";
    if(point_moved==i){
      c = "#f00" ;
    }
    DrawRectangleRec({ x: points[i].x-size/2, y: points[i].y-size/2, w: size, h: size }, c);
  }
}
