// https://stackoverflow.com/questions/33001192/how-to-send-a-string-to-a-gnome-shell-extension
const { Gio } = imports.gi;
const Lang = imports.lang;
// https://github.com/olebowle/gnome-shell-timer/blob/master/timer%40olebowle.gmx.com/extension.js
const Mainloop = imports.mainloop;
const Shell = imports.gi.Shell;

// https://medium.com/cesar-update/exposing-a-d-bus-interface-in-linux-part-2-90374a9e381b

const States = {
    None: 'None',
    Active: 'Active',
    Title: 'Title',
    MoveStarted: 'MoveStarted',
    Moving: 'Moving',
    MoveEnded: 'MoveEnded',
    ResizeStarted: 'ResizeStarted',
    Resizing: 'Resizing',
    ResizeEnded: 'ResizeEnded'
}

const NotificationStates = [
    States.MoveStarted,
    States.MoveEnded,
    States.ResizeStarted,
    States.ResizeEnded,
    States.Active,
    States.Title
    ];

const MovingStates = [
    States.Moving,
    States.MoveStarted
    ];

const ResizingStates = [
    States.Resizing,
    States.ResizeStarted
    ];

const MR_DBUS_IFACE = `
<node>
   <interface name="org.gnome.Shell.Extensions.Windows">
      <method name="FocusInfo">
         <arg type="s" direction="out" name="data" />
      </method>
   </interface>
</node>`;

// function makeStruct(names) {
//   var names = names.split(' ');
//   var count = names.length;
//   function constructor() {
//     for (var i = 0; i < count; i++) {
//       this[names[i]] = arguments[i];
//     }
//   }
//   return constructor;
// }

function WindowNotesEx() {
    this._init();
}

WindowNotesEx.prototype = {
  _init: function() {
    // :: add window titles that should be ignored in events; these
    // :: are windows where a notes context wouldn't make sense
    this._filterEvents = ["WindowNotes", "RemindMeLater"];

    // this.WindowData = makeStruct("state id title top left width height");
    this._lastState = States.None;
    this._lastWindowData = this._newWindowData();
  },

  _newWindowData: function() {
    return {"state": States.None, "id": 0, "title": "", "x": 0, "y": 0, "width": 0, "height": 0};
  },

  _checkStateTimer: function() {
    if(this._enabled) {
      this._checkState();
      Mainloop.timeout_add_seconds(1, Lang.bind(this, this._checkStateTimer));
    }
  },

  _checkState: function() {
    let winData = this._newWindowData();

    const activeWin = global.get_window_actors().find(w => w.meta_window[`has_focus`]?.() === true);
    if (activeWin) {
      const propGetKeys = ['id', 'title'];
      propGetKeys.forEach(key => winData[key] = activeWin.meta_window[`get_${key}`]?.());
      if(!this._filterEvents.includes(winData["title"])) {
        // const propAllowsKeys = ['move', 'resize'];
        // propAllowsKeys.forEach(key => properties[key] = activeWin.meta_window[`allows_${key}`]?.());
        const propKeys = ['width', 'height', 'x', 'y', 'maximized_horizontally', 'maximized_vertically'];
        propKeys.forEach(key => winData[key] = Reflect.get(activeWin, key));
        const propMetaKeys = ['maximized_horizontally', 'maximized_vertically'];
        propMetaKeys.forEach(key => winData[key] = Reflect.get(activeWin.meta_window, key));
      }
      else {
        winData['title'] = '';
        winData['id'] = 0;
      }

      // figure out the delta (if any) from the last read

      if(!this._lastWindowData['id'] || winData['id'] !== this._lastWindowData['id']) {
        winData['state'] = States.Active;
      }
      else if(winData['title'] !== this._lastWindowData['title']) {
        winData['state'] = States.Title;
      }
      else if((winData['width'] !== this._lastWindowData['width']) || (winData['height'] !== this._lastWindowData['height'])) {
        winData['state'] = States.Resizing;
      }
      else if((winData['x'] !== this._lastWindowData['x']) || (winData['y'] !== this._lastWindowData['y'])) {
        winData['state'] = States.Moving;
      }

      if(winData['state'] === States.Moving) {
        if(!MovingStates.includes(this._lastWindowData['state'])) {
          winData['state'] = States.MoveStarted;
        }
      }
      else if(winData['state'] === States.Resizing) {
        if(!ResizingStates.includes(this._lastWindowData['state'])) {
          winData['state'] = States.ResizeStarted;
        }
      }
      else if(winData['state'] === States.None) {
        if(MovingStates.includes(this._lastWindowData['state'])) {
          winData['state'] = States.MoveEnded;
        }
        else if(ResizingStates.includes(this._lastWindowData['state'])) {
          winData['state'] = States.ResizeEnded;
        }
      }

      if(NotificationStates.includes(winData['state'])) {
        // let text = JSON.stringify(winData);
        // let msg = new Gio.DBusMessage.new();
        // msg.set_body(text);
        // msg.set_flags(Gio.DBusCapabilityFlags.None);
        // let conn = this._dbus.get_connection();
        // conn.send_message(msg);
        console.log(JSON.stringify(winData));
      }

      this._lastWindowData = winData;
    }
  },

  enable: function() {
    // https://gjs-docs.gnome.org/
    this._dbus = Gio.DBusExportedObject.wrapJSObject(MR_DBUS_IFACE, this);
    this._dbus.export(Gio.DBus.session, '/org/gnome/Shell/Extensions/Windows');

    // :: I'd love to make this all event-driven, but GNOME Shell doesn't
    // :: provide enough events to cover all the cases I need to monitor
    // :: (in fact, it's sadly lacking in events), so, we poll
    // let callbackID = global.display.connect('notify::focus-window', Lang.bind(this, this._focus_changed));

    this._enabled = true;

    // :: this starts the internal polling and event-tracking system
    // this._checkStateTimer();
  },

  disable: function() {
    this._dbus.flush();
    this._dbus.unexport();
    delete this._dbus;
    this._enabled = false;
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
      if(!this._filterEvents.includes(properties["title"])) {
        // const propAllowsKeys = ['move', 'resize'];
        // propAllowsKeys.forEach(key => properties[key] = activeWin.meta_window[`allows_${key}`]?.());
        const propKeys = ['width', 'height', 'x', 'y', 'maximized_horizontally', 'maximized_vertically'];
        propKeys.forEach(key => properties[key] = Reflect.get(activeWin, key));
        const propMetaKeys = ['maximized_horizontally', 'maximized_vertically'];
        propMetaKeys.forEach(key => properties[key] = Reflect.get(activeWin.meta_window, key));
      }
      else {
        delete properties['title'];
        properties['id'] = 0;
      }
    }

    return JSON.stringify(properties);
  }
}

function init() {
  return new WindowNotesEx();
}
