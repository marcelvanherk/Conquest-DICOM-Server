//localhost:3002/api/dicom/rs/studies?limit=25&offset=0&fuzzymatching=false&includefield=00081030%2C00080060&StudyDate=20220801-20220816
/*----------------------------------------------------------------------------------------
 ----------------------Conquest Dicom API using node.js ----------------------------------
------------------------------------------------------------------------------------------
CQPORT         = Conquest Dicom Pacs Port 
CQIP           = Conquest Dicom Pacs IP 
CQAE           = Conquest Dicom Pacs AE TITLE
const dgateSpawn   = Dgate(64).exe command used spawn process
- I have noted spawn works better using promises. 
- I had problems using 'exec' in promises
- A future improvement will change all code to use 'spawn' instead of 'exec'

----------------------------Sample Resources From QIDO docs---------------------------------------
---Study's Instances---
/studies/{study}/instances{?search*}
---All Series---
/series{?parameter*}
---All Instances---
/instances{?search

--------------------------------------------other samples-----------------------------------------
 /studies/1.3.46.670589.30.1.6.1.116520970982.1491317778640.1/series/1.3.46.670589.30.1.6.1.116520970982.1491318133781.1/instances/:sopInstanceUid/frames/0
 /studies?PatientID=11235813
 /studies?PatientID=11235813&StudyDate=20130509
 /studies?00100010=SMITH*&00101002.00100020=11235813&limit=25
 /studies?00100010=SMITH*&OtherPatientIDsSequence.00100020=11235813
 /studies?PatientID=11235813&includefield=00081048,00081049,00081060
 /studies?PatientID=11235813&includefield=00081048&includefield=00081049&includefield=00081060
 /studies?PatientID=11235813&StudyDate=20130509-20130510
 /studies?StudyInstanceUID=1.2.392.200036.9116.2.2.2.2162893313.1029997326.94587,1.2.392.200036.9116.2.2.2.2162893313.1029997326.94583
 /studies?00230010=AcmeCompany&includefield=00231002&includefield=00231003
 /studies?00230010=AcmeCompany&00231001=001239&includefield=00231002&includefield=00231003
--------------------------------------------------------------------------------------------*/

const express = require("express");
const fs = require("fs");
const router = express.Router();
const path = require("path");
const crypto = require("crypto");
const { exec, spawn } = require("child_process");
const { tagsForLevel } = require("../../helpers/dicom/tags");
const url = require("url");
const os = require("os");

/* memory buffer used to get images*/
const MAX_BUFFER = 30000 * 1024;
const QPARAMS = [
  "includefield",
  "limit",
  "offset",
  "fuzzymatching",
  "viewport",
  "window",
  "accept",
  "annotation",
  "charset",
  "quality",
  "iccprofile",
];

/* used to cache the response if dicom conquest PACS is alive*/
let CQDICOMISALIVE = false;

/*------------------------------------------------------
PACS config - The develpoper needs to configure this data
Other option is take this data from DB query
It needs to be configured with your data
--------------------------------------------------------*/
let CQPORT = process.env.CQPORT || "5678";
let CQIP = process.env.CQIP || "127.0.0.1";
let CQAE = process.env.CQAE || "CONQUESTSRV1";
let APIFOLDER = `${global.appRoot}/api/dgate`;

let dgateSpawn = "servertask";
let dgateEx = `${dgateSpawn} -p${CQPORT} -h${CQAE} -q${CQIP} -w${APIFOLDER}`;
let dgateExBin = `${dgateEx}  -$`;

const isWin = () => {
  return os.platform() === "win32";
};

/*------------------------------------------------------------------
Execute dgate commands using 'spawn' and promises.
------------------------------------------------------------------*/
function os_func_spawn() {
  this.execCommand = (cmd) => {
    return new Promise((resolve, reject) => {
      let buffer = "";
      const child = spawn(
        dgateSpawn,
        [`-p${CQPORT}`, `-h${CQAE}`, `-q${CQIP}`, `-w${APIFOLDER}`, cmd],
        { cwd: APIFOLDER }
      );
      child.on("error", function (err) {
        reject(err);
      });
      child.stdout.on("data", (data) => {
        buffer += data;
      });

      child.stderr.on("data", (data) => {
        console.error(`stderr: ${data}`);
        reject(data.toString());
      });
      child.on("close", (code, signal) => {
        resolve(buffer.toString());
      });
      child.on("exit", function (code, signal) {
        if (code == 1) {
          reject("Error");
        }
      });
    });
  };
}

