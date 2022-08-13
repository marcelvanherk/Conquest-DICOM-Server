import DicomService from '~/services/DicomService'
export default ({ $axios }, inject) => {
  inject('DicomService', DicomService($axios))
}
