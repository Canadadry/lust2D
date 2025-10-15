var Button = function (props,children) {
  return <rectangle id={props.id} class="fit m-2 p-2" x={props.x} y={props.y} r={props.r} g={props.g} b={props.b}>
    {children}
  </rectangle>
};

exports.init = function(){
  return { x: 0, y: 0 , offset_x:0,offset_y:0,pressed:false};
}

exports.update = function(model,event){
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
    <Button id="root" x={props.x} y={props.y} r={0} g={255} b={0}>
      <rectangle id="first" w={50} h={50} r={0} g={0} b={255}></rectangle>
      <rectangle id="second" w={10} h={150} r={255} g={0} b={255}></rectangle>
      <img src="spritesheet.png" class="grow"></img>
    </Button>
  );
};
