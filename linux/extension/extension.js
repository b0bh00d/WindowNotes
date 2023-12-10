/* exported init */
const {
  Gio
} = imports.gi;

const MR_DBUS_IFACE = `
<node>
   <interface name="org.gnome.Shell.Extensions.Windows">
      <method name="FocusInfo">
         <arg type="s" direction="out" name="win" />
      </method>
   </interface>
</node>`;

class WindowNotesEx {
  enable() {
    this._dbus = Gio.DBusExportedObject.wrapJSObject(MR_DBUS_IFACE, this);
    this._dbus.export(Gio.DBus.session, '/org/gnome/Shell/Extensions/Windows');
  }

  disable() {
    this._dbus.flush();
    this._dbus.unexport();
    delete this._dbus;
  }

  FocusInfo() {
    const properties = { 'id': 0 };

    const activeWin = global.get_window_actors().find(w => w.meta_window[`has_focus`]?.() === true);
    if (activeWin) {
      const propGetKeys = ['id', 'title'];
      propGetKeys.forEach(key => properties[key] = activeWin.meta_window[`get_${key}`]?.());
      // const propAllowsKeys = ['move', 'resize'];
      // propAllowsKeys.forEach(key => properties[key] = activeWin.meta_window[`allows_${key}`]?.());
      const propKeys = ['width', 'height', 'x', 'y', 'maximized_horizontally', 'maximized_vertically'];
      propKeys.forEach(key => properties[key] = Reflect.get(activeWin, key));
      const propMetaKeys = ['maximized_horizontally', 'maximized_vertically'];
      propMetaKeys.forEach(key => properties[key] = Reflect.get(activeWin.meta_window, key));
    }

    return JSON.stringify(properties);
  }
}

function init() {
  return new WindowNotesEx();
}
