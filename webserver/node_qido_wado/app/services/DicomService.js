import qs from 'qs'
export default ($axios) => ({
  dicomPatients(obj) {
    return $axios.post(`dicom/dicomPatients`, obj)
  },
  dicomStudies(obj) {
    return $axios.post(`dicom/dicomStudies`, obj)
  },
  dicomQIDOStudies(query) {
    return $axios.get(`dicom/rs/studies`, {
      params: query,

      paramsSerializer: (p) => {
        return qs.stringify(p, { arrayFormat: 'repeat' })
      },
    })
  },
  dicomQIDOSeries(obj) {
    return $axios.get(`dicom/rs/studies/${obj.StudyInstanceUID}/series`, {
      params: obj.query,
      paramsSerializer: (p) => {
        return qs.stringify(p, { arrayFormat: 'repeat' })
      },
    })
  },
  dicomQIDOImages(obj) {
    return $axios.get(
      `dicom/rs/studies/${obj.StudyInstanceUID}/series/${obj.SeriesInstanceUID}/instances`,
      {
        params: obj.query,
        paramsSerializer: (p) => {
          return qs.stringify(p, { arrayFormat: 'repeat' })
        },
      }
    )
  },
  dicomSeries(obj) {
    return $axios.post(`dicom/dicomSeries`, obj)
  },
  dicomSlice(slice) {
    return $axios.get(`dicom/dicomSlice/${slice}`)
  },
  dicomConverter(graphic, size, slice) {
    return $axios.get(`dicom/dicomConverter/${graphic}/${size}/${slice}`)
  },
  dicomImages(obj) {
    return $axios.post(`dicom/dicomImages`, obj)
  },
  dicomwado(query) {
    return $axios.get(`dicom/wadouri?${query}`)
  },
})
