/**
 * Application GUI.
 */

// decode query
dwv.utils.decodeQuery = dwv.utils.base.decodeQuery;
// Progress
dwv.gui.displayProgress = function (/*percent*/) { /*does nothing*/ };
// get element
dwv.gui.getElement = dwv.gui.base.getElement;
// refresh
dwv.gui.refreshElement = dwv.gui.base.refreshElement;

// namespace
var dwvsimple = dwvsimple || {};

dwvsimple.Gui = function (app) {
  /**
   * Handle preset change.
   * @param {string} name The name of the new preset.
   */
  this.onChangePreset = function (name) {
    // update viewer
    app.onChangeWindowLevelPreset({currentTarget: {value: name}});
    // set selected
    this.setSelectedPreset(name);
  };
  /**
   * Handle tool change.
   * @param {string} name The name of the new tool.
   */
  this.onChangeTool = function (name) {
      app.onChangeTool({currentTarget: {value: name}});
  };
  /**
   * Handle display reset.
   */
  this.onDisplayReset = function () {
      app.onDisplayReset();
      // reset preset dropdown
      var domPresets = document.getElementById('presets');
      domPresets.selectedIndex = 0;
  };
}; // class dwvsimple.Gui

/**
 * Update preset dropdown.
 * @param {Array} presets The list of presets to use as options.
 */
dwvsimple.Gui.prototype.updatePresets = function (presets) {
  var domPresets = document.getElementById('presets');
  // clear previous
  while (domPresets.hasChildNodes()) {
      domPresets.removeChild(domPresets.firstChild);
  }
  // add new
  for (var i = 0; i < presets.length; ++i) {
    var option = document.createElement('option');
    option.value = presets[i];
    var label = presets[i];
    var key = "wl.presets."+label+".name";
    if (dwv.i18nExists(key)) {
      label = dwv.i18n(key);
    }
    option.appendChild(document.createTextNode(label));
    domPresets.appendChild(option);
  }
};

/**
 * Set the selected preset in the preset dropdown.
 * @param {string} name The name of the preset to select.
 */
dwvsimple.Gui.prototype.setSelectedPreset = function (name) {
  var domPresets = document.getElementById('presets');
  // find the index
  var index = 0;
  for (index in domPresets.options) {
    if (domPresets.options[index].value === name) {
        break;
    }
  }
  // set selected
  domPresets.selectedIndex = index;
};
