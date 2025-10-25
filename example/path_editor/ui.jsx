var Icon = function(props){
  return <tile
    id={props.id}
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

exports.build=function(props){
  return (<rectangle class="fit m-2 p-2 lv" color="#aaa">
    {props.segments.filter(function(s,id){
      return id > 0;
    }).map(function(s,id){
      return <item class="lh fit m-2">
        <txt msg={s.kind} class="grow min-x-70" font_size={20} ></txt>;
        {(s.kind=="bezier")?(
          <Icon id={(id+1)+"-line"} at_x={0} at_y={0}></Icon>
        ):(
          <Icon id={(id+1)+"-bezier"} at_x={0} at_y={1}></Icon>
        )}
        <Icon id={(id+1)+"-split"} at_x={3} at_y={1}></Icon>
        {props.segments.length>=2?(
          <Icon id={(id+1)+"-delete"} at_x={3} at_y={4}></Icon>
        ):""}
      </item>
    })}
  </rectangle>);
}
