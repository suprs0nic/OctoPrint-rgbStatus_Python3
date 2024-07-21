from binascii import unhexlify
from flask import jsonify, make_response

from octoprint.plugin import SettingsPlugin, BlueprintPlugin, ShutdownPlugin

import spirgbleds
import json
import os.path

from octoprint.events import Events
# from octoprint_lui.constants import LuiEvents

from constants import *

class RgbStatusPlugin(SettingsPlugin, BlueprintPlugin, ShutdownPlugin):
    def __init__(self):
        self._heating = False
        self._default_color = (0.0, 0.0, 1.0, 0.0)  # Blue
        self._lights_enabled = False

    def initialize(self):
        # Initialize with default color "off", and enable transitions of 200ms with a refresh rate of 20ms
        spirgbleds.initialize(color=(0.0, 0.0, 0.0, 0.0), transitionsEnabled=True, transitionRefreshInterval=20, transitionTime=200)
        spirgbleds.start()

        self._lights_enabled = self._settings.get_boolean(["lights_enabled"])
        self._set_events()
        self.on_startup()

    def on_shutdown(self):
        self.on_lights_off()
        spirgbleds.stop()

    def on_gcode_sent(self, comm_instance, phase, cmd, cmd_type, gcode):
        heating = gcode in ["M109", "M190"]

        if heating != self._heating:
            self._heating = heating
            self._find_current_state()

    def get_settings_defaults(self):
        return {
            "lights_enabled":   True,
            "startup_color":    "#0000FF",      # Blue
            "startup_pattern": RgbPatterns.NORMAL_PULSING,
            "idle_color":       "#0000FF",      # Blue
            "idle_pattern":     RgbPatterns.CONSTANT,
            "paused_color":     "#00BFBE",      # Cyan
            "paused_pattern":   RgbPatterns.CONSTANT,
            "error_color":      "#FF0000",      # Red
            "error_pattern":    RgbPatterns.SLOW_PULSING,
            "heating_color":    "#FF8000",      # Orange
            "heating_pattern":  RgbPatterns.NORMAL_PULSING,
            "printing_color":   "#000000FF",    # White
            "printing_pattern": RgbPatterns.CONSTANT,
            "finished_color":   "#00FF00",      # Green
            "finished_pattern": RgbPatterns.CONSTANT
        }

    # Begin RGB status API

    @BlueprintPlugin.route("/preview/<string:hex_color>/<string:pattern>", methods=["GET"])
    def preview(self, hex_color, pattern):
        self._logger.debug("Previewing color: %s" % hex_color)
        self._send_color(RgbTarget.BOTH, int(pattern), "#" + hex_color)
        return make_response(jsonify({"color": hex_color}))

    @BlueprintPlugin.route("/setcolor/<string:name>/<string:hex_color>/<string:pattern>", methods=["GET"])
    def setcolor(self, name, hex_color, pattern):
        setcolor = name + "_color"
        setpattern = name + "_pattern"
        self._settings.set([setcolor], "#" + hex_color)
        self._settings.set([setpattern], int(pattern))
        self._settings.save()
        self.restore()
        return make_response(jsonify({"color": self._settings.get([setcolor])}))

    @BlueprintPlugin.route("/getcolor/<string:name>", methods=["GET"])
    def getcolor(self, name):
        getcolor = name + "_color"
        getpattern = name + "_pattern"
        color = self._settings.get([getcolor])
        pattern = self._settings.get([getpattern])
        return make_response(jsonify({"color": color, "pattern": pattern}))

    @BlueprintPlugin.route("/getdefault/<string:name>", methods=["GET"])
    def getdefault(self, name):
        getcolor = name + "_color"
        getpattern = name + "_pattern"
        defaultvars = self.get_settings_defaults()
        color = defaultvars[getcolor]
        pattern = defaultvars[getpattern]
        return make_response(jsonify({"color": color, "pattern": pattern}))

    @BlueprintPlugin.route("/setdefault/all", methods=["GET"])
    def setdefault_all(self):
        defaultvars = self.get_settings_defaults()
        # self._settings.set(defaultvars)
        for key in defaultvars.keys():
            self._settings.set([key], defaultvars.get(key))
        self._settings.save()
        self.restore()
        return make_response(jsonify({"Settings": defaultvars.get(key)}))

    @BlueprintPlugin.route("/restore", methods=["POST"])
    def restorefromsite(self):
        self._find_current_state()
        return jsonify(lights_enabled=self._lights_enabled)

    def restore(self):
        self._find_current_state()

    @BlueprintPlugin.route("/status", methods=["GET"])
    def status(self):
        return jsonify(lights_enabled=self._lights_enabled)

    @BlueprintPlugin.route("/on", methods=["POST"])
    def on(self):
        self._lights_enabled = True
        self._find_current_state()

        self._settings.set(["lights_enabled"], True)
        self._settings.save()

        return jsonify(lights_enabled=self._lights_enabled)

    @BlueprintPlugin.route("/off", methods=["POST"])
    def off(self):
        self._lights_enabled = False
        self.on_lights_off()

        self._settings.set(["lights_enabled"], False)
        self._settings.save()

        return jsonify(lights_enabled=self._lights_enabled)

    # End RGB status API

    # Begin color mapping
    # TODO: If we don't need extra logic, translate these methods into a
    # dict and lookup the color/pattern/target

    def on_lights_off(self):
        self._send_color(RgbTarget.BOTH, RgbPatterns.CONSTANT, (0.0, 0.0, 0.0, 0.0))
        self._logger.debug("Switching the lights off")

    def on_startup(self):
        color_hex = self._settings.get(["startup_color"])
        pattern = self._settings.get(["startup_pattern"])
        self._logger.debug("Setting startup color: %s" % color_hex)
        self._send_color(RgbTarget.BOTH, pattern, color_hex)

    def on_idle(self):
        color_hex = self._settings.get(["idle_color"])
        pattern = self._settings.get(["idle_pattern"])
        self._logger.debug("Setting idle color: %s" % color_hex)
        self._send_color(RgbTarget.BOTH, pattern, color_hex)

    def on_error(self):
        color_hex = self._settings.get(["error_color"])
        pattern = self._settings.get(["error_pattern"])
        self._logger.debug("Setting error color: %s" % color_hex)
        self._send_color(RgbTarget.BOTH, pattern, color_hex)

    def on_heating(self):
        color_hex = self._settings.get(["heating_color"])
        pattern = self._settings.get(["heating_pattern"])
        self._logger.debug("Setting heating color: %s" % color_hex)
        self._send_color(RgbTarget.BOTH, pattern, color_hex)

    def on_printing(self):
        color_hex = self._settings.get(["printing_color"])
        pattern = self._settings.get(["printing_pattern"])
        self._logger.debug("Setting printing color: %s" % color_hex)
        self._send_color(RgbTarget.BOTH, pattern, color_hex)

    def on_paused(self):
        color_hex = self._settings.get(["paused_color"])
        pattern = self._settings.get(["paused_pattern"])
        self._logger.debug("Setting printing color: %s" % color_hex)
        self._send_color(RgbTarget.BOTH, pattern, color_hex)

    def on_finished(self):
        color_hex = self._settings.get(["finished_color"])
        pattern = self._settings.get(["finished_pattern"])
        self._logger.debug("Setting finished color: %s" % color_hex)
        self._send_color(RgbTarget.BOTH, pattern, color_hex)

    # End color mapping

    def _find_current_state(self, event=None, payload=None):
        """
        Find the current state of the printer, and set the light color
        according to it. Switches lights off if they are not enabled
        """

        # Switch lights off if the lights are not enabled
        if not self._lights_enabled:
            self.on_lights_off()
        # First try to act based on an event
        elif event == Events.PRINT_DONE:
            self.on_finished()
        elif event == Events.PRINT_FAILED:
            self.on_error()
        else:
            # In all other situations, actually determine the state
            if self._printer.is_error():
                self.on_error()
            elif self._printer.is_printing():
                if self._heating:
                    self.on_heating()
                else:
                    self.on_printing()
            elif self._printer.is_paused():
                self.on_paused()
            else:
                data = self._printer.get_current_data()
                if not event == Events.FILE_SELECTED and data.get("progress", {}).get("completion", 0) == 100:
                    self.on_finished()
                else:
                    self.on_idle()

    def _set_events(self):
        """
        Sets
