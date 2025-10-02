exports.test = "test";
exports.create = function () {
  return (
    <rectangle x={10} h={200} g={0} r={0}>
      <rectangle x={10} h={50} g={0} r={128}></rectangle>
      <rectangle x={10} h={150} g={0} b={128}></rectangle>
    </rectangle>
  );
};
