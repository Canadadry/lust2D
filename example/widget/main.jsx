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

var val = 100;

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
      Slider({id:"line",name:"test",val:val})
    }</item>
  )
  ui_draw(root);
  var b = ui_bb("line-box");
  console.log("box : ",JSON.stringify(b));
}
