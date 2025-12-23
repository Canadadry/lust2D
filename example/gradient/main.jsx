var createNode = ui_create
var size = 25;
var points = [ { at: { x: 50, y :50 }, color: "#ff0" },{ at: { x: 750, y: 550 }, color: "#0ff" },  ];
var point_moved = null;

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
  DrawRectangleRec({ x: 0, y: 0, w: window.width, h: window.height },0.5,{
    start:{ x: points[0].at.x/window.width, y: points[0].at.y/window.height },
    end:{   x: points[1].at.x/window.width, y: points[1].at.y/window.height },
    from:points[0].color,
    to:points[1].color,
  })
  for (var i = 0; i < points.length;i+=1){
    var size_p = 10;
    var x = points[i].at.x;
    var y = points[i].at.y;
    DrawRectangleRec({ x: x - size_p / 2, y: y - size_p / 2, w: size_p, h: size_p }, 0.25,"#000");
    size_p = 8;
    DrawRectangleRec({ x: x-size_p/2, y: y-size_p/2, w:size_p, h: size_p },  1,points[i].color);
  }
  ui_draw(ui_compute(
    <rectangle x={200} y={300} w={300} h={50} fill={{
      start: { x: 0, y: 0.5 },
      end: { x: 1, y: 0.5 },
      from: points[0].color,
      to: points[1].color,
    }}></rectangle>
  ));


}
