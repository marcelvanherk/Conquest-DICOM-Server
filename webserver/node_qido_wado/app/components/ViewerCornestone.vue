<template>
  <v-container fluid style="max-height: 90vh" class="pa-0 pt-1">
    <lazy-wait-dialog :d-wait="dialogWait" />
    <div class="wrapper">
      <!-- <div class="white--text">{{ desloc }}</div> -->
      <div class="toolbar d-flex justify-space-between">
        <div style="visibility: none"></div>
        <v-btn-toggle dense class="black">
          <v-btn
            title="Toggle Annotations"
            class="grey my-1 btnDicom"
            @click.stop="clickDP1"
          >
            <v-icon color="white" large>mdi-alpha-a</v-icon>
          </v-btn>
          <div v-if="dp1" id="drop-content1" class="d-flex flex-column">
            <v-btn
              title="Bidirectional"
              :class="{ active: activeBtn === 'btnBidir' }"
              class="grey my-1 btnDicom"
              @click.stop="handleBidirectional(canvas)"
            >
              <v-icon normal color="white">mdi-arrow-left-right</v-icon>
            </v-btn>
            <v-btn
              title="Arrow Annotate"
              :class="{ active: activeBtn === 'btnArrowAn' }"
              class="grey my-1 btnDicom"
              @click.stop="handleArrowAnnotate(canvas)"
            >
              <v-icon normal color="white">mdi-arrow-right</v-icon>
            </v-btn>
            <v-btn
              title="Elliptical ROI"
              :class="{ active: activeBtn === 'btnEllip' }"
              class="grey my-1 btnDicom"
              @click.stop="handleEllipticalRoi(canvas)"
            >
              <v-icon normal color="white">mdi-ellipse-outline</v-icon>
            </v-btn>
            <v-btn
              title="Rectangle ROI"
              class="grey my-1 btnDicom"
              :class="{ active: activeBtn === 'btnRect' }"
              @click.stop="handleRectangleROI(canvas)"
            >
              <v-icon normal color="white">mdi-rectangle-outline</v-icon>
            </v-btn>
            <v-btn
              title="FreeHand ROI"
              :class="{ active: activeBtn === 'btnFreeHand' }"
              class="grey my-1 btnDicom"
              @click.stop="handleFreehand(canvas)"
            >
              <v-icon normal color="white">mdi-gesture</v-icon>
            </v-btn>
            <v-btn
              title="Length"
              :class="{ active: activeBtn === 'btnLen' }"
              class="grey my-1 btnDicom"
              @click.stop="handleLength(canvas)"
            >
              <v-icon normal color="white">mdi-ruler</v-icon>
            </v-btn>
            <v-btn
              title="Angle"
              :class="{ active: activeBtn === 'btnAng' }"
              class="grey my-1 btnDicom"
              @click.stop="handleAngle(canvas)"
            >
              <v-icon normal color="white">mdi-angle-acute</v-icon>
            </v-btn>
            <v-btn
              title="Probe"
              :class="{ active: activeBtn === 'btnProb' }"
              class="grey my-1 btnDicom"
              @click.stop="handleProbe(canvas)"
            >
              <v-icon normal color="white">mdi-pencil</v-icon>
            </v-btn>
          </div>
          <v-btn
            title="Stack"
            :class="{ active: activeBtn === 'btnStack' }"
            class="grey my-1 btnDicom"
            @click.stop="handleStackScrollMouseWheel(canvas)"
          >
            <v-icon normal color="white">mdi-stack-exchange</v-icon>
          </v-btn>
          <v-btn
            title="WW/WC"
            class="grey my-1 btnDicom"
            :class="{ active: activeBtn === 'btnWWC' }"
            @click.stop="handleWWWC(canvas)"
          >
            <v-icon normal color="white">mdi-brightness-4</v-icon>
          </v-btn>
          <v-btn
            title="WW/WC"
            class="grey my-1 btnDicom"
            @click.stop="clickDP2()"
          >
            <v-icon normal color="white">mdi-arrow-collapse-down</v-icon>
          </v-btn>
          <div class="dropdown">
            <div v-if="dp2" id="drop-content2">
              <v-btn class="sliceButton" @click.stop="soft(canvas)">
                Soft Tissue(1)
              </v-btn>
              <v-btn class="sliceButton" @click.stop="bone(canvas)">
                Bone(2)
              </v-btn>
              <v-btn class="sliceButton" @click.stop="brain(canvas)">
                Brain(3)
              </v-btn>
              <v-btn class="sliceButton" @click.stop="stroke(canvas)">
                Stroke(4)
              </v-btn>
              <v-btn class="sliceButton" @click.stop="lung(canvas)">
                Lung(5)
              </v-btn>
            </div>
          </div>
          <v-btn
            title="Invert"
            class="grey my-1 btnDicom"
            :class="{ active: activeBtn === 'btnInvert' }"
            @click.stop="handleInvert(canvas)"
          >
            <v-icon normal color="white">mdi-invert-colors</v-icon>
          </v-btn>
          <v-btn
            title="Magnify"
            class="grey my-1 btnDicom"
            :class="{ active: activeBtn === 'btnMagnify' }"
            @click.stop="handleMagnify(canvas)"
          >
            <v-icon medium color="white">mdi-magnify-scan</v-icon>
          </v-btn>
          <v-btn
            title="Zoom"
            class="grey my-1 btnDicom"
            :class="{ active: activeBtn === 'btnZoom' }"
            @click.stop="handleZoom(canvas)"
          >
            <v-icon medium color="white">mdi-loupe</v-icon>
          </v-btn>
          <v-btn
            title="Pan"
            class="grey my-1 btnDicom"
            :class="{ active: activeBtn === 'btnPan' }"
            @click.stop="handlePan(canvas)"
          >
            <v-icon medium color="white">mdi-pan</v-icon>
          </v-btn>
          <v-btn
            title="Eraser"
            class="grey my-1 btnDicom"
            :class="{ active: activeBtn === 'btnEraser' }"
            @click.stop="handleEraser(canvas)"
          >
            <v-icon medium color="white">mdi-eraser</v-icon>
          </v-btn>
          <v-btn
            title="Reset"
            class="grey my-1 btnDicom"
            @click.stop="handleReset(canvas)"
          >
            <v-icon medium color="white">mdi-restore-alert</v-icon>
          </v-btn>
          <v-btn
            title="Rotate"
            class="grey my-1 btnDicom"
            @click.stop="clickDP5()"
          >
            <v-icon medium color="white">mdi-rotate-3d-variant</v-icon>
          </v-btn>
          <v-btn
            title="Remove Measurements"
            class="grey my-1 btnDicom"
            @click.stop="removeMeasures()"
          >
            <v-icon color="white" large>mdi-alpha-r</v-icon>
          </v-btn>

          <div class="dropdown">
            <div v-if="dp5" id="drop-content4" class="d-flex flex-column">
              <v-btn class="sliceButton" @click.stop="doFlip(canvas, 'hflip')">
                Flip H
              </v-btn>
              <v-btn class="sliceButton" @click.stop="doFlip(canvas, 'vflip')">
                Flip V
              </v-btn>
              <v-btn class="sliceButton" @click.stop="doRotate(canvas, -90)">
                Rotate L
              </v-btn>
              <v-btn class="sliceButton" @click.stop="doRotate(canvas, 90)">
                Rotate R
              </v-btn>
            </div>
          </div>
          <v-btn
            class="grey my-1 btnDicom"
            :class="{ active: activeBtn === 'btnPlayClick' }"
            title="Play Clip"
            @click.stop="playClip(canvas)"
          >
            <v-icon medium color="white">mdi-play</v-icon>
          </v-btn>

          <v-btn
            title="Print"
            class="grey my-1 btnDicom"
            @click.stop="toPDF(canvas)"
          >
            <v-icon normal color="white">mdi-printer</v-icon>
          </v-btn>
          <v-btn
            title="Save as"
            class="grey my-1 btnDicom"
            @click.stop="saveas"
          >
            <v-icon color="white" large>mdi-alpha-s</v-icon>
          </v-btn>

          <v-btn
            title="Full Screen"
            class="grey my-1 btnDicom"
            @click.stop="fullScreen()"
          >
            <v-icon color="white">mdi-fullscreen</v-icon>
          </v-btn>
        </v-btn-toggle>
        <v-progress-circular
          :rotate="90"
          :size="35"
          :width="2"
          :value="progress"
          color="#9ccef9"
          class="ma-1"
        >
          <span class="caption">{{ progress }}%</span>
        </v-progress-circular>
      </div>
      <div class="d-flex">
        <div class="left">
          <div
            id="divthumb"
            class="stack-wrapper scrollbar"
            @mousewheel.prevent="wheelThumbs"
          >
            <img
              v-for="(item, index) in thumbs"
              :id="`th${index}`"
              :key="index"
              :src="item.thumb"
              class="stack"
              :class="{ activeimg: index === activeImg }"
              @click.stop="selectItem(item, index)"
            />
          </div>
        </div>

        <div id="right" class="right">
          <div class="mrtopcenter plotinfo">{{ mrtopcenter }}</div>
          <div class="mrbottomcenter plotinfo">{{ mrbottomcenter }}</div>
          <div class="mrleftcenter plotinfo">{{ mrleftcenter }}</div>
          <div class="mrrightcenter plotinfo">{{ mrrightcenter }}</div>
          <div class="leftup plotinfo">
            <div>{{ leftup.leftup1 }}</div>
            <div>{{ leftup.leftup2 }}</div>
            <div>{{ leftup.leftup3 }}</div>
            <div>{{ leftup.leftup4 }}</div>
          </div>
          <div class="rightup plotinfo">
            <div>{{ rightup.rightup1 }}</div>
            <div>{{ rightup.rightup2 }}</div>
            <div>{{ rightup.rightup3 }}</div>
            <div>{{ rightup.rightup4 }}</div>
          </div>
          <div class="bottomleft plotinfo">{{ bottomleft }}</div>

          <div class="bottomright plotinfo">
            <div>{{ bottomright.bottomright1 }}</div>
            <div>{{ bottomright.bottomright2 }}</div>
          </div>
          <div class="opagination">
            <v-pagination
              v-if="stack.imageIds.length > 1"
              v-model="currentFrame"
              :length="stack.imageIds.length"
              :total-visible="5"
              @next="nextframe"
              @previous="previousframe"
              @input="gotoframe"
            ></v-pagination>
          </div>
          <div class="dicom-wrapper d-flex justify-center">
            <div id="dicomImage" ref="canvas" class="dicom-viewer"></div>
          </div>
        </div>
      </div>
    </div>
  </v-container>
