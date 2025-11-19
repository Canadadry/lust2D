var Slider = function(props){
  return <rectangle class="fit-y grow-x lh p-1 m-1" color="#aa0">
    <txt msg={props.name} class="grow max-bound-x" font_size={props.size} color="#000"></txt>
    <rectangle id={props.id+"-box"} class="fit-y grow-x ls" radius={1} color="#00a">
      <rectangle x={props.pos} w={props.size} h={props.size} radius={1} color="#f0a">
      </rectangle>
    </rectangle>
    <item class="fit ls">
      <txt msg={"255"} class="grow max-bound-x" font_size={props.size} color="#aa0"></txt>
      <txt msg={""+Math.floor(props.val)} class="grow max-bound-x" font_size={props.size} color="#000"></txt>
    </item>
</rectangle>
}

var val = 100;
var from = 0;
var to = 255;
var pos = 0;
var pressed = false;
var size = 20;

function conf() {
  window.width = 800;
  window.height = 600;
  window.title = "widget";
}

function init() {
}

function render() {
  ClearBackground("#fff");
  ui_clear();
  var root = ui_compute(<item w={200}>{
    Slider({id:"line",name:"test",val:val,pos:pos,size:size})
  }</item>);
  ui_draw(root);

  if(is_mouse_button_pressed("left")){
    var node = ui_pick(get_mouse_x(), get_mouse_y());
    if(node=="line-box"){
      pressed = true;
    }
  }else if(is_mouse_button_released("left")){
    pressed = false;
  }
  if(pressed){
    var b = ui_bb("line-box");
    var local_x = get_mouse_x() - b.x - size/2;
    if (local_x < 0) { local_x = 0; }
    if (local_x > b.w-size) { local_x = b.w-size; }
    pos = local_x;
    val = map( 0, b.w - size, from, to,local_x);
  }
}
