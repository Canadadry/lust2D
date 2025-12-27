var useStateLib = require('useState');
var useState = useStateLib.useState;
var startApp = useStateLib.startApp;
// var createNode = ui_create;
// var createNode =  function(node_type,props,children){
//   console.log("type", node_type, "props", props, "children",children );
//   return ui_create(node_type, props, children);
// }
var createNode = useStateLib.renderComponent;

Label = function(props){
  return <txt msg={props.name} class="grow max-bound-x" font_size={24} color="#000"></txt>
}

Buton = function(props){
  return <txt msg={props.name} class="grow max-bound-x" font_size={24} color="#000"></txt>
}

TodoApp = function (initialState) {
  var state = useState(initialState);
  var todos = state[0];
  var setTodos = state[1];

  return <item class="fit lv">
    <Buton name="add" onClick={function () {setTodos(todos.concat(["task"]));}}></Buton>
    <Buton name="remove" onClick={function () {setTodos(todos.slice(1));}}></Buton>
    {todos ? todos.map(function(t){return <Label name={t}></Label>}):<Label name="empty"></Label>}
  </item>
};
conf = function () {
  window.width = 800;
  window.height = 600;
  window.title = "widget";
}

init = function (){
  // startApp(TodoApp, ["A", "B", "C"]);
  // console.log(JSON.stringify(TodoApp(), null, 2));
}

render = function () {
  ClearBackground("#ff0");
  ui_clear();
  startApp(<TodoApp></TodoApp>);
}