/*------------------------------------------------------------------
middleware to check if dicom is alive
------------------------------------------------------------------*/
const checkDicomIsAlive = async (req, res, next) => {
  if (CQDICOMISALIVE) {
    return next();
  }
  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);echo([[${CQAE}]])`;
  const os = new os_func_spawn();
  try {
    const dados = await os.execCommand(cmd);
    if (dados == "1") {
      CQDICOMISALIVE = true;
      return next();
    } else {
      console.log("Maybe Conquest is not running");
      return res.status(400).send({
        message: "Image server is not available",
      });
    }
  } catch (error) {
    console.log(error);
    console.log("Maybe Conquest is not running");
    return res.status(400).send({
      message: "PACS server is not available",
    });
  }
};

router.use(checkDicomIsAlive);

/*-------------------------------------------------
 Common function to get params to QIDO
--------------------------------------------------*/
const getParams = (params, tags, query) => {
  // add default tags to req params tags
  // attention: here params already can be populated with keys from req.params
  tags.forEach((element) => {
    const opt = { [element]: "" };
    params = { ...opt, ...params }; // order opt and params is important
  });

  // take query tags and add to params
  Object.keys(query || {}).forEach((propName) => {
    if (!QPARAMS.includes(propName)) {
      let v = query[propName];
      if (typeof v === "string" || v instanceof String) {
        v = v.replace(/,/g, "\\\\");
      }
      const opt = { [propName]: v };
      params = { ...params, ...opt }; // order opt and params is important
    }
  });
  return params;
};

/*-------------------------------------------------
 Common function to get tags to QIDO
--------------------------------------------------*/
const getDefaultTags = (level, query) => {
  let tags = [];
  let incfields = query?.includefield;
  if (incfields && incfields !== "all") {
    if (typeof incfields === "string" || incfields instanceof String) {
      incfields = incfields.split(",");
    }
    if (!Array.isArray(incfields)) {
      incfields = [incfields];
    }

    tags = [...tags, ...incfields];
  }
  const defaultTagsForLevel = tagsForLevel(level);
  tags = [...tags, ...defaultTagsForLevel];
  return tags;
};

/*-------------------------QIDO API--------------------------------
All studies
/studies{?search*}
------------------------------------------------------------------*/
router.get("/rs/studies", async (req, res) => {
  // const query = req.query;
  const query = url.parse(req.url, true).query;
  let params = { QueryRetrieveLevel: "STUDY" };
  let tags = getDefaultTags("STUDY", query);
  const tparams = getParams(params, tags, query);
  const offset = query?.offset ? parseInt(query.offset, 10) : 0;
  const limit = query?.limit ? parseInt(query.limit, 10) : 200;

  if (limit) tparams["99990C01"] = limit;
  if (offset) tparams["99990C02"] = offset;
  tparams["99990C00"] = "StudyDate"; // sort study date
  params = JSON.stringify(tparams);

  const dicomweb = true; // indicate json format
  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);studies([[${CQAE}]],[[${params}]],[[${dicomweb}]])`;
  try {
    let temp = [];
    const os = new os_func_spawn();
    const dados = await os.execCommand(cmd);
    temp = JSON.parse(dados);
    // StudyDate: item['00080020']?.Value?.[0],
    // if (temp && temp.length > 0) {
    //   temp.sort((a, b) => {
    //     if (a["00080020"]?.Value?.[0] == b["00080020"]?.Value?.[0]) return 0;
    //     if (a["00080020"]?.Value?.[0] < b["00080020"]?.Value?.[0]) return -1;
    //     if (a["00080020"]?.Value?.[0] > b["00080020"]?.Value?.[0]) return 1;
    //   });
    // }
    // if (limit) temp = temp.slice(offset, offset + limit);
    // else temp = temp.slice(offset);
    res.set("Content-Type", "application/json");
    res.status(temp.length > 0 ? 200 : 204).send(temp);
  } catch (error) {
    console.log(error);
    return res.status(400).send({
      message: "Error finding studies",
    });
  }
});

