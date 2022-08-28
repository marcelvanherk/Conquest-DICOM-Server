<template>
  <v-container fluid>
    <lazy-wait-dialog :d-wait="dialogWait" />
    <v-tabs v-model="tab" background-color="primary">
      <v-tab>Search</v-tab>
      <v-tab @click.stop="resizeCorn()">Corn </v-tab>
    </v-tabs>
    <v-tabs-items v-model="tab">
      <v-tab-item>
        <v-card>
          <v-card-text class="mt-2">
            <v-row>
              <v-col sm="9">
                <v-row>
                  <v-col cols="12" sm="5">
                    <v-text-field
                      v-model="estudo.di"
                      label="Range from"
                      type="date"
                      max="2999-12-31"
                      hide-details="auto"
                      clearable
                    ></v-text-field>
                  </v-col>
                  <v-col cols="12" sm="5">
                    <v-text-field
                      v-model="estudo.df"
                      label="to"
                      type="date"
                      max="2999-12-31"
                      hide-details="auto"
                      clearable
                    ></v-text-field>
                  </v-col>
                  <v-checkbox
                    v-model="estudo.filtra"
                    label="Filter"
                    :false-value="0"
                    :true-value="1"
                    hide-details=""
                  ></v-checkbox>
                </v-row>
                <v-row>
                  <v-col cols="12" sm="4">
                    <v-text-field
                      v-model="paciente.pacid"
                      label="Patient Id"
                      hide-details="auto"
                      clearable
                      @keyup.enter="searchStudies"
                      @input="paciente.pacnome = ''"
                    ></v-text-field>
                  </v-col>
                  <v-col cols="12" sm="8">
                    <v-text-field
                      v-model="paciente.pacnome"
                      label="Patient Name"
                      hide-details="auto"
                      clearable
                      @keyup.enter="searchStudies"
                      @input="paciente.pacid = ''"
                    ></v-text-field>
                  </v-col>
                </v-row>
              </v-col>

              <v-col sm="3" class="d-flex justify-center align-center">
                <v-btn
                  color="info"
                  title="Buscar"
                  hide-details="auto"
                  @click.stop.prevent="searchStudies"
                >
                  <v-icon>mdi-magnify</v-icon>Run
                </v-btn>
              </v-col>
            </v-row>
          </v-card-text>
          <v-card-text>
            <v-card-title
              class="d-flex indigo white--text py-1 justify-space-between mb-4"
            >
              <v-col class="py-0">Studies</v-col>
            </v-card-title>

            <v-row>
              <v-col cols="12">
                <v-data-table
                  v-model="modelStudy"
                  single-select
                  dense
                  :headers="headersStudies"
                  :items="studies"
                  item-key="StudyInstanceUID"
                  class="elevation-1 marcarguias dstudies"
                  disable-pagination
                  hide-default-footer
                  @click:row="rowClickStudy"
                >
                  <template #[`item.StudyDate`]="{ item }">
                    {{
                      item.StudyDate
                        ? $moment(item.StudyDate, 'YYYYMMDD').format(
                            'DD-MM-YYYY'
                          )
                        : ''
                    }}
                  </template>
                  <template #[`item.PatientBirthDate`]="{ item }">
                    {{
                      item.PatientBirthDate
                        ? $moment(item.PatientBirthDate, 'YYYYMMDD').format(
                            'DD-MM-YYYY'
                          )
                        : ''
                    }}
                  </template>
                </v-data-table>
              </v-col>
            </v-row>
            <v-card-title
              class="
                d-flex
                indigo
                white--text
                py-1
                justify-space-between
                mt-4
                mb-4
              "
            >
              <v-col class="py-0">Series</v-col>
            </v-card-title>

            <v-row>
              <v-col cols="12">
                <v-data-table
                  v-model="modelSerie"
                  single-select
                  dense
                  :headers="headersSeries"
                  :items="series"
                  disable-pagination
                  hide-default-footer
                  item-key="SeriesInstanceUID"
                  class="elevation-1 marcarguias dseries"
                  @click:row="rowClickSerie"
                >
                  <template #[`item.SeriesTime`]="{ item }">
                    {{
                      item.SeriesTime
                        ? $moment({})
                            .seconds(item.SeriesTime)
                            .format('HH:mm:ss')
                        : ''
                    }}
                  </template>
                  <template #[`item.SeriesDate`]="{ item }">
                    {{
                      item.SeriesDate
                        ? $moment(item.SeriesDate, 'YYYYMMDD').format(
                            'DD-MM-YYYY'
                          )
                        : ''
                    }}
                  </template>
                </v-data-table>
              </v-col>
            </v-row>
            <v-card-title
              class="
                d-flex
                indigo
                white--text
                py-1
                justify-space-between
                mt-4
                mb-4
              "
            >
              <v-col class="py-0">Images</v-col>
            </v-card-title>

            <v-row>
              <v-col cols="12">
                <v-data-table
                  v-model="modelImage"
                  single-select
                  dense
                  :headers="headersImage"
                  :items="images"
                  item-key="InstanceNumber"
                  class="elevation-1 marcarguias dimage"
                  disable-pagination
                  hide-default-footer
                  @click:row="rowClickImage"
                >
                </v-data-table>
              </v-col>
            </v-row>
            <v-row justify="space-around">
              <v-col
                v-for="item in images"
                :key="item.SOPInstanceUID"
                cols="12"
                sm="3"
                md="2"
                lg="2"
              >
                <v-card class="mx-auto my-2 pa-2" outlined elevation="3">
                  <div class="text-h7 mb-1">
                    <span
                      ><strong>{{ item.InstanceNumber }}</strong></span
                    >
                  </div>
                  <a
                    href="javascript:void(null)"
                    @click.stop="loadCornerstone(item)"
                  >
                    <v-img :src="getSrcImage(item)" aspect-ratio="1.7"></v-img>
                  </a>
                </v-card>
              </v-col>
            </v-row>
          </v-card-text>
        </v-card>
      </v-tab-item>
      <v-tab-item eager>
        <viewer-cornestone
          :key="keycorn"
          :main-image="mainImage"
          :thumbs-images="thumbsImg"
        />
      </v-tab-item>
    </v-tabs-items>
  </v-container>
