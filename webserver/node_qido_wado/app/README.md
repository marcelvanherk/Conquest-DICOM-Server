# Conquest PACS Dicom Viewer  

This viewer uses the node.js QIDO/WADO api to conquest dicom server, 
configure and install that one first and note its IP address and port number.

The viewer component used is cornerstone. There is a papaya bonus component as well(not used in demo app). 
But I had prefer conerstore because it works better with vue.js and was faster in my testing.

## Requirements
* Install [Node.js]{https://nodejs.org/en/}

## Build Setup

```bash
# unzip the folder and go there
# cd 'your_unpacked_folder'

# install dependencies
$ npm install

# configure the IP addres and port of the API 
  default localhost:3002 in dev mode, localhost:3000 in production mode
  to change this configuration you need edit the API protocol, hostname and port to access in production as:
Edit the file: axios.js in the plugin folder and change these lines:
```
    const protocol = -- replace with the protocol used to access the API (http or https)  
    const hostname = -- replace with the hostname used to access the API
    const port =  -- replace with the port used to access the API
```    

# serve with hot reload at localhost:3000
$ npm run dev

# build for production and launch server
$ npm run build
$ npm run generate
$ npm run start

```

For detailed explanation on how things work, check out the [documentation](https://nuxtjs.org).

## Special Directories

You can create the following extra directories, some of which have special behaviors. Only `pages` is required; you can delete them if you don't want to use their functionality.

### `assets`

The assets directory contains your uncompiled assets such as Stylus or Sass files, images, or fonts.

More information about the usage of this directory in [the documentation](https://nuxtjs.org/docs/2.x/directory-structure/assets).

### `components`

The components directory contains your Vue.js components. Components make up the different parts of your page and can be reused and imported into your pages, layouts and even other components.

More information about the usage of this directory in [the documentation](https://nuxtjs.org/docs/2.x/directory-structure/components).

### `layouts`

Layouts are a great help when you want to change the look and feel of your Nuxt app, whether you want to include a sidebar or have distinct layouts for mobile and desktop.

More information about the usage of this directory in [the documentation](https://nuxtjs.org/docs/2.x/directory-structure/layouts).

### `pages`

This directory contains your application views and routes. Nuxt will read all the `*.vue` files inside this directory and setup Vue Router automatically.

More information about the usage of this directory in [the documentation](https://nuxtjs.org/docs/2.x/get-started/routing).

### `plugins`

The plugins directory contains JavaScript plugins that you want to run before instantiating the root Vue.js Application. This is the place to add Vue plugins and to inject functions or constants. Every time you need to use `Vue.use()`, you should create a file in `plugins/` and add its path to plugins in `nuxt.config.js`.

More information about the usage of this directory in [the documentation](https://nuxtjs.org/docs/2.x/directory-structure/plugins).

### `static`

This directory contains your static files. Each file inside this directory is mapped to `/`.

Example: `/static/robots.txt` is mapped as `/robots.txt`.

More information about the usage of this directory in [the documentation](https://nuxtjs.org/docs/2.x/directory-structure/static).

### `store`

This directory contains your Vuex store files. Creating a file in this directory automatically activates Vuex.

More information about the usage of this directory in [the documentation](https://nuxtjs.org/docs/2.x/directory-structure/store).


This directory contains Conquest Viewer Application sample showing how to get studies, series, and images 
calling the Conquest QIDO/WADO API.

More information about the usage of this directory in [the documentation](https://nuxtjs.org/guide/routing).



## Nuxt Commands (more details in this [link]{https://nuxtjs.org/docs/get-started/commands})
* target: server (default value)  - It will needs a node server running to server the files in production
nuxt dev - Launch the development server.
nuxt build - Build and optimize your application with webpack for production.
nuxt start - Start the production server (after running nuxt build). Use it for Node.js hosting like Heroku, Digital Ocean, etc.


* target: static  - - Doesn´t needs a node server in production
nuxt dev - Launch the development server.
nuxt generate - Build the application (if needed), generate every route as a HTML file and statically export to dist/ directory (used for static hosting).
nuxt start - serve the dist/ directory like your static hosting would do (Netlify, Vercel, Surge, etc), great for testing before deploying.

The conquest front end app is configured with target static. I advice use this option.

Then if you want generate a dist folder to production just do:
nuxt generate  - this command will generate all is need to you deploy your app. The files generated goes to dist folder. You can think the dist folder as a normal web app.

To deploy the dist folder depends on service you will  use.
This [link]{https://nuxtjs.org/deployments}  explain each one. 
Additionaly, You can use any web server to deploy the dist folder as a web app.

This is some links to deploy with apache:
[link1]{https://medium.com/@helderferrari2/how-i-deployed-my-nuxt-universal-application-on-a-vps-80c0227dd2a8}
[link2]{https://stackoverflow.com/questions/70736044/how-can-i-deploy-a-nuxt-js-app-to-apache}

With target server, the pages requested by front end are rendered on server(needs node.js) and sent to front end. This option is very hard to configure in the app code to works well. 

With target static, the pages requested by front end are rendered on client browser(no needs node.js). I use this option, is very simple.