/*-------------------------QIDO API--------------------------------
 Study's Series
/studies/{study}/series{?search*}
------------------------------------------------------------------*/
router.get("/rs/studies/:study/series", async (req, res) => {
  const { study } = req.params;
  if (!study) {
    return res.status(400).send("Invalid study.");
  }
  const query = url.parse(req.url, true).query;
  let params = { QueryRetrieveLevel: "SERIES", StudyInstanceUID: study };
  let tags = getDefaultTags("SERIES", query);
  const tparams = getParams(params, tags, query);

  const offset = query?.offset ? parseInt(query.offset, 10) : 0;
  const limit = query?.limit ? parseInt(query.limit, 10) : 200;
  if (limit) tparams["99990C01"] = limit;
  if (offset) tparams["99990C02"] = offset;
  tparams["99990C00"] = "SeriesDate"; // sort series date  tag 00080021
  params = JSON.stringify(tparams);
  const dicomweb = true; // indicate json format
  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);series([[${CQAE}]],[[${params}]],[[${dicomweb}]])`;
  try {
    const os = new os_func_spawn();
    const dados = await os.execCommand(cmd);
    try {
      temp = JSON.parse(dados);
    } catch (error) {
      console.log(error);
      temp = [];
    }
    res.set("Content-Type", "application/json");
    res.status(temp.length > 0 ? 200 : 204).send(temp);
  } catch (error) {
    console.log(error);
    return res.status(400).send({
      message: "Error finding series",
    });
  }
});

/*-------------------------QIDO API--------------------------------
 Study's Series' Instances
/studies/{study}/series/{series}/instances{?search*}
------------------------------------------------------------------*/
router.get("/rs/studies/:study/series/:series/instances", async (req, res) => {
  const query = url.parse(req.url, true).query;
  const { study, series } = req.params;
  if (!study || !series) {
    return res.status(400).send("Invalid study or series.");
  }
  let params = {
    QueryRetrieveLevel: "IMAGE",
    StudyInstanceUID: study,
    SeriesInstanceUID: series,
  };
  let tags = getDefaultTags("IMAGE", query);
  const tparams = getParams(params, tags, query);

  const offset = query?.offset ? parseInt(query.offset, 10) : 0;
  const limit = query?.limit ? parseInt(query.limit, 10) : 200;
  if (limit) tparams["99990C01"] = limit;
  if (offset) tparams["99990C02"] = offset;
  tparams["99990C00"] = "ImageNumber"; // sort image number tag 00200013
  params = JSON.stringify(tparams);

  const dicomweb = true; // indicate json format
  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);images([[${CQAE}]],[[${params}]],[[${dicomweb}]])`;
  try {
    const os = new os_func_spawn();
    const dados = await os.execCommand(cmd);
    let temp = JSON.parse(dados);
    res.set("Content-Type", "application/json");
    res.status(temp.length > 0 ? 200 : 204).send(temp);
  } catch (error) {
    console.log(error);
    return res.status(400).send({
      message: "Error finding instances.",
    });
  }
});

/*-------------------------QIDO API--------------------------------
 get a frame
------------------------------------------------------------------*/
router.get(
  "/rs/studies/:study/series/:series/instances/:instance/frames/:frame",
  async (req, res) => {
    const { study, series, instance, frame } = req.params;
    if (!study || !series || !instance || !frame) {
      return res
        .status(400)
        .send("Invalid study or series or instance or frame.");
    }
    const boundary =
      "simpleconquestqidobridge" + crypto.randomBytes(16).toString("hex");
    const cmd = `--dolua:dofile([[./queryfunctions.lua]]);getframe([[${CQAE}]],[[${boundary}]],[[${study}]], [[${series}]], [[${instance}]],[[${frame}]])`;
    exec(
      `${dgateExBin}  "${cmd}" `,
      { encoding: "binary", maxBuffer: MAX_BUFFER, cwd: APIFOLDER },
      function (error, stdout, stderr) {
        if (stderr) {
          console.log(stderr);
          return res.status(400).send({
            message: "Error finding frame.",
          });
        } else {
          const contentType = `multipart/related; boundary=${boundary}`;
          res.set("Content-Type", contentType);
          res.end(Buffer.from(stdout, "binary"));
        }
      }
    );
  }
);

/*-------------------------QIDO API METADATA-----------------------
Study Metadata
/studies/{study}/metadata
------------------------------------------------------------------*/
router.get("/rs/studies/:study/metadata", async (req, res) => {
  const { study } = req.params;
  if (!study) {
    return res.status(400).send("Invalid study.");
  }
  const level = "STUDY";
  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);metadata([[${CQAE}]],[[${level}]],[[${study}]],[[]],[[]])`;
  try {
    const os = new os_func_spawn();
    const dados = await os.execCommand(cmd);
    // fs.writeFileSync("../json.txt", dados, "utf8");
    try {
      temp = JSON.parse(dados);
    } catch (error) {
      console.log(error);
      temp = [];
    }
    res.set("Content-Type", "application/json");
    res.status(temp.length > 0 ? 200 : 204).send(temp);
  } catch (error) {
    console.log(error);
    return res.status(400).send({
      message: "Error finding study metadata",
    });
  }
});

/*-------------------------QIDO API METADATA------------------------
Series Metadata
/studies/{study}/series/{series}/metadata
------------------------------------------------------------------*/
router.get("/rs/studies/:study/series/:series/metadata", async (req, res) => {
  const { study, series } = req.params;
  if (!study || !series) {
    return res.status(400).send("Invalid study or series.");
  }
  const level = "SERIES";
  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);metadata([[${CQAE}]],[[${level}]],[[${study}]],[[${series}]],[[]])`;
  try {
    const os = new os_func_spawn();
    const dados = await os.execCommand(cmd);
    try {
      temp = JSON.parse(dados);
    } catch (error) {
      console.log(error);
      temp = [];
    }
    res.set("Content-Type", "application/json");
    res.status(temp.length > 0 ? 200 : 204).send(temp);
  } catch (error) {
    console.log(error);
    return res.status(400).send({
      message: "Error finding series metadata",
    });
  }
});

