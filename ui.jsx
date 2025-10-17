var Button = function (props,children) {
  return <rectangle id={props.id} class="fit m-2 p-2" x={props.x} y={props.y} color={props.color} r={props.r} g={props.g} b={props.b}>
    {children}
  </rectangle>
};


var Icon = function(props,children){
  return <tile
    src="spritesheet.png"
    at_x={props.at_x || 0}
    at_y={props.at_y || 0}
    w={26}
    h={26}
    tile_padding_x={0}
    tile_padding_y={0}
    tile_size_x={26}
    tile_size_y={26}
  ></tile>;
}

exports.init = function(){
  return { w:800,h:600,x: 0, y: 0 , offset_x:0,offset_y:0,pressed:false,at_x:3,at_y:4};
}

exports.update = function(model,event){
  if(is_key_pressed("left")) {model.at_x -= 1;}
  if(is_key_pressed("right")){model.at_x += 1;}
  if(is_key_pressed("up"))   {model.at_y -= 1;}
  if(is_key_pressed("down")) {model.at_y += 1;}
  if(event.name=="mouse_pressed" && event.target=="root"){
    model.pressed = true;
    model.offset_x = event.mouse_x - model.x;
    model.offset_y = event.mouse_y - model.y;
  }
  if(event.name=="mouse_released" && event.target=="root"){
    model.pressed = false;
    model.offset_x = 0;
    model.offset_y = 0;
  }
  if(model.pressed){
    model.x =  event.mouse_x - model.offset_x ;
    model.y =  event.mouse_y - model.offset_y ;
  }
  return model;
}

exports.render = function (props) {
  return (
    <Button id="root" x={props.x} y={props.y} color={"#0F0"}>
      <Icon at_x={props.at_x} at_y={props.at_y}></Icon>
      <rectangle id="first" w={50} h={50} color={"#FFFF00"}></rectangle>
      <rectangle id="second" w={10} h={150} color={"#FFFF00"}></rectangle>
      <img src="spritesheet.png" class="grow"></img>
      <txt msg="hello wlord" font_size={40} w={250} h={50} color="#000"></txt>
    </Button>
  );
};
