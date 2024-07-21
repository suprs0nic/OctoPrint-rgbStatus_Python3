import sys

if __name__ == "__main__":
    # Directly ran
    import spirgbleds, time

    print("Starting RGB driver")
    spirgbleds.initialize(color=(0.0, 0.0, 0.0, 0.0), transitionsEnabled=True, transitionRefreshInterval=20, transitionTime=200)
    spirgbleds.start()

    print("Red")
    spirgbleds.set_pulsing_color(2, (1.0, 0.0, 0.0, 0.0), 2000)
    input()
    print("Green")
    spirgbleds.set_pulsing_color(2, (0.0, 1.0, 0.0, 0.0), 2000)
    input()
    print("Blue")
    spirgbleds.set_pulsing_color(2, (0.0, 0.0, 1.0, 0.0), 2000)
    input()
    print("White")
    spirgbleds.set_pulsing_color(2, (0.0, 0.0, 0.0, 1.0), 2000)
    input()
    print("Off")
    spirgbleds.set_constant_color(2, (0.0, 0.0, 0.0, 0.0))
    input()

    for j in [2, 0, 1]:
        print("Red")
        spirgbleds.set_constant_color(j, (1.0, 0.0, 0.0, 0.0))
        input()
        print("Green")
        spirgbleds.set_constant_color(j, (0.0, 1.0, 0.0, 0.0))
        input()
        print("Blue")
        spirgbleds.set_constant_color(j, (0.0, 0.0, 1.0, 0.0))
        input()
        print("White")
        spirgbleds.set_constant_color(j, (0.0, 0.0, 0.0, 1.0))
        input()
        print("Off")
        spirgbleds.set_constant_color(j, (0.0, 0.0, 0.0, 0.0))
        input()

    print("Stopping RGB driver")
    spirgbleds.stop()
else:
    # OctoPrint import
    import rgbstatusplugin

    __plugin_name__ = "Leapfog RGB status"
    def __plugin_load__():
        global __plugin_implementation__
        __plugin_implementation__ = rgbstatusplugin.RgbStatusPlugin()

        global __plugin_hooks__
        __plugin_hooks__ = {
            "octoprint.comm.protocol.gcode.sent": __plugin_implementation__.on_gcode_sent,
        }
