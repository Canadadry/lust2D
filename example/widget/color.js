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
  model.r.fill = {
    start:{ x: 0, y: 0.5 },
    end:{   x: 1, y: 0.5 },
    from:{r:0,g:model.g.val,b:model.b.val},
    to:{r:255,g:model.g.val,b:model.b.val},
  };
  model.g.fill = {
    start:{ x: 0, y: 0.5 },
    end:{   x: 1, y: 0.5 },
    from:{r:model.r.val,g:0,b:model.b.val},
    to:{r:model.r.val,g:255,b:model.b.val},
  };
  model.b.fill = {
    start:{ x: 0, y: 0.5 },
    end:{   x: 1, y: 0.5 },
    from:{r:model.r.val,g:model.g.val,b:0},
    to:{r:model.r.val,g:model.g.val,b:255},
  };
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
