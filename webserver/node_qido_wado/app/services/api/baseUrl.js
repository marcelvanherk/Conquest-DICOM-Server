export default (baseURL) => {
  let urlapi
  // if (process.env.NODE_ENV == 'development') urlapi = 'http://localhost:3001'
  if (process.env.NODE_ENV == 'development') urlapi = baseURL
  else {
    // em prod isso nginx redireciona para o endereco da api correta
    urlapi = `${window.location.protocol}//${window.location.hostname}:${window.location.port}`
  }
  return urlapi
}