/*-------------------------QIDO API METADATA-----------------------
Instance Metadata
/studies/{study}/series/{series}/instances/{instance}/metadata
------------------------------------------------------------------*/
router.get(
  "/rs/studies/:study/series/:series/instances/:instance/metadata",
  async (req, res) => {
    const { study, series, instance } = req.params;
    if (!study || !series || !instance) {
      return res.status(400).send("Invalid study or series or instance.");
    }
    const level = "IMAGE";
    const cmd = `--dolua:dofile([[./queryfunctions.lua]]);metadata([[${CQAE}]],[[${level}]],[[${study}]],[[${series}]],[[${instance}]])`;
    try {
      const os = new os_func_spawn();
      const dados = await os.execCommand(cmd);
      try {
        temp = JSON.parse(dados);
      } catch (error) {
        console.log(error);
        temp = [];
      }
      res.set("Content-Type", "application/json");
      res.status(temp.length > 0 ? 200 : 204).send(temp);
    } catch (error) {
      console.log(error);
      return res.status(400).send({
        message: "Error finding instance metadata",
      });
    }
  }
);

/*-------------------------DICOM objects (Thumbnail)--------------------------------
Study Thumbnail
/studies/{study}/thumbnail
-----------------------------------------------------------------------------------------*/
router.get("/rs/studies/:study/thumbnail", async (req, res) => {
  const { study } = req.params;
  if (!study) {
    return res.status(400).send("Invalid study");
  }
  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);thumbs([[${CQAE}]],[[${study}]],[[]],[[]],[[]])`;
  exec(
    `${dgateExBin}  "${cmd}" `,
    { encoding: "binary", maxBuffer: MAX_BUFFER, cwd: APIFOLDER },
    function (error, stdout, stderr) {
      if (stderr) {
        console.log("erro", stderr);
        return res.status(400).send({
          message: "Error finding study thumbnail.",
        });
      } else {
        res.set("Content-Type", "image/jpeg"); // contentType
        res.end(Buffer.from(stdout, "binary"));
      }
    }
  );
});

/*-------------------------DICOM objects (Thumbnail)--------------------------------
Series Thumbnail
/studies/{study}/series/{series}/thumbnail
http://localhost:3002/api/dicom/rs/studies/2.16.840.1.113669.632.20.121711.10000161048/1.3.46.670589.11.17521.5.0.3124.2008081915302126599/thumbnail
-----------------------------------------------------------------------------------------*/
router.get("/rs/studies/:study/series/:series/thumbnail", async (req, res) => {
  const { study, series } = req.params;
  if (!study || !series) {
    return res.status(400).send("Invalid study or series.");
  }
  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);thumbs([[${CQAE}]],[[${study}]],[[${series}]],[[]],[[]])`;
  exec(
    `${dgateExBin}  "${cmd}" `,
    { encoding: "binary", maxBuffer: MAX_BUFFER, cwd: APIFOLDER },
    function (error, stdout, stderr) {
      if (stderr) {
        console.log("erro", stderr);
        return res.status(400).send({
          message: "Error finding series thumbnail.",
        });
      } else {
        res.set("Content-Type", "image/jpeg"); // contentType
        res.end(Buffer.from(stdout, "binary"));
      }
    }
  );
});

/*-------------------------DICOM objects (Thumbnail)--------------------------------
Instance Thumbnail
/studies/{study}/series/{series}/instances/{instance}/thumbnail
// http://localhost:3002/api/dicom/rs/studies/1.3.46.670589.5.2.10.2156913941.892665384.993397/series/1.3.46.670589.5.2.10.2156913941.892665339.860724/thumbnail?viewport=256%2C256
-----------------------------------------------------------------------------------------*/

router.get(
  "/rs/studies/:study/series/:series/instances/:instance/thumbnail",
  async (req, res) => {
    // const query = url.parse(req.url, true).query || {};
    const { study, series, instance } = req.params;
    if (!study || !series || !instance) {
      return res.status(400).send("Invalid study or series or instance.");
    }
    const cmd = `--dolua:dofile([[./queryfunctions.lua]]);thumbs([[${CQAE}]],[[${study}]],[[${series}]],[[${instance}]],[[]])`;
    exec(
      `${dgateExBin}  "${cmd}" `,
      { encoding: "binary", maxBuffer: MAX_BUFFER, cwd: APIFOLDER },
      function (error, stdout, stderr) {
        if (stderr) {
          console.log("erro", stderr);
          return res.status(400).send({
            message: "Error finding instance thumbnail.",
          });
        } else {
          res.set("Content-Type", "image/jpeg"); // contentType
          res.end(Buffer.from(stdout, "binary"));
        }
      }
    );
  }
);

