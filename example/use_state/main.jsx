var useStateLib = require('useState');
var useState = useStateLib.useState;
var startApp = useStateLib.startApp;
// var createNode = ui_create//useStateLib.renderComponent;
createNode = function(node,props,children){
  if(typeof node==="function"){
    console.log("cmpt found");
    return node(props, children);
  }
  var id = ui_create(node,props,children)
  console.log("id",id,"node", node, "props", JSON.stringify(props), "children",JSON.stringify(children) );
  return id;
}

Label = function(props){
  return <txt msg={props.name} class="grow max-bound-x" font_size={24} color="#000"></txt>
}

Buton = function(props){
  return <txt msg={props.name} class="grow max-bound-x" font_size={24} color="#000"></txt>
}

TodoApp = function (props) {
  var state = useState(props.init);
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
}

render = function () {
  ClearBackground("#ff0");
  ui_clear();
  var root_id = <TodoApp init={["a","b","c"]}></TodoApp>
  ui_compute(root_id);
  ui_draw(root_id);

  // startApp(<TodoApp></TodoApp>);
}
