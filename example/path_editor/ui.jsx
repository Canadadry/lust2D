var WColor = require('../widget/color');
var Path = require('path');

var type_move = 0;
var type_line = 1;
var type_bezier = 2;

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

function type_to_string(t){
  if(t==type_move){
    return "move"
  }else if(t==type_line){
    return "line"
  }else if(t==type_bezier){
    return "bezier"
  }
}

var Icon = function(props){
  return <tile
    id={props.id} at_x={props.at_x || 0} at_y={props.at_y || 0} src="spritesheet.png"
    w={26} h={26} tile_padding_x={0} tile_padding_y={0} tile_size_x={26} tile_size_y={26}
  ></tile>;
}

var Slider = function(props){
  return <rectangle class="fit-y grow-x lh p-1 m-1" color="#aa0">
    <txt msg={props.name} class="grow max-bound-x" font_size={20} color="#000"></txt>
    <rectangle id={props.id+"-box"} class="fit-y grow-x ls" radius={1} color="#00a">
      <rectangle w={20} h={20} radius={1} color="#f0a">
      </rectangle>
    </rectangle>
    <txt msg={""+props.val} class="grow max-bound-x" font_size={20} color="#000"></txt>
</rectangle>
}

var Colors=function(props){
  return <item id={props.id} class="fit-y grow-x lv">
    <item class="fit-y grow-x  lh">
      <rectangle id="item-fill" class="grow-x" h={30} color={WColor.export(props.colors.fill)}></rectangle>
      <rectangle id="item-border" class="grow-x" h={30} color={WColor.export(props.colors.border)}></rectangle>
    </item>
    {props.mode=="fill"?WColor.draw(props.colors.fill):WColor.draw(props.colors.border)}
  </item>
}

var PathView=function(props){
  return (<rectangle class="fit m-2 p-2 lv" color="#aaa">
    <item class="fit lh m-1">
      <Icon id="new" at_x={1} at_y={2}></Icon>
      <Icon id="load" at_x={0} at_y={3}></Icon>
      <Icon id="save" at_x={3} at_y={2}></Icon>
    </item>
    {props.segments.filter(function(s,id){
      return id > 0;
    }).map(function(s,id){
      return <item class="lh fit m-2">
        <txt msg={type_to_string(s.kind)} class="grow min-x-70" font_size={20} ></txt>;
        {(s.kind==type_bezier)?(
          <Icon id={(id+1)+"-swap"} at_x={0} at_y={0}></Icon>
        ):(
          <Icon id={(id+1)+"-swap"} at_x={0} at_y={1}></Icon>
        )}
        <Icon id={(id+1)+"-split"} at_x={3} at_y={1}></Icon>
        {props.segments.length>=2?(
          <Icon id={(id+1)+"-delete"} at_x={3} at_y={4}></Icon>
        ):""}
      </item>
    })}
  </rectangle>);
}

exports.build=function(props){
  return <item class="fit lv">
    <Colors colors={props.colors} mode={props.mode}></Colors>
    <PathView segments={props.path.segments}></PathView>
  </item>
}

exports.init = function(){
  var model= {
    path: Path.init(),
    silder_width: 0,
    mode: "fill",
    colors: {
      fill: WColor.init("fill"),
      border: WColor.init("border")
    }
  }
  model.path.fill = WColor.export(model.colors.fill);
  model.path.border = WColor.export(model.colors.border);
  return model;
}

exports.handle_click = function(model){
  if(model.mode=="fill"){
    model.colors.fill = WColor.update(model.colors.fill);
    if( model.colors.fill.r.pressed==true
      ||model.colors.fill.g.pressed==true
      ||model.colors.fill.b.pressed==true
    ){
      model.path.dirty = true;
    }
  }else{
    model.colors.border = WColor.update(model.colors.border);
    if( model.colors.border.r.pressed==true
      ||model.colors.border.g.pressed==true
      ||model.colors.border.b.pressed==true
    ){
      model.path.dirty = true;
    }
  }

  var node = ui_pick(get_mouse_x(), get_mouse_y());
  if(is_mouse_button_released("left") && model.path.point_moved==null){
    if(node=="new"){
      model.colors.fill= WColor.init("fill");
      model.colors.border = WColor.init("border");
      Path.new_canvas(model.path);
      model.path.fill = WColor.export(model.colors.fill);
      model.path.border = WColor.export(model.colors.border);
    }else if(node=="item-fill"){
      model.mode = "fill";
    }else if(node=="item-border"){
      model.mode = "border";
    }else if(node=="load"){
      if(file_exist("backup.json")){
        model.path=JSON.parse(read("backup.json"));
        model.path.dirty = true;
      }
    }else if(node=="save"){
      write("backup.json", JSON.stringify(model.path));
    }else{
      var parsed = parse_node_id(node);
      if (parsed != null) {
        if (parsed.idx < model.path.segments.length) {
          if (parsed.kind == "split") {
            Path.split_segment(model.path,parsed.idx)
          } else if (parsed.kind == "delete" && model.path.segments.length > 2) {
            Path.delete_segment(model.path,parsed.idx)
          } else if (parsed.kind == "swap") {
            Path.swap_segment(model.path,parsed.idx)
          }
        }
      }
    }
  }
  if(model.path.dirty){
    model.path.fill = WColor.export(model.colors.fill);
    model.path.border = WColor.export(model.colors.border);
  }
  return model
}