/*-------------------------DICOM objects (Thumbnail)--------------------------------
Frame Thumbnail
/studies/{study}/series/{series}/instances/{instance}/frames/{frames}/thumbnail
// http://localhost:3002/api/dicom/rs/studies/1.3.46.670589.5.2.10.2156913941.892665384.993397/series/1.3.46.670589.5.2.10.2156913941.892665339.860724/thumbnail?viewport=256%2C256
-----------------------------------------------------------------------------------------*/

router.get(
  "/rs/studies/:study/series/:series/instances/:instance/frames/:frames/thumbnail",
  async (req, res) => {
    // const query = url.parse(req.url, true).query || {};
    const { study, series, instance, frames } = req.params;
    if (!study || !series || !instance || !frames) {
      return res
        .status(400)
        .send("Invalid study or series or instance or frame.");
    }
    const cmd = `--dolua:dofile([[./queryfunctions.lua]]);thumbs([[${CQAE}]],[[${study}]],[[${series}]],[[${instance}]],[[${frames}]])`;
    exec(
      `${dgateExBin}  "${cmd}" `,
      { encoding: "binary", maxBuffer: MAX_BUFFER, cwd: APIFOLDER },
      function (error, stdout, stderr) {
        if (stderr) {
          console.log("erro", stderr);
          return res.status(400).send({
            message: "Error finding frame thumbnail.",
          });
        } else {
          res.set("Content-Type", "image/jpeg"); // contentType
          res.end(Buffer.from(stdout, "binary"));
        }
      }
    );
  }
);

/*-------------------------Retrieve DICOM objects (WADO-RS)--------------------------------
/studies/{study}
-----------------------------------------------------------------------------------------*/
router.get("/rs/studies/:study", async (req, res) => {
  const { study } = req.params;
  if (!study) {
    return res.status(400).send("Invalid study.");
  }
  const query = url.parse(req.url, true).query;
  let params = { QueryRetrieveLevel: "SERIES", StudyInstanceUID: study };
  const tparams = getParams(params, tags, query);
  const offset = query?.offset ? parseInt(query.offset, 10) : 0;
  const limit = query?.limit ? parseInt(query.limit, 10) : 200;
  if (limit) tparams["99990C01"] = limit;
  if (offset) tparams["99990C02"] = offset;
  tparams["99990C00"] = "SeriesDate"; // sort series tag 00080020
  params = JSON.stringify(tparams);

  const dicomweb = true; // indicate json format
  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);series([[${CQAE}]],[[${params}]],[[${dicomweb}]])`;
  try {
    let temp = [];
    const os = new os_func_spawn();
    const dados = await os.execCommand(cmd);
    temp = JSON.parse(dados);
    res.set("Content-Type", "application/json");
    res.status(temp.length > 0 ? 200 : 204).send(temp);
  } catch (error) {
    console.log(error);
    return res.status(400).send({
      message: "Error finding studies",
    });
  }
});

/*-------------------------Retrieve DICOM objects (WADO-RS)--------------------------------
/studies/{study}/series/{series}
-----------------------------------------------------------------------------------------*/
router.get("/rs/studies/:study/series/:series", async (req, res) => {
  const query = url.parse(req.url, true).query;
  const { study, series } = req.params;
  if (!study || !series) {
    return res.status(400).send("Invalid study or series.");
  }
  let params = {
    QueryRetrieveLevel: "IMAGE",
    StudyInstanceUID: study,
    SeriesInstanceUID: series,
    SOPInstanceUID: "",
  };
  let tags = getDefaultTags("IMAGE", query);
  const tparams = getParams(params, tags, query);
  const offset = query?.offset ? parseInt(query.offset, 10) : 0;
  const limit = query?.limit ? parseInt(query.limit, 10) : 200;
  if (limit) tparams["99990C01"] = limit;
  if (offset) tparams["99990C02"] = offset;
  tparams["99990C00"] = "ImageNumber";
  params = JSON.stringify(tparams);

  const dicomweb = true; // indicate json format
  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);images([[${CQAE}]],[[${params}]],[[${dicomweb}]])`;
  try {
    const os = new os_func_spawn();
    const dados = await os.execCommand(cmd);
    let temp = JSON.parse(dados);
    res.set("Content-Type", "application/json");
    res.status(temp.length > 0 ? 200 : 204).send(temp);
  } catch (error) {
    console.log(error);
    return res.status(400).send({
      message: "Error finding images.",
    });
  }
});

