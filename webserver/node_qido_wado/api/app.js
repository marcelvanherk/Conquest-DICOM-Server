const express = require("express");
const app = express();
const port = 3002;
require("dotenv").config();
const fs = require("fs");
const cors = require("cors");
const path = require("path");
global.appRoot = path.resolve(__dirname);

const morganHelper = require("./helpers/morganhelper");

const dicomRoutes = require("./api/routes/dicomRoutes");

/* body parser */
app.use(express.json({ limit: "100mb" }));
app.use(
  express.urlencoded({
    limit: "100mb",
    extended: true,
    parameterLimit: 1000000,
  })
);

/* cors*/
app.options("*", cors());
app.use(cors());

/*log requests */
morganHelper(app);

/* hello world */
app.get("/", (req, res) => res.send("Hello World!"));

app.use("/api/dicom", dicomRoutes);

app.use((req, res, next) => {
  const error = new Error("Route Not found");
  error.status = 404;
  next(error);
});

app.use((error, req, res, next) => {
  console.log(error.stack);
  res.status(error.status || 500);
  return res.json({
    message: error.message,
  });
});

app.listen(port, () => console.log(`App listening on port ${port}!`));
