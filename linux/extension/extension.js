/* exported init */
const { Gio } = imports.gi;
const Lang = imports.lang;
const Shell = imports.gi.Shell;

const MR_DBUS_IFACE = `
<node>
   <interface name="org.gnome.Shell.Extensions.Windows">
      <method name="FocusInfo">
         <arg type="s" direction="out" name="win" />
      </method>
   </interface>
</node>`;

function WindowNotesEx() {
    this._init();
}

WindowNotesEx.prototype = {
  _init: function() {
    // add window titles that should be ignored in events
    this.filter_events = ["WindowNotes", "RemindMeLater"];
  },

  enable: function() {
    this._dbus = Gio.DBusExportedObject.wrapJSObject(MR_DBUS_IFACE, this);
    this._dbus.export(Gio.DBus.session, '/org/gnome/Shell/Extensions/Windows');

    // let callbackID = global.display.connect('notify::focus-window', Lang.bind(this, this._focus_changed));
  },

  disable: function() {
    this._dbus.flush();
    this._dbus.unexport();
    delete this._dbus;
  },

  // https://wiki.archlinux.org/title/Desktop_notifications
  // _focus_changed: function() {
  //   let focus_app = Shell.WindowTracker.get_default().focus_app?.get_name();
  //   if (focus_app) {
  //       console.log(focus_app);
  //   }
  // },

  FocusInfo: function() {
    const properties = { 'id': 0 };

    const activeWin = global.get_window_actors().find(w => w.meta_window[`has_focus`]?.() === true);
    if (activeWin) {
      const propGetKeys = ['id', 'title'];
      propGetKeys.forEach(key => properties[key] = activeWin.meta_window[`get_${key}`]?.());
      if(!this.filter_events.includes(properties["title"])) {
        // const propAllowsKeys = ['move', 'resize'];
        // propAllowsKeys.forEach(key => properties[key] = activeWin.meta_window[`allows_${key}`]?.());
        const propKeys = ['width', 'height', 'x', 'y', 'maximized_horizontally', 'maximized_vertically'];
        propKeys.forEach(key => properties[key] = Reflect.get(activeWin, key));
        const propMetaKeys = ['maximized_horizontally', 'maximized_vertically'];
        propMetaKeys.forEach(key => properties[key] = Reflect.get(activeWin.meta_window, key));
      }
      else {
        delete properties.title;
        properties.id = 0;
      }
    }

    return JSON.stringify(properties);
  }
}

function init() {
  return new WindowNotesEx();
}
