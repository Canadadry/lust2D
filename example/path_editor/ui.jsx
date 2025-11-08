var type_move = 0;
var type_line = 1;
var type_bezier = 2;

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
  return <rectangle id={props.id} class="fit-y grow-x lh p-1 m-1" color="#aa0">
    <txt id={props.id+"-name"} msg={props.name} class="grow max-bound-x" font_size={20} color="#000"></txt>
    <rectangle id={props.id+"-box"} class="fit-y grow-x ls" radius={1} color="#00a">
      <rectangle id={props.id + "-knob"} w={20} h={20} radius={1} color="#f0a">
      </rectangle>
    </rectangle>
    <txt id={props.id+"-val"} msg={""+props.val} class="grow max-bound-x" font_size={20} color="#000"></txt>
</rectangle>
}

var Colors=function(props){
  return <item id={props.id} class="fit-y grow-x lv">
    <item id="item-c" class="fit-y grow-x  lh">
      <rectangle id="item-fill" class="grow-x" h={30} color={props.colors.fill}></rectangle>
      <rectangle id="item-border" class="grow-x" h={30} color={props.colors.border}></rectangle>
    </item>
    <item id="item-s-r" class="fit-y grow-x  lh">
      <Slider id="silder-r" name="R" min={0} max={255} val={props.colors.fill.r}></Slider>
    </item>
    <item id="item-s-g" class="fit-y grow-x  lh">
      <Slider id="silder-g" name="G" min={0} max={255} val={props.colors.fill.g}></Slider>
    </item>
    <item id="item-s-b" class="fit-y grow-x lh">
      <Slider id="silder-b" name="B" min={0} max={255} val={props.colors.fill.b}></Slider>
    </item>
  </item>
}

var Path=function(props){
  return (<rectangle id={props.id} class="fit m-2 p-2 lv" color="#aaa">
    <item id={props.id+"-icns"} class="fit lh m-1">
      <Icon id="new" at_x={1} at_y={2}></Icon>
      <Icon id="load" at_x={0} at_y={3}></Icon>
      <Icon id="save" at_x={3} at_y={2}></Icon>
    </item>
    {props.segments.filter(function(s,id){
      return id > 0;
    }).map(function(s,id){
      return <item id={"item-"+id+1} class="lh fit m-2">
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
  return <item id="root" class="fit lv">
    <Colors id="colors" colors={props.colors}></Colors>
    <Path id="path" segments={props.segments}></Path>
  </item>
}
