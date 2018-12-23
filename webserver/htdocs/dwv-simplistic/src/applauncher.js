/**
 * Application launcher.
 */
// namespace
var dwvsimple = dwvsimple || {};

// main application gui (global to be accessed from html)
var dwvAppGui = null;

// start app function
function startApp() {
    // translate page
    dwv.i18nPage();

    // update legend
    document.getElementById('dwvVersion').appendChild(
      document.createTextNode(dwv.getVersion()));

    // initialise the application
    var dwvApp = new dwv.App();
    dwvApp.init({
        "containerDivId": "dwv",
        "tools": ["Scroll", "ZoomAndPan", "WindowLevel"],
        "isMobile": true
    });

    // app gui
    dwvAppGui = new dwvsimple.Gui(dwvApp);

    // listen to 'load-end'
    dwvApp.addEventListener('load-end', function (/*event*/) {
        // activate tools
        document.getElementById('tools').disabled = false;
        document.getElementById('reset').disabled = false;
        document.getElementById('presets').disabled = false;
        // update presets
        dwvAppGui.updatePresets(dwvApp.getViewController().getWindowLevelPresetsNames());
    });
    // listen to 'wl-center-change'
    dwvApp.addEventListener('wl-center-change', function (/*event*/) {
        // update presets (in case new was added)
        dwvAppGui.updatePresets(dwvApp.getViewController().getWindowLevelPresetsNames());
        // suppose it is a manual change so switch preset to manual
        dwvAppGui.setSelectedPreset("manual");
    });
}

// Image decoders (for web workers)
dwv.image.decoderScripts = {
    "jpeg2000": "node_modules/dwv/decoders/pdfjs/decode-jpeg2000.js",
    "jpeg-lossless": "node_modules/dwv/decoders/rii-mango/decode-jpegloss.js",
    "jpeg-baseline": "node_modules/dwv/decoders/pdfjs/decode-jpegbaseline.js"
};

// status flags
var domContentLoaded = false;
var i18nInitialised = false;
// launch when both DOM and i18n are ready
function launchApp() {
    if ( domContentLoaded && i18nInitialised ) {
        startApp();
    }
}
// i18n ready?
dwv.i18nOnInitialised( function () {
    i18nInitialised = true;
    launchApp();
});

// check browser support
dwv.browser.check();
// initialise i18n
dwv.i18nInitialise("auto", "node_modules/dwv");

// DOM ready?
document.addEventListener("DOMContentLoaded", function (/*event*/) {
    domContentLoaded = true;
    launchApp();
});