/*-------------------------Retrieve DICOM objects (WADO-RS)--------------------------------
/studies/{study}/series/{series}/instances/{instance}
-----------------------------------------------------------------------------------------*/
router.get(
  "/rs/studies/:study/series/:series/instances/:instance",
  async (req, res) => {
    const { study, series, instance } = req.params;
    if (!study || !series || !instance) {
      return res
        .status(400)
        .send("Invalid study or series or instance or frame.");
    }
    const boundary =
      "simpleconquestqidobridge" + crypto.randomBytes(16).toString("hex");
    const cmd = `--dolua:dofile([[./queryfunctions.lua]]);getinstance([[${CQAE}]],[[${boundary}]],[[${study}]], [[${series}]], [[${instance}]])`;
    exec(
      `${dgateExBin}  "${cmd}" `,
      { encoding: "binary", maxBuffer: MAX_BUFFER, cwd: APIFOLDER },
      function (error, stdout, stderr) {
        if (stderr) {
          console.log(stderr);
          return res.status(400).send({
            message: "Error finding instance.",
          });
        } else {
          const contentType = `multipart/related; boundary=${boundary}`;
          res.set("Content-Type", contentType);
          res.end(Buffer.from(stdout, "binary"));
        }
      }
    );
  }
);

/*---------------------------WADO URI-----------------------------
  WADO URI  GET
------------------------------------------------------------------*/
router.get("/wadouri", async (req, res) => {
  const query = url.parse(req.url, true).query;
  const {
    requestType,
    contentType = "image/jpeg",
    studyUID,
    seriesUID,
    objectUID,
  } = query;
  if (requestType !== "WADO") {
    return res.status(400).send({
      message: "Invalid Request Type",
    });
  }
  if (!studyUID || !seriesUID || !objectUID) {
    return res.status(400).send("Invalid studyUID, serieUID or objectUID.");
  }
  const params = new URLSearchParams(query);
  let str = params.toString();
  const quote = isWin() ? '""' : '\\"';
  str = str.replace(/"/g, quote);
  const cmd = `--wadoparse:${str}`;
  exec(
    `${dgateExBin}  "${cmd}" `,
    { encoding: "binary", maxBuffer: MAX_BUFFER, cwd: APIFOLDER },
    function (error, stdout, stderr) {
      if (stderr) {
        console.log("erro", stderr);
        return res.status(400).send({
          message: "Error finding slice.",
        });
      } else {
        const i = stdout.indexOf("\n\n");
        const s = stdout.substring(i + 2);
        res.set("Content-Type", contentType);
        res.end(Buffer.from(s, "binary"));
      }
    }
  );
});

// second option
router.get("/wadouri_alt", async (req, res) => {
  const query = url.parse(req.url, true).query;
  const {
    studyUID,
    seriesUID,
    objectUID,
    requestType,
    contentType = "image/jpeg",
    rows = "0",
    columns = "0",
    region = "",
    windowCenter = "0",
    windowWidth = "0",
    frameNumber = "0",
    transferSyntax = "",
    anonymize = "",
    annotation = "",
    imageQuality = 100,
    charset = "",
    presentationUID = "",
  } = query;
  if (requestType !== "WADO") {
    return res.status(400).send({
      message: "Invalid Request Type",
    });
  }
  if (!studyUID || !seriesUID || !objectUID) {
    return res.status(400).send("Invalid studyUID, serieUID or objectUID.");
  }
  t =
    "{ studyUID=" +
    `'${studyUID}',` +
    "seriesUID=" +
    `'${seriesUID}',` +
    "objectUID=" +
    `'${objectUID}',` +
    "requestType=" +
    `'${requestType}',` +
    "contentType=" +
    `'${contentType}',` +
    "rows=" +
    `'${rows}',` +
    "columns=" +
    `'${columns}',` +
    "region=" +
    `'${region}',` +
    "windowCenter=" +
    `'${windowCenter}',` +
    "windowWidth=" +
    `'${windowWidth}',` +
    "frameNumber=" +
    `'${frameNumber}',` +
    "transferSyntax=" +
    `'${transferSyntax}',` +
    "anonymize=" +
    `'${anonymize}',` +
    "annotation=" +
    `'${annotation}',` +
    "imageQuality=" +
    `'${imageQuality}',` +
    "charset=" +
    `'${charset}',` +
    "presentationUID=" +
    `'${presentationUID}'` +
    "}";
  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);wadouri([[${t}]])`;

  exec(
    `${dgateExBin}  "${cmd}" `,
    { encoding: "binary", maxBuffer: MAX_BUFFER, cwd: APIFOLDER },
    function (error, stdout, stderr) {
      if (stderr) {
        console.log("erro", stderr);
        return res.status(400).send({
          message: "Error finding slice.",
        });
      } else {
        // const nulbuf = new Uint8Array([0x00]);
        // const newBuffer = Buffer.concat([
        //   Buffer.from(stdout, "binary"),
        //   nulbuf,
        // ]);
        res.set("Content-Type", contentType); // contentType
        res.end(Buffer.from(stdout, "binary"));
      }
    }
  );
});

/*---------------------------WADO CGI-----------------------------
  WADO CGI  GET
------------------------------------------------------------------*/
router.get("/wadocgi", async (req, res) => {
  const query = url.parse(req.url, true).query;

  const {
    studyUID,
    seriesUID,
    objectUID,
    requestType,
    contentType = "image/jpeg",
    rows = "0",
    columns = "0",
    region = "",
    windowCenter = "0",
    windowWidth = "0",
    frameNumber = "0",
    transferSyntax = "",
    anonymize = "",
    annotation = "",
    imageQuality = 100,
    charset = "",
    presentationUID = "",
  } = query;

  if (requestType !== "WADO") {
    return res.status(400).send({
      message: "Invalid Request Type",
    });
  }
  if (!studyUID || !seriesUID || !objectUID) {
    return res.status(400).send("Invalid studyUID, serieUID or objectUID.");
  }
  t =
    "{ studyUID=" +
    `'${studyUID}',` +
    "seriesUID=" +
    `'${seriesUID}',` +
    "objectUID=" +
    `'${objectUID}',` +
    "requestType=" +
    `'${requestType}',` +
    "contentType=" +
    `'${contentType}',` +
    "rows=" +
    `'${rows}',` +
    "columns=" +
    `'${columns}',` +
    "region=" +
    `'${region}',` +
    "windowCenter=" +
    `'${windowCenter}',` +
    "windowWidth=" +
    `'${windowWidth}',` +
    "frameNumber=" +
    `'${frameNumber}',` +
    "transferSyntax=" +
    `'${transferSyntax}',` +
    "anonymize=" +
    `'${anonymize}',` +
    "annotation=" +
    `'${annotation}',` +
    dicomi;
  "imageQuality=" +
    `'${imageQuality}',` +
    "charset=" +
    `'${charset}',` +
    "presentationUID=" +
    `'${presentationUID}'` +
    "}";
  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);wadocgi([[${t}]])`;
  exec(
    `${dgateExBin}  "${cmd}" `,
    { encoding: "binary", maxBuffer: MAX_BUFFER, cwd: APIFOLDER },
    function (error, stdout, stderr) {
      if (stderr) {
        console.log("erro", stderr);
        return res.status(400).send({
          message: "Error finding slice.",
        });
      } else {
        const i = stdout.indexOf("\n\n");
        const s = stdout.substring(i + 2);
        // fs.writeFileSync("./image.gif", s, { encoding: "binary" });
        res.set("Content-Type", contentType);
        res.end(Buffer.from(s, "binary"));
      }
    }
  );
});

