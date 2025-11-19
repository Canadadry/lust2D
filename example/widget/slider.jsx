exports.draw = function (props) {
    return <rectangle class="fit-y grow-x lh p-1 m-1" color="#aa0">
      <txt msg={props.name} class="grow max-bound-x" font_size={props.size} color="#000"></txt>
      <rectangle id={props.id + "-box"} class="fit-y grow-x ls" radius={1} color="#00a">
        <rectangle x={props.pos} w={props.size} h={props.size} radius={1} color="#f0a">
        </rectangle>
      </rectangle>
      <item class="fit ls">
        <txt msg={"255"} class="grow max-bound-x" font_size={props.size} color="#aa0"></txt>
        <txt msg={"" + Math.floor(props.val)} class="grow max-bound-x" font_size={props.size} color="#000"></txt>
      </item>
    </rectangle>
  }

exports.update = function(model){
    if(is_mouse_button_pressed("left")){
      var node = ui_pick(get_mouse_x(), get_mouse_y());
      if(node==(model.id+"-box")){
        model.pressed = true;
      }
    }else if(is_mouse_button_released("left")){
      model.pressed = false;
    }
    if(model.pressed){
      var b = ui_bb(model.id+"-box");
      var local_x = get_mouse_x() - b.x - model.size/2;
      if (local_x < 0) { local_x = 0; }
      if (local_x > b.w-model.size) { local_x = b.w-model.size; }
      model.pos = local_x;
      model.val = map( 0, b.w - model.size, model.from, model.to,local_x);
    }
    return model;
  }
exports.init=function(id,name){
    return {
      id:id,
      name:name,
      val: 100,
      from: 0,
      to: 255,
      pos: 0,
      pressed: false,
      size: 20,
    };
  }
