export default function ({ $axios, $config }) {
  if (process.client) {
    const protocol = window.location.protocol
    const hostname = window.location.hostname
    const port = window.location.port
    // let url = `${protocol}//${hostname}:${port}/api/`
    // if (process.env.NODE_ENV == 'development') url = `${$config.baseURL}/api/`
    const url =
      process.env.NODE_ENV === 'development'
        ? `${$config.baseURL}/api/`
        : `${protocol}//${hostname}:${port}/api/`
    $axios.defaults.baseURL = url
  }
}