/*---------------------Custom API-----------------------------------
   get a dicom image
   slice format is  PatientID:SOPInstanceUID
------------------------------------------------------------------*/
router.get("/dicomSlice/:slice", async (req, res) => {
  const slice = req.params.slice;
  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);readslice([[${slice}]])`;
  if (!slice) {
    return res.status(400).send({
      message: "Invalid slice",
    });
  }
  exec(
    `${dgateExBin}  "${cmd}" `,
    { encoding: "binary", maxBuffer: MAX_BUFFER, cwd: APIFOLDER },
    function (error, stdout, stderr) {
      if (stderr) {
        console.log(stderr);
        return res.status(400).send({
          message: "Houve um erro buscando slice.",
        });
      } else {
        res.set("Content-Type", "application/dicom");
        res.end(Buffer.from(stdout, "binary"));
      }
    }
  );
});

/*---------------------Custom API-----------------------------------
   get a slice as bmp,gif or jpeg
   params: ghrahic: bmp, jpg, dicom or gif image
   slice format is  PatientID:SOPInstanceUID
------------------------------------------------------------------*/
router.get("/dicomConverter/:graphic/:size/:slice", async (req, res) => {
  let graph;
  const { graphic = "jpg", size = 128, slice = "" } = req.params;
  if (graphic == "jpeg" || graphic == "jpg") {
    graph = "jpg";
  }
  // ("j3","j4","j5","j1","j2","j1","js","j7","jk",'jl","un")
  if (!["bmp", "gif", "jpg"].includes(graph)) {
    return res.status(400).send({ message: "Invalid file type." });
  }
  if (!slice) {
    return res.status(400).send({ message: "Invalid slice." });
  }

  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);converteslice([[${graph}]],[[${size}]],[[${slice}]])`;
  exec(
    `${dgateExBin}  "${cmd}" `,
    { encoding: "binary", maxBuffer: MAX_BUFFER, cwd: APIFOLDER },
    function (error, stdout, stderr) {
      if (stderr) {
        console.log(stderr);
        return res.status(400).send({
          message: "Error finding slice.",
        });
      } else {
        let tempMime;
        if (["jpeg", "jpg"].includes(graph)) {
          tempMime = "image/jpeg";
        } else tempMime = `image/${graph}`;
        res.set("Content-Type", tempMime);
        return res.end(Buffer.from(stdout, "binary"));
      }
    }
  );
});

