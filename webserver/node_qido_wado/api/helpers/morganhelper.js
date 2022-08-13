const morgan = require("morgan");
morgan.token("date", function () {
  const p = new Date()
    .toString()
    .replace(/[A-Z]{3}\+/, "+")
    .split(/ /);
  return p[2] + "/" + p[1] + "/" + p[3] + ":" + p[4] + " " + p[5];
});
morgan.format(
  "customformat",
  '[:date[clf]] ":method :url" :status :res[content-length] - :response-time ms'
);

module.exports = (app) => {
  app.use(morgan("customformat"));
};