</template>
<script>
//   deactivate = (name) => {
//     cornerstoneTools.setToolActive(this.referencedToolNames.stackScroll)
//     cornerstoneTools.setToolDisabled(this.referencedToolNames.pan)
//   }
//        activate() {
//     setToolDisabled(this.referencedToolNames.stackScroll);
//     setToolActive(this.referencedToolNames.pan, { mouseButtonMask: 1 });
//     setToolActive(this.referencedToolNames.zoomWheel, { mouseButtonMask: 4 });
//   }

// Packages
import Hammer from 'hammerjs'
import dicomParser from 'dicom-parser'
import * as cornerstone from 'cornerstone-core'
import * as cornerstoneMath from 'cornerstone-math'
import cornerstoneWADOImageLoader from 'cornerstone-wado-image-loader/dist/dynamic-import/cornerstoneWADOImageLoader.min.js'
import * as cornerstoneTools from 'cornerstone-tools'
const scrollToIndex = cornerstoneTools.importInternal('util/scrollToIndex')

cornerstoneWADOImageLoader.external.cornerstone = cornerstone
cornerstoneWADOImageLoader.external.dicomParser = dicomParser
cornerstoneTools.external.cornerstoneMath = cornerstoneMath
cornerstoneTools.external.cornerstone = cornerstone
cornerstoneTools.external.Hammer = Hammer
const zoomConfig = {
  configuration: {
    invert: false,
    preventZoomOutsideImage: false,
    minScale: 0.1,
    maxScale: 20.0,
  },
}
if (!cornerstoneWADOImageLoader.initialized) {
  const config = {
    maxWebWorkers: navigator.hardwareConcurrency || 1,
    startWebWorkersOnDemand: true,
    taskConfiguration: {
      decodeTask: {
        codecsPath: '/codecs/cornerstoneWADOImageLoaderCodecs.js',
      },
    },
  }
  cornerstoneWADOImageLoader.webWorkerManager.initialize(config)
}

