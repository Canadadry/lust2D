var useStateLib = require('useState');
var useState = useStateLib.useState;
var startApp = useStateLib.startApp;
useStateLib.registerFunc(ui_create, ui_draw, ui_clear);
createNode = function(node_type,props,children){
  return { "type": node_type, "props": props, "children":children };
}

Buton = function(props){
  return <txt msg={props.name} class="grow max-bound-x" font_size={props.size} color="#000"></txt>
}

TodoApp = function (initialState) {
  var state = useState(initialState);
  var todos = state[0];
  var setTodos = state[1];

  return <item>
    <Buton name="add" onClick={function () {setTodos(todos.concat(["task"]));}}></Buton>
    <Buton name="remove" onClick={function () {setTodos(todos.slice(1));}}></Buton>
    {todos ? todos.map(function(t){return <txt msg={t}></txt>}):<txt msg="empty"></txt>}
  </item>
};

conf = function () {
  window.width = 800;
  window.height = 600;
  window.title = "widget";
}

init = function (){
  try{
  startApp(TodoApp, ["A", "B", "C"]);
  console.log(JSON.stringify(TodoApp(), null, 2));
  }catch(e){
    console.log(e.message, e.stack);
  }
}

render = function () {
  ClearBackground("#ff0");
  // try{
  //   startApp(<TodoApp></TodoApp>);
  // }catch(e){
  //   console.log(e.message, e.stack);
  // }
}
