var ui = require('ui');

var Point=function(x,y){
  return { x: x || 0, y: y || 0 };
}
var Segment=function(kind,p,c1,c2){
  return {
    kind: kind,
    c1: c1 ||Point(),
    c2: c2 || Point(),
    p: p|| Point()
  };
}

function splitBezier(p1, c1, c2, p2) {
  var p12    = splitLine(p1, c1);
  var c12    = splitLine(c1, c2);
  var c23    = splitLine(c2, p2);
  var p123   = splitLine(p12, c12);
  var c123   = splitLine(c12, c23);
  var middle = splitLine(p123, c123);
  var firstCurve = { c1: p12, c2: p123, p: middle };
  var secondCurve = { c1: c123, c2: c23, p: p2 };
  return [firstCurve, secondCurve];
}

function splitLine(a, b) {
    return {
        x: (a.x + b.x) / 2,
        y: (a.y + b.y) / 2
    };
}

function lineToBezier(a,b){
  var m = splitLine(a, b);
  var c1 = splitLine(a, m);
  var c2 = splitLine(b, m);
  return { c1: c1, c2: c2, p: b };
}

function parse_node_id(str) {
  if(str==null){
    return null;
  }
  var regex = /^(\d+)-(\w+)$/;
  var match = str.match(regex);
  if (match) {
      var idx = parseInt(match[1], 10);
      var kind = match[2];
      return { idx:idx, kind:kind };
  } else {
      return null;
  }
}


var segments = [];
var point_moved = null;
var dirty = false;
var canvas_offset = 200;
function conf() {
  window.width = 800;
  window.height = 600;
  window.title = "path editor";
}

function init() {
  NewCanvas("canvas", window.width-canvas_offset, window.height);
  new_canvas();
}

function new_canvas(){
  var offset = 20;
  segments = [
    Segment("move",Point(offset,offset)),
    Segment("bezier",
      Point(window.width-offset-canvas_offset,window.height-offset),
      Point(window.width-offset-canvas_offset,offset*2),
      Point(window.width-offset*2-canvas_offset,offset)
    ),
    Segment("line",Point(offset,window.height - offset))
  ];
  dirty = true;
}

function updateCanvas(){
  SetCanvas("canvas");
  ClearCanvas("#fff");
  SetFillColor("#aaffee");
  for (var i = 0; i < segments.length;i++){
    if(segments[i].kind=="move"){
      Close();
      MoveTo(segments[i].p);
    } else if(segments[i].kind=="line"){
      LineTo(segments[i].p);
    } else if(segments[i].kind=="bezier"){
      BezierTo(segments[i].c1,segments[i].c2,segments[i].p,20);
    }
  }
  Close();
  CanvasToImage("shape");
  dirty = false;
}

function close_enough(p1,p2,dist){
  return ((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)) < (dist * dist);
}

function move_point() {
  if (is_mouse_button_pressed("left")) {
    for (var j = 0; j < segments.length; j++) {
      var points = [segments[j].p];
      if(segments[j].kind=='bezier'){
        points.push(segments[j].c1);
        points.push(segments[j].c2);
      }
      for (var i = 0; i < points.length; i++) {
        var dist = 10;
        var m = Point(get_mouse_x()-canvas_offset, get_mouse_y());
        if (close_enough(m, points[i],dist)) {
          point_moved = points[i];
        }
      }
    }
  } else if (is_mouse_button_released("left")) {
    point_moved = null;
  }else if(point_moved != null){
    point_moved.x = get_mouse_x() - canvas_offset;
    point_moved.y = get_mouse_y();
    dirty = true;
  }
}

function handle_click(){
  if(is_mouse_button_released("left")&& point_moved==null){
    var node = ui_pick(get_mouse_x(), get_mouse_y());
    if(node=="new"){
      new_canvas();
    }else if(node=="load"){
      if(file_exist("backup.json")){
        segments=JSON.parse(read("backup.json"));
        dirty = true;
      }
    }else if(node=="save"){
      write("backup.json", JSON.stringify(segments,null,2));
    }
    var parsed = parse_node_id(node);
    if (parsed!=null){
      if (parsed.idx < segments.length) {
        if (parsed.kind == "split") {
          if (segments[parsed.idx].kind == 'line') {
            var middle = splitLine(segments[parsed.idx - 1].p, segments[parsed.idx].p);
            var end = Point(segments[parsed.idx].p.x, segments[parsed.idx].p.y);
            segments[parsed.idx].p = middle
            segments.splice(parsed.idx+1, 0, Segment('line', end));
            dirty = true;
          }else if (segments[parsed.idx].kind == 'bezier') {
            var to_split = segments[parsed.idx]
            var splitted = splitBezier(segments[parsed.idx - 1].p, to_split.c1, to_split.c2, to_split.p);
            segments[parsed.idx].c1 = splitted[0].c1;
            segments[parsed.idx].c2 = splitted[0].c2;
            segments[parsed.idx].p = splitted[0].p;
            segments.splice(parsed.idx+1, 0, Segment('bezier', splitted[1].p, splitted[1].c1, splitted[1].c2));
            dirty = true;
          }
        }else if (parsed.kind == "delete" && segments.length > 2) {
            segments = segments.filter(function (s, idx) { return idx != parsed.idx });
            dirty = true;
        }else if (parsed.kind == "swap") {
          if(segments[parsed.idx].kind=="line"){
            var b = lineToBezier(segments[parsed.idx - 1].p, segments[parsed.idx].p);
            segments[parsed.idx].c1 = b.c1;
            segments[parsed.idx].c2 = b.c2;
            segments[parsed.idx].kind = "bezier";
          }else{
            segments[parsed.idx].kind = "line";
          }
          dirty = true;
        }
      }
    }
  }
}

function render() {
  move_point();
  if (dirty) {
    updateCanvas()
  }
  ClearBackground("#fff");
  DrawImagePro(
    "shape",
    undefined,
    { x: canvas_offset, y: 0, w: window.width-canvas_offset, h: window.height }
  );
  for (var j = 0; j < segments.length; j++) {
     var points = [{ p: segments[j].p, c: "#0f0" }];
    if(segments[j].kind=='bezier'){
      points.push({ p: segments[j].c1, c: "#00f" });
      points.push({ p: segments[j].c2, c: "#00f" });
    }
    for (var i = 0; i < points.length; i++) {
      var size = 10;
      var c = points[i].c;
      if (point_moved == points[i].p) {
        c = "#f00";
      }
      DrawRectangleRec({ x: points[i].p.x +canvas_offset- size / 2, y: points[i].p.y - size / 2, w: size, h: size }, c);
    }
  }

  ui_clear();
  var root = ui.build({ segments: segments })
  ui_compute(root);
  handle_click();
  ui_draw(root);

}