let playBool = true

export default {
  name: 'ViewerCornestone',
  props: {
    mainImage: {
      type: String,
      default: null,
    },
    thumbsImages: {
      type: Array,
      default: () => [],
    },
  },

  data: () => ({
    // desloc: 0,
    progress: 0,
    instanceNumber: null,
    dialogWait: false,
    activeImg: null,
    measurements: [],
    currentFrame: 1,
    mprPlane: '',
    mrtopcenter: '',
    mrbottomcenter: '',
    mrleftcenter: '',
    mrrightcenter: '',
    leftup: {
      leftup1: null,
      leftup2: null,
      leftup3: null,
    },
    rightup: {
      rightup1: null,
      rightup2: null,
      rightup3: null,
      rightup4: null,
    },
    bottomleft: null,
    bottomright: {
      bottomright1: null,
      bottomright2: null,
    },
    stack: { currentImageIdIndex: 0, imageIds: [] },
    // thumbs: [],
    loaded: false,
    dp1: false,
    dp2: false,
    dp3: false,
    dp4: false,
    dp5: false,
    activeBtn: '',
    currentImage: null,
  }),
  head() {
    return {
      script: [
        {
          hid: 'printjs',
          src: 'https://printjs-4de6.kxcdn.com/print.min.js',
          defer: true,
          callback: () => {},
        },
        {
          hid: 'jsPDF',
          src: 'https://cdnjs.cloudflare.com/ajax/libs/jspdf/2.5.1/jspdf.umd.min.js',
          defer: true,
          callback: () => {},
        },
      ],
    }
  },
  computed: {
    thumbs() {
      return this.thumbsImages || []
    },
  },
  watch: {
    thumbs(newVal) {
      if (newVal) cornerstone.imageCache.purgeCache()
    },
    mainImage: {
      handler(newVal) {
        if (newVal) {
          this.$nextTick(() => {
            this.activeImg = this.thumbsImages.findIndex(
              (o) => o.mainImage == newVal
            )
            this.loadAndViewImage(newVal)
          })
        }
      },
      immediate: true,
    },
  },
  created() {
    cornerstoneTools.init({
      globalToolSyncEnabled: true,
    })

    // Grab Tool Classes
    const StackScrollMouseWheelTool = cornerstoneTools.StackScrollMouseWheelTool
    // const StackScrollTool = cornerstoneTools.StackScrollTool
    const WwwcTool = cornerstoneTools.WwwcTool
    const PanTool = cornerstoneTools.PanTool
    const PanMultiTouchTool = cornerstoneTools.PanMultiTouchTool
    const ZoomTool = cornerstoneTools.ZoomTool
    const FreehandRoiTool = cornerstoneTools.FreehandRoiTool
    const ZoomTouchPinchTool = cornerstoneTools.ZoomTouchPinchTool
    const ZoomMouseWheelTool = cornerstoneTools.ZoomMouseWheelTool

    cornerstoneTools.addTool(StackScrollMouseWheelTool)
    // cornerstoneTools.addTool(StackScrollTool)
    cornerstoneTools.addTool(PanTool)
    cornerstoneTools.addTool(ZoomTool, zoomConfig)
    cornerstoneTools.addTool(WwwcTool)
    cornerstoneTools.addTool(PanMultiTouchTool)
    cornerstoneTools.addTool(FreehandRoiTool)
    cornerstoneTools.addTool(ZoomTouchPinchTool)
    cornerstoneTools.addTool(ZoomMouseWheelTool)

    // Add them
    cornerstoneTools.setToolActive('StackScrollMouseWheel', {})
    // cornerstoneTools.setToolActive('StackScrollTool', { mouseButtonMask: 1 })
    cornerstoneTools.setToolActive('Pan', { mouseButtonMask: 4 }) // Middle
    cornerstoneTools.setToolActive('Zoom', { mouseButtonMask: 2 }) // Right
    cornerstoneTools.setToolActive('Wwwc', { mouseButtonMask: 1 }) // Left & Touch
    cornerstoneTools.setToolActive('PanMultiTouch', {})
    cornerstoneTools.setToolActive('FreehandRoi', { mouseButtonMask: 1 })
    cornerstoneTools.setToolActive('ZoomMouseWheel', {})
    cornerstoneTools.setToolActive('ZoomTouchPinch', {})
  },

  mounted() {
    window.addEventListener('resize', this.onResizeDoc)
    this.canvas = this.$refs.canvas
    cornerstone.enable(this.canvas, {
      renderer: 'webgl',
    })
    const onImageRendered = (e) => {
      const viewport = cornerstone.getViewport(e.target)
      this.bottomright.bottomright1 = `WW/WC: ${Math.round(
        viewport.voi.windowWidth
      )}/${Math.round(viewport.voi.windowCenter)}`

      this.bottomright.bottomright2 = `Zoom: ${Number(
        viewport.scale.toFixed(2) * 100
      ).toLocaleString('pt-BR', {
        minimumFractionDigits: 2,
        maximumFractionDigits: 2,
      })}%`

      this.leftup.leftup1 = (
        this.currentImage.data.string('x00100010') || ''
      ).substr(0, 15)
      this.leftup.leftup2 = (
        this.currentImage.data.string('x00100020') || ''
      ).substr(0, 15)
      this.leftup.leftup3 = (
        this.currentImage.data.string('x00081030') || ''
      ).substr(0, 15) //
      this.leftup.leftup4 = (
        this.currentImage.data.string('x00081040') || ''
      ).substr(0, 15) // depart

      this.rightup.rightup1 = this.$moment(
        this.currentImage.data.string('x00080020'),
        'YYYYMMDD'
      ).format('DD-MM-YYYY')
      this.instanceNumber = this.currentImage.data.string('x00200013')
      this.bottomleft = `Image #${this.instanceNumber}/${this.thumbs.length}`
      this.rightup.rightup2 = this.$moment({})
        .seconds(this.currentImage.data.string('x00080030'))
        .format('HH:mm:ss')
      this.rightup.rightup3 = this.currentImage.data.string('x00080060') // modality
      const dispArea = cornerstone.getDisplayedArea(this.currentImage, viewport)
      this.rightup.rightup4 = `${dispArea.brhc.x}x${dispArea.brhc.y}`

      this.activeImg = this.thumbsImages.findIndex(
        (o) => o.mainImage == this.currentImage.imageId.replace('wadouri:', '')
      )
      this.scrollThumb('th' + this.activeImg)
    }

    cornerstone.events.addEventListener(
      'cornerstoneimageloadprogress',
      (event) => {
        const eventData = event.detail
        this.progress = eventData.percentComplete
      }
    )
    this.canvas.addEventListener('cornerstoneimagerendered', onImageRendered)
    this.canvas.addEventListener(
      'cornerstonetoolsmeasurementcompleted',
      (e) => {
        const measure = {
          tool: e.detail.toolName, // this.props.tool,
          note: '',
          data: e.detail.measurementData,
        }
        this.measurements.push(measure)
      }
    )

    this.canvas.addEventListener('cornerstonenewimage', (e) => {
      if (this.stack.imageIds.length > 0) {
        this.bottomleft = `image #${this.stack.currentImageIdIndex + 1}/${
          this.stack.imageIds.length
        }`
      } else {
        this.bottomleft = `Image #${this.instanceNumber}/${this.thumbs.length}`
      }
    })
  },
  beforeUnmount() {
    window.removeEventListener('resize', this.onResizeDoc)
  },
  methods: {
    debounce(fn, d) {
      let timer
      return function () {
        const context = this
        const args = arguments
        clearTimeout(timer)
        timer = setTimeout(() => {
          fn.apply(context, args)
        }, d)
      }
    },
    onResizeDoc() {
      if (this.canvas && this.thumbs?.[this.activeImg]?.mainImage) {
        // console.log('loaded')
        this.loadAndViewImage(this.thumbs[this.activeImg].mainImage)
        // cornerstone.resize(this.canvas, false)
      }
    },
    scrollParentToChild(parent, child) {
      // Where is the parent on page
      const parentRect = parent.getBoundingClientRect()
      // What can you see?
      const parentViewableArea = {
        height: parent.clientHeight,
        width: parent.clientWidth,
      }

      // Where is the child
      const childRect = child.getBoundingClientRect()
      // Is the child viewable?
      const isViewable =
        childRect.top >= parentRect.top &&
        childRect.top <= parentRect.top + parentViewableArea.height

      // if you can't see the child try to scroll parent
      if (!isViewable) {
        // scroll by offset relative to parent
        parent.scrollTop = childRect.top + parent.scrollTop - parentRect.top
      }
    },
    scrollThumb(id) {
      this.scrollParentToChild(
        document.getElementById('divthumb'),
        document.getElementById(id)
      )
    },
    selectItem(item, index) {
      this.activeImg = index
      this.loadAndViewImage(item.mainImage)
    },
    printImg() {
      // window.printJS('right', 'html')
      const canvas = document.getElementsByClassName('cornerstone-canvas')?.[0]
      if (!canvas) return
      // window.printJS({
      //   printable: canvas.toDataURL('image/png'),
      //   type: 'image',
      //   imageStyle: 'width:100%',
      // })

      canvas.toBlob((blob) => {
        window.open(URL.createObjectURL(blob), '_blank')
      }, 'image/png')
      // const url = canvas.toDataURL('image/png')
      // const blob = this.dataURItoBlob(url)
      // window.open(URL.createObjectURL(blob), '_blank')
    },
    toPDF(element) {
      const type = 'png'
      const viewport = cornerstone.getViewport(element)
      const canvas = document.getElementsByClassName('cornerstone-canvas')[0]
      const zoom = viewport.scale.toFixed(2)
      const cols = this.currentImage.columns * zoom
      const rows = this.currentImage.rows * zoom

      let myCanvas = document.createElement('canvas')
      myCanvas = this.cropCanvas(
        canvas,
        Math.round(canvas.width / 2 - cols / 2),
        Math.round(canvas.height / 2 - rows / 2),
        cols,
        rows
      )
      myCanvas.toBlob((blob) => {
        window.open(URL.createObjectURL(blob), '_blank')
      }, `image/${type}`)
    },

    removeMeasures() {
      const element = this.canvas
      // for each measurement remove it
      this.measurements.forEach((measure) => {
        cornerstoneTools.clearToolState(element, measure.tool)
      })
      cornerstone.updateImage(element)
    },
    firstframe() {
      const element = this.canvas
      const frame = 1
      this.currentFrame = frame
      scrollToIndex(element, 0)
    },
    gotoframe(page) {
      const element = this.canvas
      scrollToIndex(element, page - 1)
    },
    previousframe() {
      const element = this.canvas
      if (this.currentFrame > 1) {
        const frame = this.currentFrame
        // this.currentFrame = frame
        scrollToIndex(element, frame - 1)
      }
    },
    // case 'play': {
    //   cornerstoneTools.playClip(element, 30)
    //   this.setState({inPlay: true})
    //   break
    // }
    // case 'pause': {
    //     cornerstoneTools.stopClip(element)
    //     this.setState({inPlay: false})
    //   break
    // }
    nextframe() {
      const element = this.canvas
      if (this.currentFrame < this.stack.imageIds.length) {
        const frame = this.currentFrame
        scrollToIndex(element, frame - 1)
      }
    },
    lastframe(page) {
      const element = this.canvas
      const frame = this.stack.imageIds.length
      this.currentFrame = frame
      scrollToIndex(element, frame - 1)
    },
    plotCenter() {
      if (this.mprPlane === 'sagittal') {
        this.mrtopcenter1 = `S`
        this.mrbottomcenter = `I`
        this.mrleftcenter = `A`
        this.mrrightcenter = `P`
      } else if (this.mprPlane === 'axial') {
        this.mrtopcenter = `A`
        this.mrbottomcenter = `P`
        this.mrleftcenter = `R`
        this.mrrightcenter = `L`
      } else if (this.mprPlane === 'coronal') {
        this.mrtopcenter = `S`
        this.mrbottomcenter = `I`
        this.mrleftcenter = `R`
        this.mrrightcenter = `L`
      } else {
        this.mrtopcenter = `A`
        this.mrbottomcenter = `P`
        this.mrleftcenter = `R`
        this.mrrightcenter = `L`
      }
    },
    mprPlanePosition(image) {
      let resp
      try {
        // if (!this.isDicom) return this.mprPlane
        // const image = this.files[0].image
        const imageOrientation = image.data.string('x00200037').split('\\')
        let v = new Array(6).fill(0)
        v[0] = parseFloat(imageOrientation[0]) // the x direction cosines of the first row X
        v[1] = parseFloat(imageOrientation[1]) // the y direction cosines of the first row X
        v[2] = parseFloat(imageOrientation[2]) // the z direction cosines of the first row X
        v[3] = parseFloat(imageOrientation[3]) // the x direction cosines of the first column Y
        v[4] = parseFloat(imageOrientation[4]) // the y direction cosines of the first column Y
        v[5] = parseFloat(imageOrientation[5]) // the z direction cosines of the first column Y
        v = v.map((x) => Math.round(x))
        let p = [
          v[1] * v[5] - v[2] * v[4],
          v[2] * v[3] - v[0] * v[5],
          v[0] * v[4] - v[1] * v[3],
        ] // cross product of X x Y
        p = p.map((x) => Math.abs(x))
        if (p[0] === 1) {
          resp = 'sagittal'
        } else if (p[1] === 1) {
          resp = 'coronal'
        } else if (p[2] === 1) {
          resp = 'axial'
        }
      } catch (error) {
        resp = ''
      }
      return resp
    },
    clickDP1() {
      this.closeDropDown(1)
      this.dp1 = !this.dp1
    },
    clickDP5() {
      this.closeDropDown(5)
      this.dp5 = !this.dp5
    },
    clickDP2() {
      this.closeDropDown(2)
      this.dp2 = !this.dp2
    },
    /* View in fullscreen */
    openFullscreen(elem) {
      if (elem.requestFullscreen) {
        elem.requestFullscreen()
      } else if (elem.mozRequestFullScreen) {
        /* Firefox */
        elem.mozRequestFullScreen()
      } else if (elem.webkitRequestFullscreen) {
        /* Chrome, Safari and Opera */
        elem.webkitRequestFullscreen()
      } else if (elem.msRequestFullscreen) {
        /* IE/Edge */
        elem.msRequestFullscreen()
      }
    },

    /* Close fullscreen */
    closeFullscreen(document) {
      if (document.exitFullscreen) {
        document.exitFullscreen()
      } else if (document.mozCancelFullScreen) {
        /* Firefox */
        document.mozCancelFullScreen()
      } else if (document.webkitExitFullscreen) {
        /* Chrome, Safari and Opera */
        document.webkitExitFullscreen()
      } else if (document.msExitFullscreen) {
        /* IE/Edge */
        document.msExitFullscreen()
      }
    },

    fullScreen() {
      if (document.fullscreenElement) {
        document
          .exitFullscreen()
          .then(() => console.log('Document Exited from Full screen mode'))
          .catch((err) => console.error(err))
      } else {
        document.documentElement.requestFullscreen()
      }
    },
    async loadAndViewImage(im) {
      this.closeDropDown(-1)
      this.loaded = false
      // this.dialogWait = true
      cornerstone.disable(this.canvas)
      cornerstone.enable(this.canvas)
      // cornerstone.imageCache.purgeCache()

      // const cachedImage = cornerstone.imageCache.imageCache['wadouri:' + im]
      // console.log(im, cachedImage)

      // if (cachedImage && cachedImage.loaded) {
      //   // image in cache and fully loaded
      // }

      // if (cachedImage && !cachedImage.loaded) {
      //   // image currently beeing loaded
      //   console.log('cached21')
      // }

      // if (!cachedImage || !cachedImage.loaded) {
      //   // image not jet loaded
      // }

      const dataSet =
        await cornerstoneWADOImageLoader.wadouri.dataSetCacheManager.load(
          im,
          cornerstoneWADOImageLoader.internal.xhrRequest
        )
      const numFrames = dataSet.intString('x00280008') || 0
      const imageTemps = []
      const imageIdRoot = 'wadouri:' + im
      for (let i = 0; i < numFrames; i++) {
        const t = imageIdRoot + '?frame=' + i
        imageTemps.push(t)
      }

      this.stack = {
        currentImageIdIndex: 0,
        imageIds: imageTemps,
      }

      // const imw = 'wadouri:' + im -- precisa wadouri
      cornerstone.loadAndCacheImage(imageIdRoot).then((image) => {
        // precisa prefixo wadouri
        this.currentImage = image
        const viewport = cornerstone.getDefaultViewportForImage(
          this.canvas,
          image
        )
        // now that we have an image frame in the cornerstone cache, we can decrement
        // the reference count added by load() above when we loaded the metadata.  This way
        // cornerstone will free all memory once all imageId's are removed from the cache
        // const url = im.replace('wadouri:', '')
        // cornerstoneWADOImageLoader.wadouri.dataSetCacheManager.unload(im)
        cornerstone.displayImage(this.canvas, image, viewport)

        cornerstone.resize(this.canvas, true)
        // cornerstone.reset(this.canvas)
        this.mprPlane = this.mprPlanePosition(image)
        // this.removeMeasures()
        this.plotCenter()
        if (this.stack.imageIds.length > 0) {
          cornerstoneTools.clearToolState(this.canvas, 'stack')
          cornerstoneTools.addStackStateManager(this.canvas, [
            'stack',
            'playClip',
          ])
          cornerstoneTools.addToolState(this.canvas, 'stack', this.stack)
          this.bottomleft = `Image
          ${this.stack.currentImageIdIndex + 1}/${this.stack.imageIds.length}`
        } else {
          this.bottomleft = `Image #${this.instanceNumber}/${this.thumbs.length}`
        }
        this.loaded = true
        // this.dialogWait = false
        cornerstoneTools.setToolDisabled('FreehandRoi')
        cornerstoneTools.setToolDisabled('Magnify')
        const ZoomTool = cornerstoneTools.ZoomTool
        cornerstoneTools.addTool(ZoomTool, zoomConfig)
        cornerstoneTools.setToolActive('Zoom', { mouseButtonMask: 1 })
      })
    },

    cropCanvas(canvas, x, y, width, height) {
      // create a temp canvas
      const newCanvas = document.createElement('canvas')
      // set its dimensions
      newCanvas.width = width
      newCanvas.height = height
      // draw the canvas in the new resized temp canvas
      newCanvas
        .getContext('2d')
        .drawImage(canvas, x, y, width, height, 0, 0, width, height)
      return newCanvas
    },

    saveas() {
      const type = 'png' // localStorage.getItem(SETTINGS_SAVEAS)
      // getSettingsSaveInto()
      // cornerstoneTools.SaveAs(this.dicomImage, `${this.filename}.${type}`, `image/${type}`)
      const element = this.canvas
      const viewport = cornerstone.getViewport(element)
      const canvas = document.getElementsByClassName('cornerstone-canvas')[0]
      const zoom = viewport.scale.toFixed(2)
      const cols = this.currentImage.columns * zoom
      const rows = this.currentImage.rows * zoom

      let myCanvas = document.createElement('canvas')
      myCanvas = this.cropCanvas(
        canvas,
        Math.round(canvas.width / 2 - cols / 2),
        Math.round(canvas.height / 2 - rows / 2),
        cols,
        rows
      )
      const filename = this.currentImage.data.string('x00100010') || 'paciente'
      const a = document.createElement('a')
      a.href = myCanvas.toDataURL(`image/${type}`)
      a.download = `${filename}.${type}`
      document.body.appendChild(a) // Required for this to work in FireFox
      a.click()
      // }
      // else {
      // store image into sandbox file system
      // const element = this.canvas
      // const viewport = cornerstone.getViewport(element)
      // const canvas = document.getElementsByClassName('cornerstone-canvas')[0]
      // const zoom = viewport.scale.toFixed(2)
      // const cols = this.image.columns * zoom
      // const rows = this.image.rows * zoom
      // let myCanvas = document.createElement('canvas')
      // myCanvas = this.cropCanvas(canvas,
      //   Math.round(canvas.width / 2 - cols / 2),
      //   Math.round(canvas.height / 2 - rows / 2),
      //   cols, rows)
      // blobUtil.canvasToBlob(myCanvas, `image/${type}`).then(blob => {
      //   blobUtil.blobToArrayBuffer(blob).then(arrayBuffer => {
      //     const name = `${getFileName(this.filename)}-MPR-${this.mprPlane}`
      //     let newName = name
      //     let counter = 1
      //     let done = false
      //     do {
      //       let filename = `${newName}.${type}`
      //       const checkName = this.props.fsCurrentList.find(e => e.name === filename)
      //       if (checkName === undefined) {
      //         fs.transaction('rw', fs.files, async () => {
      //           await fs.files.add({
      //             parent: this.props.fsCurrentDir,
      //             name: filename,
      //             type: type,
      //             size: arrayBuffer.byteLength,
      //             data: arrayBuffer
      //           })
      //         }).then(() => {
      //           this.props.makeFsRefresh()
      //         })
      //         done = true
      //       } else {
      //         newName = `${name} - ${counter}`
      //         counter++
      //       }
      //     } while (!done)
      //   })
      // })
      // }
    },
    playClip(element) {
      this.canvas.onwheel = null

      this.activeBtn = 'btnPlayClick'
      if (playBool === true) {
        // frame rate (0008,2144)
        const frameRate = this.currentImage.data.string('x00082144') || 10
        cornerstoneTools.playClip(element, frameRate)
        playBool = false
      } else {
        cornerstoneTools.stopClip(element)
        playBool = true
      }
    },
    soft(element) {
      this.canvas.onwheel = null
      this.closeDropDown(2)
      const viewport = cornerstone.getViewport(element)
      viewport.voi.windowWidth = 400
      viewport.voi.windowCenter = 20
      cornerstone.setViewport(element, viewport)
      this.setBottomRight1(viewport)
    },
    bone(element) {
      this.canvas.onwheel = null
      this.closeDropDown(2)
      const viewport = cornerstone.getViewport(element)
      viewport.voi.windowWidth = 2000
      viewport.voi.windowCenter = 300
      cornerstone.setViewport(element, viewport)
      this.setBottomRight1(viewport)
    },
    brain(element) {
      this.canvas.onwheel = null
      this.closeDropDown(2)
      const viewport = cornerstone.getViewport(element)
      viewport.voi.windowWidth = 400
      viewport.voi.windowCenter = 240
      cornerstone.setViewport(element, viewport)
      this.setBottomRight1(viewport)
    },
    stroke(element) {
      this.canvas.onwheel = null
      this.closeDropDown(2)
      const viewport = cornerstone.getViewport(element)
      viewport.voi.windowWidth = 1600
      viewport.voi.windowCenter = -600
      cornerstone.setViewport(element, viewport)
      this.setBottomRight1(viewport)
    },
    lung(element) {
      this.canvas.onwheel = null
      this.closeDropDown(2)
      const viewport = cornerstone.getViewport(element)
      viewport.voi.windowWidth = 358
      viewport.voi.windowCenter = 240
      cornerstone.setViewport(element, viewport)
      this.setBottomRight1(viewport)
    },
    setBottomRight1(viewport) {
      this.canvas.onwheel = null
      this.bottomright.bottomright1 =
        'WW/WC:' +
        Math.round(viewport.voi.windowWidth) +
        '/' +
        Math.round(viewport.voi.windowCenter)
    },
    handleWWWC(element) {
      this.canvas.onwheel = null
      this.closeDropDown(-1)
      this.activeBtn = 'btnWWC'
      this.AddTool('Wwwc')
      // const WwwcTool = cornerstoneTools.WwwcTool
      // if (this.loaded) {
      //   cornerstoneTools.addTool(WwwcTool)
      //   cornerstoneTools.setToolActive('Wwwc', { mouseButtonMask: 1 })
      // } else {
      //   cornerstoneTools.removeTool('WwwcTool')
      // }
    },
    handleZoom(element) {
      this.canvas.onwheel = null
      this.closeDropDown(-1)
      this.activeBtn = 'btnZoom'
      this.AddTool('Zoom')
      this.AddTool('ZoomMouseWheel')
      cornerstoneTools.removeTool('Magnify')
    },
    handlePan(element) {
      this.canvas.onwheel = null
      this.closeDropDown(-1)
      this.activeBtn = 'btnPan'
      this.AddTool('Pan')
      cornerstoneTools.removeTool('Magnify')
    },
    handleMagnify(element) {
      this.canvas.onwheel = null
      this.closeDropDown(-1)
      this.activeBtn = 'btnMagnify'
      this.AddTool('Magnify')
      cornerstoneTools.removeTool('Zoom')
    },
    closeDropDown(dp) {
      if (dp !== 1) this.dp1 = false
      if (dp !== 2) this.dp2 = false
      if (dp !== 3) this.dp3 = false
      if (dp !== 4) this.dp4 = false
      if (dp !== 5) this.dp5 = false
    },
    handleBidirectional() {
      this.canvas.onwheel = null
      this.closeDropDown(1)
      this.activeBtn = 'btnBidir'
      this.AddTool('Bidirectional')
    },
    // handleChangeLayout(element) {},
    // handleSortPlus() {
    // },
    // handleSortMinus() {
    // imageId1 = imageId1.reverse()
    // loadAndViewImage(imageId1)
    // },
    handleEllipticalRoi(element) {
      this.canvas.onwheel = null
      this.closeDropDown(1)
      this.activeBtn = 'btnEllip'
      this.AddTool('EllipticalRoi')
    },

    handleRectangleROI(element) {
      this.canvas.onwheel = null
      this.closeDropDown(1)
      this.activeBtn = 'btnRect'
      this.AddTool('RectangleRoi')
    },
    handleFreehand(element) {
      this.canvas.onwheel = null
      this.closeDropDown(1)
      this.activeBtn = 'btnFreeHand'
      this.AddTool('FreehandRoi')
    },
    handleLength(element) {
      this.canvas.onwheel = null
      this.closeDropDown(1)
      this.activeBtn = 'btnLen'
      this.AddTool('Length')
    },
    AddTool(toolName) {
      // Add the tool
      this.canvas.onwheel = null
      if (this.loaded) {
        const apiTool = cornerstoneTools[`${toolName}Tool`]
        cornerstoneTools.addTool(apiTool)
        cornerstoneTools.setToolActive(toolName, { mouseButtonMask: 1 })
      } else {
        cornerstoneTools.removeTool(toolName)
      }
    },

    handleAngle(element) {
      this.canvas.onwheel = null
      this.closeDropDown(1)
      this.activeBtn = 'btnAng'
      this.AddTool('Angle')
    },

    handleEraser(element) {
      this.canvas.onwheel = null
      this.closeDropDown(-1)
      this.activeBtn = 'btnEraser'
      this.AddTool('Eraser')
    },

    handleProbe(element) {
      this.canvas.onwheel = null
      this.closeDropDown(1)
      this.activeBtn = 'btnProb'
      this.AddTool('Probe')
    },
    handleReferenceLine(element) {
      this.canvas.onwheel = null
      this.closeDropDown(-1)
      this.activeBtn = 'btnReferenceLine'
      const elements = cornerstone.getEnabledElements()
      const synchronizer = new cornerstoneTools.Synchronizer(
        'cornerstonenewimage',
        cornerstoneTools.updateImageSynchronizer
      )
      elements.forEach((value) => {
        const { element } = value
        synchronizer.add(element)
      })
      synchronizer.enabled = true
      cornerstoneTools.addTool(cornerstoneTools.ReferenceLinesTool)
      cornerstoneTools.setToolEnabled('ReferenceLines', {
        synchronizationContext: synchronizer,
      })
    },
    // handleStackScrollTool(element) {
    //   this.closeDropDown(-1)
    //   if (this.stack.imageIds.length < 1) return
    //   const StackScrollTool = cornerstoneTools.StackScrollTool
    //   // if (series.length < 1) {
    //   //   alert('upload several DICOM.')
    //   //   return false
    //   // }
    //   // const imageIds = series.map((seriesImage) => seriesImage)
    //   // const stack = {
    //   //   currentImageIdIndex: 0,
    //   //   imageIds,
    //   // }
    //   cornerstoneTools.addStackStateManager(element, ['stack'])
    //   cornerstoneTools.addToolState(element, 'stack', this.stack)
    //   cornerstoneTools.addTool(StackScrollTool)
    //   cornerstoneTools.setToolActive('StackScrollTool', { mouseButtonMask: 1 })
    // },
    handleStackScrollMouseWheel(element) {
      cornerstoneTools.removeTool('ZoomMouseWheel')
      cornerstoneTools.removeTool('StackScrollMouseWheel')
      this.closeDropDown(-1)
      this.activeBtn = 'btnStack'
      if (this.stack.imageIds.length > 0) {
        this.canvas.onwheel = null
        const StackScrollMouseWheelTool =
          cornerstoneTools.StackScrollMouseWheelTool
        cornerstoneTools.addStackStateManager(element, ['stack'])
        cornerstoneTools.addToolState(element, 'stack', this.stack)
        cornerstoneTools.addTool(StackScrollMouseWheelTool)
        cornerstoneTools.setToolActive('StackScrollMouseWheel', {
          mouseButtonMask: 1,
        })
      } else if (this.thumbs.length > 0) {
        this.canvas.onwheel = this.debounce(this.wheelE, 50)
      } else {
        this.canvas.onwheel = null
      }
    },
    wheelE(e) {
      e.stopPropagation()
      e.preventDefault()

      if (e.deltaY > 0) {
        // next
        this.sliceKeys(1)
      } else if (e.deltaY < 0) {
        // prev
        this.sliceKeys(-1)
      }
    },
    wheelThumbs(e) {
      this.debounce(this.wheelE(e), 50)
    },
    sliceKeys(dir) {
      if (dir > 0) {
        if (this.thumbs.length > 0) {
          if (this.activeImg < this.thumbs.length) {
            this.activeImg++
            if (this.activeImg == this.thumbs.length) this.activeImg = 0
          }
        }
      } else if (dir < 0) {
        // prev
        this.activeImg--
        if (this.activeImg < 0) this.activeImg = this.thumbs.length - 1
      }
      this.loadAndViewImage(this.thumbs[this.activeImg].mainImage)
    },

    handleInvert(element) {
      this.canvas.onwheel = null
      this.activeBtn = 'btnInvert'
      const viewport = cornerstone.getViewport(element)
      viewport.invert = !viewport.invert
      cornerstone.setViewport(element, viewport)
    },

    handleArrowAnnotate(element) {
      this.canvas.onwheel = null
      this.closeDropDown(1)
      this.activeBtn = 'btnArrowAn'
      this.AddTool('ArrowAnnotate')
    },

    handleReset(element) {
      this.canvas.onwheel = null
      this.closeDropDown(-1)
      cornerstone.reset(element)
      this.activeBtn = null
    },
    // Add event handlers to flip or rotate the image
    doFlip(element, op) {
      this.canvas.onwheel = null
      this.closeDropDown(5)
      const viewport = cornerstone.getViewport(element)
      viewport[op] = !viewport[op]
      cornerstone.setViewport(element, viewport)
    },

    doRotate(element, angle) {
      this.canvas.onwheel = null
      this.closeDropDown(5)
      const viewport = cornerstone.getViewport(element)
      viewport.rotation += angle
      cornerstone.setViewport(element, viewport)
    },
  },
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.active,
.btn:hover {
  background-color: #6e7069 !important;
  color: black;
  border: 2px solid red;
  background-color: white;
  color: black;
}

.left {
  width: 90px;
  float: left;
  background-color: black;
  position: relative;
  text-align: center;
  /* border: 1px solid #9ccef9; */
  margin-left: 3px;
}
.right {
  flex-grow: 1;
  float: right;
  background-color: black;
  position: relative;
  /* border: solid 1px #9ccef9; */
  margin-right: 10px;
  margin-left: 5px;
}
.dicom-wrapper {
  width: 100%;
  height: 80vh;
  padding: 5px;
  margin: 0px;
  background-color: black;
  position: relative;
}
.wrapper {
  background-color: black;
  width: 100%;
  height: calc(100vh - 50px);
  padding: 0;
  margin: 0;
  /* overflow-y: hidden; */
  position: relative;
}
.dicom-viewer {
  width: 100%;
  height: 100%;
  position: relative;
  margin-top: auto !important;
  margin-bottom: auto !important;
}
.toolbar {
  width: 100%;
  background-color: black;
  position: relative;
}
.sliceButton {
  min-width: 0;
  width: 140px;
  height: 50px;
  background-color: gray !important;
  margin-top: 5px;
  color: white;
  position: relative;
}

.mrtopcenter {
  top: 3px;
  left: 50%;
  position: absolute;
  color: white !important;
}
.mrbottomcenter {
  bottom: 3px;
  left: 50%;
  position: absolute;
  color: white !important;
}
.mrleftcenter {
  top: 50%;
  left: 3px;
  position: absolute;
  color: white !important;
}
.mrrightcenter {
  top: 50%;
  right: 3px;
  position: absolute;
  color: white !important;
}
.plotinfo {
  color: #9ccef9;
  font-size: 14px;
  font-weight: 500;
  z-index: 10;
}
.leftup {
  float: left;
  top: 0;
  left: 3px;
  position: absolute;
}

.bottomleft {
  float: left;
  bottom: 3px;
  left: 3px;
  position: absolute;
}
.opagination {
  bottom: 0px;
  left: 30%;
  background: transparent;
  color: white;
  position: absolute;
  z-index: 10;
}
.bottomright {
  float: right;
  bottom: 3px;
  right: 3px;
  position: absolute;
}

.rightup {
  float: right;
  top: 0;
  right: 3px;
  position: absolute;
}
/* .stack {
  width: 110px;
  height: 100px;
  margin: 0px;
  position: relative;
  border: 2px solid grey;
  border-radius: 12px;
} */
.stack {
  width: 60px;
  height: 50px;
  margin: 0px;
  position: relative;
  border: 2px solid grey;
  border-radius: 12px;
}
.activeimg {
  border: 2px solid cyan;
  border-radius: 12px;
}

/* .stack-wrapper {
  height: 80vh;
  width: 150px;
  overflow: hidden;
} */

.stack-wrapper {
  height: 80vh;
  width: 90px;
  overflow: auto;
}
.stack-wrapper:first-child {
  margin-top: 5px;
}
/* .stack-wrapper:hover {
  overflow-y: auto;
} */
.dropdown {
  position: relative;
  display: inline-block;
}
#drop-content1 {
  position: absolute;
  top: 6vh;
  z-index: 1;
}
#drop-content2 {
  position: absolute;
  top: 6vh;
  left: -40px;
  z-index: 1;
}
#drop-content3 {
  position: absolute;
  top: 6vh;
  left: -40px;
  z-index: 1;
}
#drop-content4 {
  position: absolute;
  top: 6vh;
  left: -40px;
  z-index: 1;
}
#drop-content5 {
  position: absolute;
  top: 6vh;
  left: -40px;
  z-index: 1;
}
.btnDicom {
  min-width: 0;
  width: 40;
}

/* Customize website's scrollbar like Mac OS
Not supports in Firefox and IE */
/* total width */
.scrollbar::-webkit-scrollbar {
  background-color: #fff;
  width: 15px;
}
.scrollbar::-webkit-scrollbar-track {
  box-shadow: inset 0 0 5px grey;
  border-radius: 3px;
}
/* .scrollbar::-webkit-scrollbar-track:hover {
  background-color: #f4f4f4;
} */

/* scrollbar itself */
.scrollbar::-webkit-scrollbar-thumb {
  background-color: #000;
  border-radius: 16px;
  border: 5px solid #fff;
}
/* .scrollbar::-webkit-scrollbar-thumb:hover {
  background-color: #404061;
  border: 4px solid #f4f4f4;
} */

/* set button(top and bottom of the scrollbar) */
/* .scrollbar::-webkit-scrollbar-button {
  display: none;
} */
</style>
