const STUDY_LEVEL = "STUDY";
const SERIES_LEVEL = "SERIES";
const IMAGE_LEVEL = "IMAGE";

module.exports = {
  STUDY_LEVEL: STUDY_LEVEL,
  SERIES_LEVEL: SERIES_LEVEL,
  IMAGE_LEVEL: IMAGE_LEVEL,
  QUERY_LEVEL: {
    STUDY: STUDY_LEVEL,
    SERIES: SERIES_LEVEL,
    IMAGE: IMAGE_LEVEL,
  },

  queryLevelToPath(studyUid, seriesUid, imageUid, qlevel) {
    switch (qlevel) {
      case STUDY_LEVEL:
        return studyUid;
      case SERIES_LEVEL:
        return `${studyUid}/${seriesUid}`;
      case IMAGE_LEVEL:
        return `${studyUid}/${seriesUid}/${imageUid}`;
      default:
        return `${studyUid}/${seriesUid}`;
    }
  },
};
