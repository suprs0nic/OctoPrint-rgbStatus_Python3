from binascii import unhexlify
from flask import jsonify

from octoprint.plugin import SettingsPlugin, BlueprintPlugin, ShutdownPlugin

import spirgbleds

from octoprint.events import Events
#from octoprint_lui.constants import LuiEvents

from constants import *

class RgbStatusPlugin(SettingsPlugin, BlueprintPlugin, ShutdownPlugin):
    def __init__(self):
        self._heating = False
        self._default_color = (0., 0., 1., 0.) # Blue
        self._lights_enabled = False

    def initialize(self):
        # Initialize with default color "off", and enable transitions of 200ms with a refresh rate of 20ms
        spirgbleds.initialize(color=(0.0,0.0,0.0,0.0), transitionsEnabled=True, transitionRefreshInterval=20, transitionTime=200)
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
            "idle_color":       "#0000FF",      # Blue
            "paused_color":     "#00BFBE",      # Cyan
            "error_color":      "#FF0000",      # Red
            "heating_color":    "#FFC901",      # Orange
            "printing_color":   "#000000FF",    # White
            "finished_color":   "#00FF00"       # Green
        }
    
    # Begin RGB status API

    @BlueprintPlugin.route("/preview/<string:hex_color>", methods=["POST"])
    def preview(self, color_hex):
        self._logger.debug("Previewing color: %s" % color_hex)
        self._send_color(RgbTarget.BOTH, RgbPatterns.NORMAL_PULSING, color_hex)

    @BlueprintPlugin.route("/restore", methods=["POST"])
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
        self._send_color(RgbTarget.BOTH, RgbPatterns.CONSTANT, (0.0,0.0,0.0,0.0))
        self._logger.debug("Switching the lights off")

    def on_startup(self):
        color_hex = self._settings.get(["startup_color"])
        self._logger.debug("Setting startup color: %s" % color_hex)
        self._send_color(RgbTarget.BOTH, RgbPatterns.NORMAL_PULSING, color_hex)

    def on_idle(self):
        color_hex = self._settings.get(["idle_color"])
        self._logger.debug("Setting idle color: %s" % color_hex)
        self._send_color(RgbTarget.BOTH, RgbPatterns.CONSTANT, color_hex)

    def on_error(self):
        color_hex = self._settings.get(["error_color"])
        self._logger.debug("Setting error color: %s" % color_hex)
        self._send_color(RgbTarget.BOTH, RgbPatterns.SLOW_PULSING, color_hex)

    def on_heating(self):
        color_hex = self._settings.get(["heating_color"])
        self._logger.debug("Setting heating color: %s" % color_hex)
        self._send_color(RgbTarget.BOTH, RgbPatterns.NORMAL_PULSING, color_hex)

    def on_printing(self):
        color_hex = self._settings.get(["printing_color"])
        self._logger.debug("Setting printing color: %s" % color_hex)
        self._send_color(RgbTarget.BOTH, RgbPatterns.CONSTANT, color_hex)

    def on_paused(self):
        color_hex = self._settings.get(["paused_color"])
        self._logger.debug("Setting printing color: %s" % color_hex)
        self._send_color(RgbTarget.BOTH, RgbPatterns.CONSTANT, color_hex)

    def on_finished(self):
        color_hex = self._settings.get(["finished_color"])
        self._logger.debug("Setting finished color: %s" % color_hex)
        self._send_color(RgbTarget.BOTH, RgbPatterns.CONSTANT, color_hex)

    # End color mapping

    def _find_current_state(self, event = None, payload = None):
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
            # In all other sitations, actually determine the state
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
                if not event == Events.FILE_SELECTED and data.get("progress",{}).get("completion", 0) == 100:
                    self.on_finished()
                else:
                    self.on_idle()

    def _set_events(self):
        """
        Sets the OctoPrint events on which to re-determine the state
        """
        state_updates = [Events.CLIENT_OPENED, Events.PRINT_STARTED, Events.PRINT_PAUSED, Events.PRINT_RESUMED,
                         Events.PRINT_DONE, Events.PRINT_FAILED, Events.FILE_SELECTED, Events.CONNECTED, Events.DISCONNECTED, Events.ERROR]

        for event in state_updates:
            self._event_bus.subscribe(event, self._find_current_state)

    def _send_color(self, target, pattern, color):
        """
        Sends the color to the SPI driver, if enabled. Otherwise keeps the lights off.
        Accepts both RGB(W) tuples and hex codes
        """

        if not self._lights_enabled:
            spirgbleds.set_constant_color(RgbTarget.BOTH, (0.0,0.0,0.0,0.0))

        # Ensure we work with a tuple of four floats
        if isinstance(color, basestring):
            color = self._parse_hex(color)
        elif len(color) == 3:
            color = color + (0.0,) # By default, keep the white channel low

        if pattern == RgbPatterns.CONSTANT:
            spirgbleds.set_constant_color(target, color)
        elif pattern == RgbPatterns.FAST_PULSING:
            spirgbleds.set_pulsing_color(target, color, 1000)
        elif pattern == RgbPatterns.NORMAL_PULSING:
            spirgbleds.set_pulsing_color(target, color, 2000)
        elif pattern == RgbPatterns.SLOW_PULSING:
            spirgbleds.set_pulsing_color(target, color, 4000)

    def _parse_hex(self, hex_color):
        """
        Parse a hex color code and return a tuple with RGBW values (ranging 0.0 - 1.0).
        Leading # are accepted. The fourth byte indicates white and may be omitted.
        In this case, 0.0 is returned for the white channel.
        """
        # Remove any #
        hex_color = hex_color.lstrip("#")
        n = len(hex_color)

        if n not in [6, 8]:
            return self._default_color
        
        # Split bytestring into floats in 0.0 - 1.0 range
        color = unhexlify(hex_color)

        r  = ord(color[0]) / 255.
        g  = ord(color[1]) / 255.
        b  = ord(color[2]) / 255.

        if n == 8:
            w  = ord(color[3]) / 255.
        else:
            w = 0.

        return (r, g, b, w)
