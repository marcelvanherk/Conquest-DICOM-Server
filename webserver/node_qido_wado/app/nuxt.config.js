// import colors from 'vuetify/es5/util/colors'
import CopyPlugin from 'copy-webpack-plugin'
// import pt from 'vuetify/es5/locale/pt'
export default {
  // Target: https://go.nuxtjs.dev/config-target
  target: 'static',
  ssr: false,
  // Refused to frame 'https://www.mercadopago.com.br/' because an ancestor violates the following Content Security Policy directive: "frame-ancestors 'self' *.mercadolibre.com"
  // Global page headers: https://go.nuxtjs.dev/config-head
  head: {
    // titleTemplate: "%s - iClinDoctor",
    title: 'Conquest Dicom Sample App',
    meta: [
      { charset: 'utf-8' },
      { name: 'viewport', content: 'width=device-width, initial-scale=1' },
      {
        hid: 'description',
        name: 'description',
        content: 'Conquest Dicom Sample App',
      },
      {
        hid: 'robots',
        name: 'robots',
        content: 'index,follow',
      },
    ],
    link: [{ rel: 'icon', type: 'image/x-icon', href: '/favicon.ico' }],
  },

  publicRuntimeConfig: {
    // url da api: se em dev - pega esse senão pega a url do dominio corrente no plugin axios
    baseURL: 'http://localhost:3002',
  },
  // Global CSS: https://go.nuxtjs.dev/config-css
  css: [],

  // Plugins to run before rendering page: https://go.nuxtjs.dev/config-plugins
  plugins: [
    { src: '~/plugins/Vuelidate' },
    {
      src: '~/plugins/services.js',
    },
    { src: '~/plugins/axios' },
  ],

  // Auto import components: https://go.nuxtjs.dev/config-components
  // components: true,
  components: {
    dirs: ['~/components'],
  },
  // Modules for dev and build (recommended): https://go.nuxtjs.dev/config-modules
  buildModules: [
    // https://go.nuxtjs.dev/eslint
    '@nuxtjs/eslint-module',
    // https://go.nuxtjs.dev/vuetify
    '@nuxtjs/vuetify',
    '@nuxtjs/moment',
  ],
  moment: {
    locales: ['pt-br'],
  },
  // Modules: https://go.nuxtjs.dev/config-modules
  modules: [
    // https://go.nuxtjs.dev/axios
    '@nuxtjs/axios',
    '@nuxtjs/auth-next',
    // https://go.nuxtjs.dev/pwa
    '@nuxtjs/pwa',
    'vuetify-dialog/nuxt',
  ],

  // Axios module configuration (https://go.nuxtjs.dev/config-axios)
  axios: {
    // baseURL: `${API_BASE_URL()}/api`,
    timeout: 30000,
    withCredentials: false,
  },

  // Axios module configuration: https://go.nuxtjs.dev/config-axios

  // PWA module configuration: https://go.nuxtjs.dev/pwa
  pwa: {
    manifest: {
      lang: 'en',
    },
  },

  // Vuetify module configuration: https://go.nuxtjs.dev/config-vuetify

  vuetify: {
    customVariables: ['~/assets/variables.scss'],
    treeShake: true,
    defaultAssets: {
      font: false,
    },
    // lang: {
    //   locales: { pt },
    //   current: 'pt',
    // },
    theme: {
      dark: false,
      themes: {
        light: {
          primary: '#42a5f6',
          secondary: '#050b1f',
          accent: '#204165',
        },
        dark: {},
      },
    },
  },

  auth: {},
  // build: {},
  // Build Configuration: https://go.nuxtjs.dev/config-build
  build: {
    extend(config, { isDev, isClient }) {
      config.resolve.alias['cornerstone-wado-image-loader'] =
        'cornerstone-wado-image-loader/dist/dynamic-import/cornerstoneWADOImageLoader.min.js'
    },
    plugins: [
      new CopyPlugin([
        {
          from: 'node_modules/cornerstone-wado-image-loader/dist/dynamic-import',
        },
      ]),
    ],
  },
}
