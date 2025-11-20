var Slider = require("../widget/slider")

exports.draw = function (props) {
  return [
    Slider.draw(props.r),
    Slider.draw(props.g),
    Slider.draw(props.b)
  ]
}
exports.update = function (model) {
  model.r = Slider.update(model.r);
  model.g = Slider.update(model.g);
  model.b = Slider.update(model.b);
  return model
}
exports.init=function(id){
  return{
    r:Slider.init(id+"color-r", "R"),
    g:Slider.init(id+"color-g", "G"),
    b:Slider.init(id+"color-b", "B"),
  }
}
exports.export=function(model){
  return {r:model.r.val,g:model.g.val,b:model.b.val,a:255}
}
