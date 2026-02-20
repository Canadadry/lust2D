var useStateLib = require('useState');
var useState = useStateLib.useState;
var startApp = useStateLib.startApp;
createNode = useStateLib.renderComponent;

Label = function(props){
  return <txt msg={props.name} class="grow max-bound-x" font_size={24} color="#000"></txt>
}

Buton = function(props){
  return <txt msg={props.name} class="grow max-bound-x" onClick={props.onClick} font_size={24} color="#000"></txt>
}

TodoApp = function (props) {
  var state = useState(props.init);
  var todos = state[0];
  var setTodos = state[1];

  return <item class="fit lv">
    <Buton name="add" onClick={function () {
      setTodos(todos.concat(["task"]));
      console.log("clicking on add")
    }}></Buton>
    <Buton name="remove" onClick={function () {
      console.log("clicking on remove")
      setTodos(todos.slice(1));
    }}></Buton>
    {todos ? todos.map(function (t) { return <Label key={t} name={t}></Label>}):<Label name="empty"></Label>}
  </item>
};
conf = function () {
  window.width = 800;
  window.height = 600;
  window.title = "widget";
}

init = function (){
}

render = function () {
  ClearBackground("#ff0");
  startApp(<TodoApp init={["a", "b", "c"]}></TodoApp>);
  if (is_mouse_button_released("left")) {
    ui_click(get_mouse_x(), get_mouse_y());
    // ui_dump_command();
  }

}
