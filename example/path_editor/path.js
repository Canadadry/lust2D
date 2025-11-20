var type_move = 0;
var type_line = 1;
var type_bezier = 2;

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


exports.new_canvas= function (model){
  var offset = 20;
  model.segments = [
    Segment(type_move,Point(offset,offset)),
    Segment(type_bezier,
      Point(window.width-offset-model.canvas_offset,window.height-offset),
      Point(window.width-offset-model.canvas_offset,offset*2),
      Point(window.width-offset*2-model.canvas_offset,offset)
    ),
    Segment(type_line,Point(offset,window.height - offset))
  ];
  model.dirty = true;
}

function updateCanvas(model){
  SetCanvas("canvas");
  ClearCanvas("#fff");
  SetFillColor(model.fill);
  SetOutlineColor(model.border);
  SetOutlineWidth(model.width);
  for (var i = 0; i < model.segments.length;i++){
    if(model.segments[i].kind==type_move){
      Close();
      MoveTo(model.segments[i].p);
    } else if(model.segments[i].kind==type_line){
      LineTo(model.segments[i].p);
    } else if(model.segments[i].kind==type_bezier){
      BezierTo(model.segments[i].c1,model.segments[i].c2,model.segments[i].p,20);
    }
  }
  Close();
  CanvasToImage("shape");
  model.dirty = false;
}

function close_enough(p1,p2,dist){
  return ((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)) < (dist * dist);
}

exports.handle_click = function (model) {
  var mx = get_mouse_x();
  var my = get_mouse_y();
  if (is_mouse_button_pressed("left")) {
    for (var j = 0; j < model.segments.length; j++) {
      var points = [model.segments[j].p];
      if(model.segments[j].kind==type_bezier){
        points.push(model.segments[j].c1);
        points.push(model.segments[j].c2);
      }
      for (var i = 0; i < points.length; i++) {
        var dist = 10;
        var m = Point(mx-model.canvas_offset, my);
        if (close_enough(m, points[i],dist)) {
          model.point_moved = points[i];
        }
      }
    }
  } else if (is_mouse_button_released("left")) {
    model.point_moved = null;
  }else if(model.point_moved != null){
    model.point_moved.x = mx - model.canvas_offset;
    model.point_moved.y = my;
    model.dirty = true;
  }
  return model
}

exports.init =function () {
  var model = {
    point_moved: null,
    dirty: false,
    segments: [],
    canvas_offset:200,
    fill:"#aaffee",
    border:"#ffaaee",
    width:10,
  };
  NewCanvas("canvas", window.width-model.canvas_offset, window.height);
  exports.new_canvas(model);
  return model;
}

exports.split_segment=function(model,idx){
  if (model.segments[idx].kind == type_line) {
    var middle = splitLine(model.segments[idx - 1].p, model.segments[idx].p);
    var end = Point(model.segments[idx].p.x, model.segments[idx].p.y);
    model.segments[idx].p = middle
    model.segments.splice(idx + 1, 0, Segment(type_line, end));
    model.dirty = true;
  } else if (model.segments[idx].kind == type_bezier) {
    var to_split = model.segments[idx]
    var splitted = splitBezier(model.segments[idx - 1].p, to_split.c1, to_split.c2, to_split.p);
    model.segments[idx].c1 = splitted[0].c1;
    model.segments[idx].c2 = splitted[0].c2;
    model.segments[idx].p = splitted[0].p;
    model.segments.splice(idx + 1, 0, Segment(type_bezier, splitted[1].p, splitted[1].c1, splitted[1].c2));
    model.dirty = true;
  }
}
exports.delete_segment=function(model,parsed_idx){
  model.segments = model.segments.filter(function (s, idx) { return idx != parsed_idx });
  model.dirty = true;
}
exports.swap_segment=function(model,parsed_idx){
  if (model.segments[parsed_idx].kind == type_line) {
    var b = lineToBezier(model.segments[parsed_idx - 1].p, model.segments[parsed_idx].p);
    model.segments[parsed_idx].c1 = b.c1;
    model.segments[parsed_idx].c2 = b.c2;
    model.segments[parsed_idx].kind = type_bezier;
  } else {
    model.segments[parsed_idx].kind = type_line;
  }
  model.dirty = true;
}

exports.render =function (model) {
  if (model.dirty) {
    updateCanvas(model)
  }
  ClearBackground("#fff");
  DrawImagePro(
    "shape",
    undefined,
    { x: model.canvas_offset, y: 0, w: window.width-model.canvas_offset, h: window.height }
  );
  for (var j = 0; j < model.segments.length; j++) {
     var points = [{ p: model.segments[j].p, c: "#0f0" }];
     if(model.segments[j].kind==type_bezier){
       points.push({ p: model.segments[j].c1, c: "#00f" });
       points.push({ p: model.segments[j].c2, c: "#00f" });
    }
    for (var i = 0; i < points.length; i++) {
      var size = 10;
      var c = points[i].c;
      if (model.point_moved == points[i].p) {
        c = "#f00";
      }
      DrawRectangleRec({ x: points[i].p.x +model.canvas_offset- size / 2, y: points[i].p.y - size / 2, w: size, h: size }, c);
    }
  }
}