/*---------------------Custom API-----------------------------------
   get Patients
------------------------------------------------------------------*/
router.post("/dicomPatients", async (req, res) => {
  let temp;
  const { PatientBirthDate, PatientSex, PatientID, PatientName } = req.body;
  const params = JSON.stringify({
    QueryRetrieveLevel: "PATIENT",
    PatientID: PatientID || "",
    PatientName: PatientName || "",
    PatientSex: PatientSex || "",
    PatientBirthDate: PatientBirthDate || "",
  });
  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);patients([[${CQAE}]],[[${params}]])`;

  try {
    const os = new os_func_spawn();
    const dados = await os.execCommand(cmd);
    try {
      temp = JSON.parse(dados);
    } catch (error) {
      temp = [];
    }
    res.send(temp);
  } catch (error) {
    console.log(error);
    return res.status(400).send({
      message: "Error finding patients",
    });
  }
});

/*---------------------Custom API-----------------------------------
   get Studies
------------------------------------------------------------------*/
router.post("/dicomStudies", async (req, res) => {
  let temp;
  const {
    PatientBirthDate = "",
    PatientID = "",
    StudyInstanceUID = "",
    PatientName = "",
    StudyDate = "",
    StudyTime = "",
  } = req.body;
  const params = JSON.stringify({
    QueryRetrieveLevel: "STUDY",
    StudyInstanceUID: StudyInstanceUID || "",
    PatientID: PatientID || "",
    PatientName: PatientName || "",
    PatientBirthDate: PatientBirthDate || "",
    StudyDate: StudyDate || "", // '19000101-20221231'
    StudyTime: StudyTime,
  });
  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);studies([[${CQAE}]],[[${params}]])`;
  try {
    const os = new os_func_spawn();
    const dados = await os.execCommand(cmd);
    try {
      temp = JSON.parse(dados);
      if (temp && temp.length > 0) {
        temp.sort((a, b) => {
          if (a.StudyDate == b.StudyDate) return 0;
          if (a.StudyDate < b.StudyDate) return -1;
          if (a.StudyDate > b.StudyDate) return 1;
        });
      }
    } catch (error) {
      temp = [];
    }
    res.send(temp);
  } catch (error) {
    console.log(error);
    return res.status(400).send({
      message: "Error finding studies",
    });
  }
});

/*---------------------Custom API-----------------------------------
   get Series
------------------------------------------------------------------*/

router.post("/dicomSeries", async (req, res) => {
  let temp;
  const { PatientID, StudyInstanceUID } = req.body;
  const params = JSON.stringify({
    PatientID: PatientID,
    SeriesInstanceUID: "",
    StudyInstanceUID: StudyInstanceUID || "",
    Modality: "",
    SeriesDescription: "",
    QueryRetrieveLevel: "SERIES",
    SeriesDate: "",
    SeriesTime: "",
  });
  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);series([[${CQAE}]],[[${params}]])`;
  try {
    const os = new os_func_spawn();
    const dados = await os.execCommand(cmd);
    try {
      temp = JSON.parse(dados);
      if (temp && temp.length > 0) {
        temp.sort((a, b) => {
          if (a.SeriesDate == b.SeriesDate) return 0;
          if (a.SeriesDate < b.SeriesDate) return -1;
          if (a.SeriesDate > b.SeriesDate) return 1;
        });
      }
    } catch (error) {
      temp = [];
    }
    res.send(temp);
  } catch (error) {
    console.log(error);
    return res.status(400).send({
      message: "Error finding series",
    });
  }
});

/*---------------------Custom API-----------------------------------
   get Images
------------------------------------------------------------------*/
router.post("/dicomImages", async (req, res) => {
  const { PatientID, SeriesInstanceUID } = req.body;
  const params = JSON.stringify({
    ConquestDevice: "",
    ConquestObjectFile: "",
    ConquestImageDate: "",
    ConquestImageTime: "",
    PatientID: PatientID || "",
    PatientName: "",
    SeriesInstanceUID: SeriesInstanceUID || "",
    InstanceNumber: "",
    ImageDate: "",
    ImageTime: "",
    StudyInstanceUID: "",
    SOPInstanceUID: "",
    NumberOfFrames: "",
  });
  const cmd = `--dolua:dofile([[./queryfunctions.lua]]);images([[${CQAE}]],[[${params}]])`;
  try {
    const os = new os_func_spawn();
    const dados = await os.execCommand(cmd);
    const fson = JSON.parse(dados);
    if (fson && fson.length > 0) {
      fson.sort((a, b) => {
        if (+a.InstanceNumber == +b.InstanceNumber) return 0;
        if (+a.InstanceNumber < +b.InstanceNumber) return -1;
        if (+a.InstanceNumber > +b.InstanceNumber) return 1;
      });
    }
    const resp = fson.map((el, index) => {
      let filename = "";
      if (el.ConquestObjectFile) {
        filename = path.parse(el.ConquestObjectFile).base;
      }
      return { ...el, filename, id: index + 1 };
    });
    res.send(resp);
  } catch (error) {
    console.log(error);
    return res.status(400).send({
      message: "Error finding images.",
    });
  }
});

module.exports = router;
