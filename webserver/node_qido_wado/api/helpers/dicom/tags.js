const { QUERY_LEVEL } = require("./querylevel");
const studyLevelTags = [
  "00080005",
  "00080020",
  "00080030",
  "00080050",
  "00080054",
  "00080056",
  "00080061",
  "00080090",
  "00081190",
  "00080201",
  "00100010",
  "00100020",
  "00100030",
  "00100040",
  "0020000D",
  "00200010",
  "00201206",
  "00201208",
];

const seriesLevelTags = [
  "00080005",
  "00080054",
  "00080056",
  "00080060",
  "0008103E",
  "00081190",
  "0020000E",
  "00200011",
  "00201209",
  "00080201",
  "00400244",
  "00400245",
  "00400275",
  "00400009",
  "00401001",
];

const imageLevelTags = [
  "00080016",
  "00080018",
  "00080056",
  "00080201",
  "00081190",
  "00200013",
  "00280010",
  "00280011",
  "00280100",
  "00280008",
];

module.exports = {
  tagsForLevel(level) {
    switch (level) {
      case QUERY_LEVEL.STUDY:
        return studyLevelTags;
      case QUERY_LEVEL.SERIES:
        return seriesLevelTags;
      case QUERY_LEVEL.IMAGE:
        return imageLevelTags;
      default:
        return studyLevelTags;
    }
  },
};