</template>

<script>
import API_BASE_URL from '@/services/api/baseUrl'
export default {
  name: 'DicomViewer',
  data: (vm) => ({
    mainImage: null,
    keycorn: 0,
    thumbsImg: [],
    tab: null,
    params: [],
    loadingComplete: false,
    dialogWait: false,
    paciente: { pacid: '', pacnome: '' },
    estudo: {
      df: vm.$moment().format('YYYY-MM-DD'),
      di: vm.$moment().subtract('5', 'days').format('YYYY-MM-DD'),
      filtra: 1,
    },
    body: document.querySelector('body'),
    modelStudy: [],
    studies: [],
    headersStudies: [
      {
        text: 'Study Date',
        align: 'start',
        sortable: false,
        value: 'StudyDate',
      },
      { text: 'Pat Name', value: 'PatientName' },
      { text: 'Pat ID', value: 'PatientID' },
      { text: 'Study Inst UID', value: 'StudyInstanceUID' },
      { text: 'Birth Date', value: 'PatientBirthDate' },
    ],

    modelSerie: [],
    series: [],
    headersSeries: [
      { text: 'Pat ID', value: 'PatientID' },
      {
        text: 'Serie Date',
        value: 'SeriesDate',
      },
      { text: 'Serie Time', value: 'SeriesTime' },
      { text: 'Series Description', value: 'SeriesDescription' },
      { text: 'Serie Inst UID', value: 'SeriesInstanceUID' },
    ],

    modelImage: [],
    images: [],
    headersImage: [
      {
        text: 'Inst. Number',
        value: 'InstanceNumber',
      },
      {
        text: 'SOP Inst UID',
        value: 'SOPInstanceUID',
      },
      { text: 'Nr Frames', value: 'NumberOfFrames', align: 'center' },
      { text: 'Series UID', value: 'SeriesInstanceUID' },
    ],
  }),
  methods: {
    resizeCorn() {
      this.keycorn++
    },
    getSrcImage(item) {
      const api = `${API_BASE_URL(this.$config.baseURL)}/api/dicom/`
      const studyUID = item.StudyInstanceUID
      const seriesUID = item.SeriesInstanceUID
      const objectUID = item.SOPInstanceUID
      const requestType = 'WADO'
      const contentType = 'image/jpeg'
      const rows = '128'
      const columns = '128'
      const query = `requestType=${requestType}&studyUID=${studyUID}&seriesUID=${seriesUID}&objectUID=${objectUID}&contentType=${contentType}&rows=${rows}&columns=${columns}`
      const res = `${api}wadouri?${query}`
      return res
    },
    rowClickStudy(item, row) {
      row.select(true)
      try {
        this.modelSerie = []
        this.series = []
        this.images = []
        this.modelImage = []
        this.searchSeries(item)
      } catch (error) {
        console.log(error)
      }
    },
    rowClickSerie(item, row) {
      row.select(true)
      this.images = []
      this.modelImage = []
      try {
        this.searchImages(item)
      } catch (error) {
        console.log(error)
      }
    },
    getSrcMainImage(item) {
      const api = `${API_BASE_URL(this.$config.baseURL)}/api/dicom/`
      const studyUID = item.StudyInstanceUID
      const seriesUID = item.SeriesInstanceUID
      const objectUID = item.SOPInstanceUID
      const requestType = 'WADO'
      const contentType = 'application/dicom'
      const query = `requestType=${requestType}&studyUID=${studyUID}&seriesUID=${seriesUID}&objectUID=${objectUID}&contentType=${contentType}`
      const res = `${api}wadouri?${query}`
      return res
    },

    loadCornerstone(item) {
      this.mainImage = this.getSrcMainImage(item)
      this.thumbsImg = this.images.map((item) => {
        item.thumb = this.getSrcImage(item) // thumbnails src
        item.mainImage = this.getSrcMainImage(item)
        return item
      })
      this.tab = 1
    },
    rowClickImage(item, row) {
      this.loadingComplete = false
      if (row) row.select(true)
    },
    async searchStudies() {
      this.dialogWait = true
      this.modelStudy = []
      this.studies = []
      this.modelSerie = []
      this.series = []
      this.modelImage = []
      this.images = []
      try {
        let resp
        let rg = ''
        const ok = this.paciente?.pacnome || this.paciente?.pacid
        const odi =
          this.estudo.di && this.estudo.filtra == 1
            ? this.$moment(this.estudo.di).format('YYYYMMDD')
            : ''
        const odf =
          this.estudo.df && this.estudo.filtra == 1
            ? this.$moment(this.estudo.df).format('YYYYMMDD')
            : ''
        if (odi && odf) {
          rg = `${odi}-${odf}`
        }
        if (this.paciente.pacnome) {
          resp = await this.$DicomService.dicomQIDOStudies({
            PatientName: this.paciente.pacnome,
            StudyDate: rg,
          })
        } else if (this.paciente.pacid) {
          resp = await this.$DicomService.dicomQIDOStudies({
            PatientID: this.paciente.pacid,
            StudyDate: rg,
          })
        }
        if (!ok) {
          this.$dialog.notify.info('Please, select the options.', {
            position: 'top-right',
            timeout: 3000,
          })
          return
        }
        if (resp?.status == 204) {
          this.$dialog.notify.warning('Records no found.', {
            position: 'top-right',
            timeout: 3000,
          })
          return
        }
        const dados = resp.data
        this.studies = []
        dados.forEach((item) => {
          const values = {
            StudyDate: item['00080020']?.Value?.[0],
            PatientName: item['00100010']?.Value?.[0],
            PatientID: item['00100020']?.Value?.[0],
            StudyInstanceUID: item['0020000D']?.Value?.[0],
            PatientBirthDate: item['00100030']?.Value?.[0],
          }
          this.studies.push(values)
        })
        if (this.studies.length == 0) {
          this.$dialog.notify.info('Records no found.', {
            position: 'top-right',
            timeout: 3000,
          })
        }
      } catch (error) {
        console.log(error)
        await this.$dialog.error({
          text: error?.response?.data?.message || 'Image server not accesible!',
          title: 'Erro',
          persistent: false,
          actions: ['Fechar'],
        })
      } finally {
        this.dialogWait = false
      }
    },
    async searchSeries(item) {
      this.dialogWait = true
      this.modelSerie = []
      this.series = []
      this.modelImage = []
      this.images = []
      try {
        const resp = await this.$DicomService.dicomQIDOSeries({
          StudyInstanceUID: item.StudyInstanceUID,
          query: { SeriesDate: '', SeriesTime: '', PatientID: '' },
        })
        if (resp?.status == 204) {
          this.$dialog.notify.warning('Records no found.', {
            position: 'top-right',
            timeout: 3000,
          })
          return
        }
        const dados = resp.data
        this.series = []
        dados.forEach((item) => {
          const values = {
            SeriesDate: item['00080021']?.Value?.[0],
            SeriesTime: item['00080031']?.Value?.[0],
            PatientID: item['00100020']?.Value?.[0],
            SeriesDescription: item['0008103E']?.Value?.[0],
            StudyInstanceUID: item['0020000D']?.Value?.[0],
            SeriesInstanceUID: item['0020000E']?.Value?.[0],
          }
          this.series.push(values)
        })
      } catch (error) {
        console.log(error)
      } finally {
        this.dialogWait = false
      }
    },
    async searchImages(item) {
      this.dialogWait = true
      try {
        const resp = await this.$DicomService.dicomQIDOImages({
          StudyInstanceUID: item.StudyInstanceUID,
          SeriesInstanceUID: item.SeriesInstanceUID,
          query: {},
        })
        if (resp?.status == 204) {
          this.$dialog.notify.warning('Records no found.', {
            position: 'top-right',
            timeout: 3000,
          })
          return
        }
        const dados = resp.data
        this.images = []
        dados.forEach((item) => {
          const values = {
            InstanceNumber: item['00200013']?.Value?.[0],
            SOPInstanceUID: item['00080018']?.Value?.[0],
            SeriesInstanceUID: item['0020000E']?.Value?.[0],
            StudyInstanceUID: item['0020000D']?.Value?.[0],
            NumberOfFrames: item['00280008']?.Value?.[0] || '-',
          }
          this.images.push(values)
        })
      } catch (error) {
        console.log(error)
      } finally {
        this.dialogWait = false
      }
    },
  },
}
</script>
<style lang="scss" scoped>
::v-deep .marcarguias .v-data-footer__select .v-select {
  margin: 5px 0 5px 34px;
}
::v-deep tr.v-data-table__selected {
  background: #bcd9e9 !important;
}

::v-deep .v-data-table th[role='columnheader'] {
  white-space: nowrap;
}

::v-deep .v-tabs-bar {
  padding-bottom: 5px;
  padding-left: 5px;
}

//::v-deep .dstudies {
//  max-height: 200px;
//  overflow-y: auto;
//}
//::v-deep .dserie {
//  max-height: 200px;
//  overflow-y: auto;
//}
//::v-deep .dimage {
//  max-height: 300px;
//  overflow-y: auto;
//}
</style>
